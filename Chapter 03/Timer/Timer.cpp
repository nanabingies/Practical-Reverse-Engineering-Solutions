
// Exercise: Write a driver to enumerate the load module list every 10 minutes

#include <ntddk.h>

#define TAG_TIMER 'time'
#pragma warning(disable : 6387)
#pragma warning(disable : 6273)

typedef unsigned short WORD;

typedef struct SYSTEM_MODULE {
	ULONG                Reserved1;
	ULONG                Reserved2;
#ifdef _WIN64
	ULONG				Reserved3;
#endif
	PVOID                ImageBaseAddress;
	ULONG                ImageSize;
	ULONG                Flags;
	WORD                 Id;
	WORD                 Rank;
	WORD                 w018;
	WORD                 NameOffset;
	CHAR                 Name[MAXIMUM_FILENAME_LENGTH];
}SYSTEM_MODULE, * PSYSTEM_MODULE;

typedef struct SYSTEM_MODULE_INFORMATION {
	ULONG                ModulesCount;
	SYSTEM_MODULE        Modules[1];
} SYSTEM_MODULE_INFORMATION, * PSYSTEM_MODULE_INFORMATION;

EXTERN_C NTSTATUS ZwQuerySystemInformation(
	ULONG SystemInformationClass,
	PVOID SystemInformation,
	ULONG SystemInformationLength,
	ULONG * ReturnLength);

NTSTATUS TimeModules();
VOID TimerRoutine(_In_ _KDPC* Dpc, _In_opt_ PVOID, _In_opt_ PVOID, _In_opt_ PVOID);

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT, _In_ PUNICODE_STRING) {

	auto ntStatus = TimeModules();
	if (!NT_SUCCESS(ntStatus)) {
		DbgPrint("[TimeModules] failed with error code %X\n", ntStatus);
		return ntStatus;
	}

	return STATUS_SUCCESS;
}

NTSTATUS TimeModules() {

	auto timer= reinterpret_cast<KTIMER*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(KTIMER), TAG_TIMER));
	if (!timer) {
		DbgPrint("[ExAllocatePoolWithTag] failed.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	//DbgPrint("[Debugging] Timer : %p\n", static_cast<PVOID>(timer));

	//auto irql = KeRaiseIrqlToDpcLevel();
	LARGE_INTEGER dueTime;
	dueTime.QuadPart = -10000000;

	auto dpc = reinterpret_cast<KDPC*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(KDPC), TAG_TIMER));
	if (!dpc) {
		DbgPrint("[ExAllocatePoolWithTag] failed.\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}
	//DbgPrint("[Debugging] Dpc : %p\n", static_cast<PVOID>(dpc));

	//__debugbreak();

	KeInitializeDpc(static_cast<PRKDPC>(dpc), TimerRoutine, nullptr);
	//DbgPrint("[Debugging] DeferredRoutine : %p\n", dpc->DeferredRoutine);
	//DbgPrint("[Debugging] DeferredContext : %p\n", dpc->DeferredContext);
	//DbgPrint("[Debugging] DpcData : %p\n", dpc->DpcData);

	KeInitializeTimerEx(static_cast<PKTIMER>(timer), NotificationTimer);

	KeSetTimerEx(static_cast<PKTIMER>(timer), dueTime, 1000 * 60, static_cast<PKDPC>(dpc));

	//KeLowerIrql(irql);

	return STATUS_SUCCESS;
}

VOID TimerRoutine(_In_ _KDPC* Dpc, _In_opt_ PVOID, _In_opt_ PVOID, _In_opt_ PVOID) {
	DbgPrint("[+] In DPC %p\n", (VOID*)Dpc);
	ULONG returnLength;
	ZwQuerySystemInformation(11, NULL, 0, &returnLength);
	auto buffer = ExAllocatePoolWithTag(NonPagedPool, returnLength, TAG_TIMER);
	if (!buffer)
		return;
	ZwQuerySystemInformation(11, buffer, returnLength, &returnLength);
	auto ModuleInfo = reinterpret_cast<SYSTEM_MODULE_INFORMATION*>(buffer);
	for (ULONG i = 0; i < ModuleInfo->ModulesCount; i++) {
		ANSI_STRING asString{};
		RtlInitAnsiString(&asString, ModuleInfo->Modules[i].Name);
		UNICODE_STRING usString{};
		RtlAnsiStringToUnicodeString(&usString, &asString, TRUE);
		DbgPrint("[>] ModuleName : %wZ\n", &usString);
		RtlFreeUnicodeString(&usString);
	}
	ExFreePoolWithTag(buffer, TAG_TIMER);
}