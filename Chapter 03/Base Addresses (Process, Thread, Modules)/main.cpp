#include <ntifs.h>
#include <ntddk.h>
#pragma warning(disable : 4100)

EXTERN_C NTKERNELAPI PVOID PsGetProcessSectionBaseAddress(__in PEPROCESS Process);

auto GetBaseAddress(_In_ HANDLE ProcessId) -> PVOID {
	PEPROCESS Eprocess;
	PVOID baseAddress = 0;

	auto _ntStatus = PsLookupProcessByProcessId(ProcessId, (PEPROCESS*)&Eprocess);
	if (NT_SUCCESS(_ntStatus)) {
		KAPC_STATE apcState{};
		KeStackAttachProcess((PRKPROCESS)Eprocess, (PRKAPC_STATE)&apcState);
		baseAddress = PsGetProcessSectionBaseAddress(Eprocess);
		KeUnstackDetachProcess((PRKAPC_STATE)&apcState);
	}

	return baseAddress;
}

void PloadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo) {
	DbgPrint("[+] %s => \n", __FUNCTION__);
	
	auto baseAddress = ImageInfo->ImageBase;
	DbgPrint("\t[>] Image Name : %wZ\n", FullImageName);
	DbgPrint("\t[>] ProcessId: %p\n", ProcessId);
	DbgPrint("\t[>] BaseAddress : %p\n", baseAddress);

	DbgPrint("[+] <= %s\n", __FUNCTION__);
}

void PcreateProcessNotifyRoutineEx(_Inout_ PEPROCESS Process, _In_ HANDLE ProcessId, _Inout_opt_ PPS_CREATE_NOTIFY_INFO CreateInfo) {
	DbgPrint("[+] %s => \n", __FUNCTION__);

	if (CreateInfo) {	// Process is being created
		auto baseAddress = GetBaseAddress(ProcessId);
		auto imageName = CreateInfo->ImageFileName;
		DbgPrint("\t[>] Image Name : %wZ\n", imageName);
		DbgPrint("\t[>] ProcessId: %p\n", ProcessId);
		DbgPrint("\t[>] BaseAddress : %p\n", baseAddress);
	}

	DbgPrint("[+] <= %s\n", __FUNCTION__);
}

void PcreateThreadNotifyRoutine(_In_ HANDLE ProcessId, _In_ HANDLE ThreadId, _In_ BOOLEAN Create) {
	DbgPrint("[+] %s => \n", __FUNCTION__);

	if (Create) {
		auto baseAddress = GetBaseAddress(ProcessId);
		DbgPrint("\t[>] ThreadId: %p\n", ThreadId);
		DbgPrint("\t[>] BaseAddress : %p\n", baseAddress);
	}

	DbgPrint("[+] <= %s\n", __FUNCTION__);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT, _In_ PUNICODE_STRING) {

	PsSetLoadImageNotifyRoutine(PloadImageNotifyRoutine);
	PsSetCreateProcessNotifyRoutineEx(PcreateProcessNotifyRoutineEx, FALSE);
	PsSetCreateThreadNotifyRoutine(PcreateThreadNotifyRoutine);

	return STATUS_SUCCESS;
}
