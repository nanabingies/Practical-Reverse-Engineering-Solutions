
// Enumerate all Timers on the system with support for multicore systems
#include <ntifs.h>
#include <ntddk.h>
#include <intrin.h>

//0x20 bytes (sizeof)
struct _KTIMER_TABLE_ENTRY
{
	ULONGLONG Lock;                                                         //0x0
	struct _LIST_ENTRY Entry;                                               //0x8
	union _ULARGE_INTEGER Time;                                             //0x18
};

//0x2200 bytes (sizeof)
struct _KTIMER_TABLE
{
	struct _KTIMER* TimerExpiry[64];                                        //0x0
	struct _KTIMER_TABLE_ENTRY TimerEntries[256];                           //0x200
};

KDPC*	dpc_array[50];
KTIMER*	timer_array[50];

auto EnumerateTimers(ULONG_PTR)->ULONG_PTR;
auto CreateTimers()->void;
auto FreeTimers()->void;

EXTERN_C
auto DriverUnload(_In_ PDRIVER_OBJECT DriverObject) -> void {
	if (DriverObject->DeviceObject != nullptr)
		IoDeleteDevice(DriverObject->DeviceObject);

	return;
}

EXTERN_C 
auto DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING) -> NTSTATUS {
	DriverObject->DriverUnload = DriverUnload;

	CreateTimers();

	KeIpiGenericCall(EnumerateTimers, 0);

	FreeTimers();

	return STATUS_SUCCESS;
}

auto EnumerateTimers(ULONG_PTR Arg) -> ULONG_PTR {

	auto pcr = KeGetPcr();

	auto prcb = pcr->CurrentPrcb;

	auto timer_table = reinterpret_cast<_KTIMER_TABLE*>((UCHAR*)prcb + 0x3680);

	auto timer_entries = &timer_table->TimerEntries->Entry;

	for (auto timer_entry = timer_entries->Flink; 
		timer_entry != timer_entries; timer_entry = timer_entry->Flink) {

		auto timer = CONTAINING_RECORD(timer_entry, _KTIMER, TimerListEntry);
		DbgPrint("\t[>] _KTIMER : %p | Dpc : %p\n",
			static_cast<PVOID>(timer), static_cast<PVOID>(timer->Dpc));
	}

	DbgPrint("\n================================================================\n\n");

	return Arg;
}

#define TIMER_TAG	'time'
#define DPC_TAG		'dpc'
auto CreateTimers() -> void {

	NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

	for (auto idx = 0; idx < 50; ++idx) {
		dpc_array[idx] = reinterpret_cast<KDPC*>
			(ExAllocatePoolWithTag(NonPagedPool, sizeof KDPC, DPC_TAG));
		if (dpc_array[idx] == nullptr)	return;

		KeInitializeDpc(dpc_array[idx], [](_In_ _KDPC* Dpc, _In_opt_ PVOID DeferredContext,
			_In_opt_ PVOID SystemArgument1, _In_opt_ PVOID SystemArgument2) {
				DbgPrint("Executing timer with associated dpc %p | DeferredContext %p | SystemArgument1 %p | SystemArgument2 %p\n",
					static_cast<PVOID>(Dpc), DeferredContext, SystemArgument1, SystemArgument2);
			}, reinterpret_cast<PVOID>(idx));
		KeInsertQueueDpc(dpc_array[idx], nullptr, nullptr);

		timer_array[idx] = reinterpret_cast<KTIMER*>
			(ExAllocatePoolWithTag(NonPagedPool, sizeof KTIMER, TIMER_TAG));
		if (timer_array[idx] == nullptr)
			return;

		LARGE_INTEGER li_timer{};
		li_timer.QuadPart = 100 * 100;

		KeInitializeTimer(timer_array[idx]);
		KeSetTimer(timer_array[idx], li_timer, dpc_array[idx]);

		

	}

	return;
}

auto FreeTimers() -> void {
	for (auto dpc : dpc_array) {
		if (dpc)
			ExFreePoolWithTag(dpc, DPC_TAG);
	}

	for (auto timer : timer_array) {
		if (timer)
			ExFreePoolWithTag(timer, TIMER_TAG);
	}
}