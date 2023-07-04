#include <ntifs.h>
#include <ntddk.h>
#include <intrin.h>

struct _KDPC_LIST
{
	struct _SINGLE_LIST_ENTRY ListHead;                                     //0x0
	struct _SINGLE_LIST_ENTRY* LastEntry;                                   //0x8
};

struct _KDPC_DATA
{
	struct _KDPC_LIST DpcList;                                              //0x0
	ULONGLONG DpcLock;                                                      //0x10
	volatile LONG DpcQueueDepth;                                            //0x18
	ULONG DpcCount;                                                         //0x1c
	struct _KDPC* volatile ActiveDpc;                                       //0x20
};

auto EnumerateDpc(ULONG_PTR)->ULONG_PTR;

EXTERN_C 
auto DriverUnload(_In_ PDRIVER_OBJECT DriverObject) ->void {
	if (DriverObject->DeviceObject != nullptr)
		IoDeleteDevice(DriverObject->DeviceObject);

	return;
}

EXTERN_C
auto DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING) -> NTSTATUS {
	DriverObject->DriverUnload = DriverUnload;

	//EnumerateDpc();

	KeIpiGenericCall(EnumerateDpc, 0);

	return STATUS_SUCCESS;
}

auto EnumerateDpc(ULONG_PTR Arg) -> ULONG_PTR {

	auto irql = KeRaiseIrqlToDpcLevel();

	// Let's queue 50 DPCs
	for (UINT64 idx = 0; idx < 50; ++idx) {
		auto dpc = reinterpret_cast<KDPC*>(ExAllocatePool(NonPagedPool, sizeof KDPC));
		if (dpc == nullptr)		return Arg;
		
		KeInitializeDpc(dpc, [](_In_ struct _KDPC* Dpc, _In_opt_ PVOID DeferredContext,
			_In_opt_ PVOID SystemArgument1, _In_opt_ PVOID SystemArgument2) {
				DbgPrint("[%s] dpc : %p | DeferredContext : %p | SystemArgument1 : %p | SystemArgument2 : %p\n",
					__FUNCTION__, static_cast<PVOID>(Dpc), DeferredContext, SystemArgument1, SystemArgument2);
			}, nullptr);

		KeInsertQueueDpc(dpc, reinterpret_cast<PVOID>(idx), reinterpret_cast<PVOID>(idx * idx));
	}

		auto pcr = KeGetPcr();
		DbgPrint("pcr : %p\n", static_cast<PVOID>(pcr));

		auto current_prcb = pcr->CurrentPrcb;
		DbgPrint("prcb : %p\n", static_cast<PVOID>(current_prcb));

		auto dpc_data = reinterpret_cast<_KDPC_DATA*>((UCHAR*)current_prcb + 0x2e00);
		DbgPrint("dpc_data : %p\n", static_cast<PVOID>(dpc_data));

		auto dpc_normal = &dpc_data[DPC_NORMAL];
		auto dpc_threaded = &dpc_data[DPC_THREADED];

		for (auto entry = dpc_normal->DpcList.ListHead.Next;
			entry != nullptr; entry = entry->Next) {

			auto dpc_entry = CONTAINING_RECORD(entry, KDPC, DpcListEntry);
			DbgPrint("[DPC_NORMAL] dpc : %p\n", static_cast<PVOID>(dpc_entry));
		}

		DbgPrint("\n=======================================================================\n\n");

		for (auto entry = dpc_threaded->DpcList.ListHead.Next;
			entry != nullptr; entry = entry->Next) {

			auto dpc_entry = CONTAINING_RECORD(entry, KDPC, DpcListEntry);
			DbgPrint("[DPC_THREADED] dpc : %p\n", static_cast<PVOID>(dpc_entry));
		}

		DbgPrint("\n=======================================================================\n\n");

		KeLowerIrql(irql);


	return Arg;
}