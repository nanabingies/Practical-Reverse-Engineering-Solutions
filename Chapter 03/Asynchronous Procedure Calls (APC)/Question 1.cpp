#include "Question 1.h"

EXTERN_C
auto DriverUnload(_In_ PDRIVER_OBJECT DriverObject) -> void {
	if (DriverObject->DeviceObject != nullptr)
		IoDeleteDevice(DriverObject->DeviceObject);

	return;
}

EXTERN_C
auto DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING) -> NTSTATUS {

	DriverObject->DriverUnload = DriverUnload;

	auto ns = KernelModeApc();
	if (!NT_SUCCESS(ns))	return STATUS_FAILED_DRIVER_ENTRY;

	auto pid = reinterpret_cast<HANDLE>(3908);
	ns = UserModeApc(pid);
	if (!NT_SUCCESS(ns))	return STATUS_FAILED_DRIVER_ENTRY;

	return STATUS_SUCCESS;
}



/// <summary>
/// Ques:
/// Write a driver using Kernel Mode Apc
/// Ques 7:
/// View the stack when they are executed
/// </summary>
/// <returns></returns>
/// 
/// stack :
/// 1: kd> kb
/// # RetAddr           : Args to Child : Call Site
/// 00 (Inline Function) : --------`------- - --------`------- - --------`------- - --------`------- - : AsynchronousProcedureCalls!KernelModeApc::__l2::<lambda_0f50872384f8936f0e86f0e2e5f588aa>::operator() + 0x42[Asynchronous Procedure Calls\Question 1.cpp @ 24]
/// 01 fffff804`10c5a22d : ffffe20a`b5781040 00000000`00000070 00000000`00000000 ffff8d07`eba767d0 : AsynchronousProcedureCalls!<lambda_0f50872384f8936f0e86f0e2e5f588aa>::<lambda_invoker_cdecl>+0x52[Asynchronous Procedure Calls\Question 1.cpp @ 25]
/// 02 fffff804`10c64bf2 : fffff804`15af3200 00000000`00000000 00000000`00000000 00000000`00000000 : nt!KiDeliverApc + 0x19d
/// 03 fffff804`10cc1b0d : fffff804`109ca180 00000000`00000000 fffff804`109ca101 00000000`00000000 : nt!KiExitDispatcher + 0x282
/// 04 fffff804`15af111a : 00000000`00000000 ffffe20a`b1502050 ffffe20a`b6a41000 fffff804`00000000 : nt!KeInsertQueueApc + 0x14d
/// 05 fffff804`15af1014 : 00000000`00000010 fffff804`15af3298 ffffe20a`b5dcee50 ffffe20a`b6a41000 : AsynchronousProcedureCalls!KernelModeApc + 0x6a[Asynchronous Procedure Calls\Question 1.cpp @ 29]
/// 06 fffff804`15af128b : 00000000`00000000 00000000`00000000 ffffe20a`b6a41000 ffffe20a`b5dcee50 : AsynchronousProcedureCalls!DriverEntry + 0x14[Asynchronous Procedure Calls\main.cpp @ 17]
/// 07 fffff804`15af11c0 : ffffe20a`b6a41000 ffff8d07`eba76a50 ffffe20a`b5dcee50 ffffe20a`b5dcee50 : AsynchronousProcedureCalls!FxDriverEntryWorker + 0xbf[minkernel\wdf\framework\kmdf\src\dynamic\stub\stub.cpp @ 349]
/// 08 fffff804`11329cb6 : 00000000`00000000 00000000`00000000 ffffe20a`b5dcee50 ffffffff`80001f50 : AsynchronousProcedureCalls!FxDriverEntry + 0x20[minkernel\wdf\framework\kmdf\src\dynamic\stub\stub.cpp @ 238]
/// 09 fffff804`113296ee : 00000000`00000000 00000000`00000000 00000000`00000500 fffff804`110611b0 : nt!IopLoadDriver + 0x4c2
/// 0a fffff804`10cd9645 : ffffe20a`00000000 ffffffff`80001f50 ffffe20a`b5781040 ffffe20a`b1a62cc0 : nt!IopLoadUnloadDriver + 0x4e
/// 0b fffff804`10d46715 : ffffe20a`b5781040 00000000`00000080 ffffe20a`b1a87380 0000246f`bd9bbdff : nt!ExpWorkerThread + 0x105
/// 0c fffff804`10de46ea : fffff804`109ca180 ffffe20a`b5781040 fffff804`10d466c0 00000000`00000000 : nt!PspSystemThreadStartup + 0x55
/// 0d 00000000`00000000 : ffff8d07`eba77000 ffff8d07`eba71000 00000000`00000000 00000000`00000000 : nt!KiStartSystemThread + 0x2a

auto KernelModeApc() -> NTSTATUS {
	auto apc = reinterpret_cast<KAPC*>(ExAllocatePoolWithTag(NonPagedPool, sizeof KAPC, KERNEL_TAG));
	if (apc == nullptr)		return STATUS_INSUFFICIENT_RESOURCES;

	KeInitializeApc(apc, KeGetCurrentThread(), OriginalApcEnvironment,
		[](_In_ PKAPC Apc, _Inout_ PKNORMAL_ROUTINE* NormalRoutine, _Inout_ PVOID* NormalContext,
			_Inout_ PVOID*, _Inout_ PVOID*) {
				DbgPrint("[%s] Apc : %p\n", __FUNCTION__, static_cast<PVOID>(Apc));
				DbgPrint("[%s] NormalRoutine : %p\n", __FUNCTION__, static_cast<PVOID>(NormalRoutine));
				DbgPrint("[%s] NormalContext : %p\n", __FUNCTION__, static_cast<PVOID>(NormalContext));
		}, nullptr, nullptr, KernelMode, nullptr);

	KeInsertQueueApc(apc, nullptr, nullptr, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

auto UserModeApc(HANDLE pid) -> NTSTATUS {
	auto apc = reinterpret_cast<KAPC*>(ExAllocatePoolWithTag(NonPagedPool, sizeof KAPC, USER_TAG));
	if (apc == nullptr)		return STATUS_INSUFFICIENT_RESOURCES;

	OBJECT_ATTRIBUTES oa{};
	InitializeObjectAttributes(&oa, nullptr, OBJ_CASE_INSENSITIVE, nullptr, nullptr);

	CLIENT_ID client_id{};
	client_id.UniqueProcess = pid;
	
	HANDLE process_handle{};
	auto ns = ZwOpenProcess(static_cast<PHANDLE>(&process_handle), FILE_READ_ACCESS | FILE_WRITE_ACCESS,
		static_cast<POBJECT_ATTRIBUTES>(&oa), &client_id);

	if (!NT_SUCCESS(ns)) {
		ZwClose(process_handle);
		ExFreePoolWithTag(apc, USER_TAG);
		return STATUS_UNSUCCESSFUL;
	}

	ULONG size = 0;
	ZwQuerySystemInformation(SystemProcessInformation, nullptr, 0, &size);
	
	auto buf = ExAllocatePoolWithTag(NonPagedPool, size, USER_TAG);
	if (buf == nullptr) {
		ZwClose(process_handle);
		ExFreePoolWithTag(apc, USER_TAG);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	ZwQuerySystemInformation(SystemProcessInformation, buf, size, &size);

	auto ptr = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>(buf);

	do {
		if (ptr->UniqueProcessId == pid) {
			auto num_threads = ptr->NumberOfThreads;

			for (ULONG idx = 0; idx < num_threads; idx++) {
				auto thread = ptr->Threads[idx];

				if (thread.State == StateWait) {
					PETHREAD curr_thread{};

					ns = PsLookupThreadByThreadId(thread.ClientId.UniqueThread, &curr_thread);
					if (NT_SUCCESS(ns)) {
						DbgPrint("Got thread %p ethread %p\n", thread.ClientId.UniqueThread, static_cast<PVOID>(curr_thread));

						KAPC_STATE kapc_state{};
						PEPROCESS process{};
						PsLookupProcessByProcessId(pid, &process);
						KeStackAttachProcess(process, &kapc_state);

						PVOID thread_base = nullptr;
						SIZE_T region_size = 0x10;
						ZwAllocateVirtualMemory(process_handle, &thread_base, 0, &region_size,
							MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);
							
						RtlCopyMemory(thread_base, "\xCC\xCC\xCC\xCC", 0x4);

						KeInitializeApc(apc, curr_thread, OriginalApcEnvironment,
							[](_In_ PKAPC Apc, _Inout_ PKNORMAL_ROUTINE*, _Inout_ PVOID*,
								_Inout_ PVOID*, _Inout_ PVOID*) {
										
									ExFreePoolWithTag(Apc, USER_TAG);
									return;
							},
							nullptr,
							reinterpret_cast<PKNORMAL_ROUTINE>(thread_base),
									UserMode, nullptr);
						KeInsertQueueApc(apc, nullptr, nullptr, IO_NO_INCREMENT);

						KeUnstackDetachProcess(&kapc_state);
						ObDereferenceObject(process);

							
						ObDereferenceObject(curr_thread);
						break;
					}
				}
				
			}

			break;
		}

		ptr = reinterpret_cast<SYSTEM_PROCESS_INFORMATION*>
			((UCHAR*)ptr + ptr->NextEntryOffset);
	} while (ptr != nullptr);

	ZwClose(process_handle);
	ExFreePoolWithTag(apc, USER_TAG);

	return STATUS_SUCCESS;
}