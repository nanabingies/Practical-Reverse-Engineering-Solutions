#pragma once

#include "Header.hpp" 
#pragma warning(disable : 4100)
#pragma warning(disable : 6387)

auto GetProcessList(_Inout_ PVOID* Buffer) -> NTSTATUS {

	*Buffer = 0;

	ULONG returnLength = 0x0;
	auto ntStatus = ZwQuerySystemInformation(0x5, NULL, 0, &returnLength);
	if (returnLength == 0) {
		DbgPrint(__FUNCTION__"[ZwQuerySystemInformation1] failed with error code %X\n", ntStatus);
		return ntStatus;
	}

	*Buffer = ExAllocatePoolWithTag(NonPagedPool, returnLength * sizeof PVOID, APC_POOL_TAG);
	if (!*Buffer) {
		DbgPrint(__FUNCTION__" [ExAllocatePoolWithTag] failed\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	ntStatus = ZwQuerySystemInformation(0x5, *Buffer, returnLength, &returnLength);
	if (!NT_SUCCESS(ntStatus)) {
		DbgPrint(__FUNCTION__" [ZwQuerySystemInformation2] failed with error code %X\n", ntStatus);
		return ntStatus;
	}

	return STATUS_SUCCESS;
}

VOID NormalRoutine(_In_ PVOID NormalContext, _In_ PVOID SystemArgument1, _In_ PVOID) {
	//DbgPrint("[+] In NORMAL ROUTINE!!!!\n");
	//DbgPrint("[+] NormalContext: %p\n", NormalContext);
	//DbgPrint("[+] Executing number : %ld\n", reinterpret_cast<SIZE_T>(SystemArgument1));
}

VOID KernelRoutine(_In_ PKAPC Apc, _Inout_ PKNORMAL_ROUTINE* NormalRoutine, _Inout_ PVOID* NormalContext, _Inout_ PVOID*, _Inout_ PVOID*) {
	//DbgPrint("[+] In KERNEL ROUTINE!!!!\n");
	//DbgPrint("[+] Apc: %p\n", static_cast<VOID*>(Apc));
	//DbgPrint("[+] NormalRoutine: %p\n", static_cast<VOID*>(NormalRoutine));
	//DbgPrint("[+] NormalContext: %p\n", static_cast<VOID*>(NormalContext));
	ExFreePoolWithTag(Apc, APC_POOL_TAG);
}

NTSTATUS InsertKernelApcs(PKTHREAD thread, PVOID number) {

	auto Apc = reinterpret_cast<_KAPC*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(_KAPC), APC_POOL_TAG));
	if (!Apc) {
		DbgPrint("[ExAllocatePoolWithTag] failed\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	KeInitializeApc(Apc, thread, OriginalApcEnvironment, static_cast<PKKERNEL_ROUTINE>(&KernelRoutine), __nullptr,
		static_cast<PKNORMAL_ROUTINE>(NormalRoutine), KernelMode, number);

	if (!KeInsertQueueApc(Apc, number, __nullptr, IO_NO_INCREMENT)) {
		DbgPrint("[KeInsertQueueApc] failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	DbgPrint("[%s] APC %p [%lld] inserted\n", __FUNCTION__, static_cast<PVOID>(Apc), reinterpret_cast<SIZE_T>(number));
	return STATUS_SUCCESS;
}

NTSTATUS InsertUserApcs(PKTHREAD thread, PVOID number) {
	auto Apc = reinterpret_cast<_KAPC*>(ExAllocatePoolWithTag(NonPagedPool, sizeof(_KAPC), APC_POOL_TAG));
	if (!Apc) {
		DbgPrint("[ExAllocatePoolWithTag] failed\n");
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	KeInitializeApc(Apc, thread, OriginalApcEnvironment, static_cast<PKKERNEL_ROUTINE>(&KernelRoutine), __nullptr,
		static_cast<PKNORMAL_ROUTINE>(NormalRoutine), UserMode, number);

	if (!KeInsertQueueApc(Apc, number, __nullptr, IO_NO_INCREMENT)) {
		DbgPrint("[KeInsertQueueApc] failed\n");
		return STATUS_UNSUCCESSFUL;
	}

	DbgPrint("[%s] APC %p [%lld] inserted\n", __FUNCTION__, static_cast<PVOID>(Apc), reinterpret_cast<SIZE_T>(number));
	return STATUS_SUCCESS;
}


auto EnumerateAPC() -> NTSTATUS {
	auto irql = KeRaiseIrqlToDpcLevel();
	auto SystemThread = KeGetCurrentThread();
	//auto SystemProcess = PsGetCurrentProcess();

	for (auto idx = 0; idx < 100; idx++) {
		if (!NT_SUCCESS(InsertKernelApcs(SystemThread, (PVOID)idx)))
			break;

		if (!NT_SUCCESS(InsertUserApcs(SystemThread, (PVOID)idx)))
			break;
	}

	auto ApcState = reinterpret_cast<KAPC_STATE*>(&SystemThread->ApcState);
	DbgPrint("[>] SystemThread : 0x%p\n", (VOID*)SystemThread);
	DbgPrint("[>] ApcState : 0x%p\n", (VOID*)ApcState);

	auto count = 0;

	// Kernel APC
	DbgPrint("[>] Dumping Kernel APCs\n");
	for (auto* entry = ApcState->ApcListHead[KernelMode].Flink; entry != &ApcState->ApcListHead[KernelMode];
		entry = entry->Flink) {

		auto CurrApc = reinterpret_cast<KAPC*>(CONTAINING_RECORD(entry, KAPC, ApcListEntry));
		DbgPrint("\t[%d] Apc : 0x%p ; KernelRoutine : 0x%p ; RundownRoutine : 0x%p ; NormalRoutine : 0x%p\n", count,
			(VOID*)CurrApc, CurrApc->Reserved[0], CurrApc->Reserved[1], CurrApc->Reserved[2]);

		count++;
	}

	DbgPrint("[>] ======================================================================================\n");
	count = 0;

	// User APC
	DbgPrint("[>] Dumping User APCs\n");
	for (auto* entry = ApcState->ApcListHead[UserMode].Flink; entry != &ApcState->ApcListHead[UserMode];
		entry = entry->Flink) {

		auto CurrApc = reinterpret_cast<KAPC*>(CONTAINING_RECORD(entry, KAPC, ApcListEntry));
		DbgPrint("\t[%d] Apc : 0x%p ; KernelRoutine : 0x%p ; RundownRoutine : 0x%p ; NormalRoutine : 0x%p\n", count,
			(VOID*)CurrApc, CurrApc->Reserved[0], CurrApc->Reserved[1], CurrApc->Reserved[2]);

		count++;
	}

	KeLowerIrql(irql);

	return STATUS_SUCCESS;
}