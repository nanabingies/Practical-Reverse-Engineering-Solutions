#pragma once

#include "Header.hpp"

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

auto ListAPC() -> NTSTATUS {

	PVOID buffer = { 0 };
	auto ntStatus = GetProcessList(&buffer);
	if (!NT_SUCCESS(ntStatus)) {
		DbgPrint("[GetProcessList] failed with error code %X\n", ntStatus);
		return ntStatus;
	}

	PETHREAD thread{};
	auto irql = KeGetCurrentIrql();
	KeRaiseIrql(DISPATCH_LEVEL, &irql);
	auto processList = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buffer);
	do {
		if (processList->NumberOfThreads > 0) {
			ntStatus = PsLookupThreadByThreadId(processList->Threads->ClientId.UniqueThread, static_cast<PETHREAD*>(&thread));
			if (NT_SUCCESS(ntStatus)) {
				auto object = *reinterpret_cast<PVOID*>(reinterpret_cast<PUCHAR>(thread) + 0x98); // struct _KAPC_STATE
				PKAPC_STATE apc_state = static_cast<PKAPC_STATE>(object);

				// Enumerating kernel APCs
				for (LIST_ENTRY* entry = apc_state->ApcListHead[0].Flink;
					entry != &apc_state->ApcListHead[0]; entry = entry->Flink) {

					auto apc = reinterpret_cast<KAPC*>(entry);
					DbgPrint("**************   KernelMode   ***************\n");
					DbgPrint("[>] APC : %p\n", apc);
					DbgPrint("[>] NormalContext : %p\n", apc->NormalContext);
					DbgPrint("[>] SystemArgument1 : %p\n", apc->SystemArgument1);
					DbgPrint("[>] SystemArgument2 : %p\n", apc->SystemArgument2);
					DbgPrint("[>] ApcMode : %X\n", apc->ApcMode);

				}

				// Enumerating user APCs
				/*for (LIST_ENTRY* entry = apc_state->ApcListHead[1].Flink;
					entry != &apc_state->ApcListHead[1]; entry = entry->Flink) {
					auto apc = reinterpret_cast<KAPC*>(entry);
					DbgPrint("[>] **************   UserMode   ***************\n");
					DbgPrint("[>] UserMode NormalContext : %p\n", apc->NormalContext);
					DbgPrint("[>] SystemArgument1 : %p\n", apc->SystemArgument1);
					DbgPrint("[>] SystemArgument2 : %p\n", apc->SystemArgument2);
					DbgPrint("[>] ApcMode : %X\n", apc->ApcMode);
				}*/
			}
		}

		processList = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>
			(reinterpret_cast<PUCHAR>(processList) + processList->NextEntryOffset);
	} while (processList->NextEntryOffset != NULL);

	KeLowerIrql(irql);

	ExFreePoolWithTag(buffer, APC_POOL_TAG);

	return STATUS_SUCCESS;
}