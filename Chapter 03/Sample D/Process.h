#pragma once
#include "Header.h"
#include "Utils.h"


void PcreateProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create) {
	CHAR var_28, var_27, var_26, var_25;
	ULONG var_24 = 0, idx;
	ULONG var_18, var_30;
	SIZE_T RegionSize;
	PEPROCESS Eprocess;
	HANDLE ProcessHandle;
	PVOID BaseAddress = 0x71800000;
	NTSTATUS status;

	var_28 = var_27 = var_26 = 0x50;
	var_25 = 0xE9;
	if (dword_10003080 == NULL)	return;

	status = PsLookupProcessByProcessId(ProcessId, (PEPROCESS*) & Eprocess);
	if (!NT_SUCCESS(status))	return;
	
	if (!Create) {
		for (idx = 0; idx < 0x40; idx += 4) {
			if (ProcessId == dword_10003040[idx]) {
				dword_10003040[idx] = 0;
			}
		}
		
		return;
	}

	for (idx = 0; idx < 0x40; idx += 4) {
		if (ParentId == dword_10003040[idx])	return;
	}

	status = ObOpenObjectByPointer((PVOID)Eprocess, 0, NULL, 0x1F0FFF, NULL, KernelMode, &ProcessHandle);
	if (!NT_SUCCESS(status))	return;
	RegionSize = *(SIZE_T*)((PUCHAR)dword_10003080 + 0x38);
	status = ZwAllocateVirtualMemory(ProcessHandle, (PVOID*)&BaseAddress, 0, (SIZE_T*)&RegionSize,
		MEM_RESERVE, PAGE_EXECUTE_READWRITE);
	if (!NT_SUCCESS(status))	goto loc_1000125F;

	ZwAllocateVirtualMemory(ProcessHandle, (PVOID*)&BaseAddress, 0, (SIZE_T*)&RegionSize,
		MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	__asm {
		lea     edx, var_18
		push    edx
		mov     eax, dword_10003080
		mov     ecx, [eax + 0x38]
		push    ecx
		mov     edx, dword_10003080
		push    edx
		mov     eax, BaseAddress
		push    eax
		mov     ecx, ProcessHandle
		push    ecx
		mov     edx, dword_10003080
		mov     eax, [edx + 0x0C]
		lea     edx, [esp + 0x68]
		int     0x2E

		add     esp, 0x14
		mov     status, eax
	}

	__asm {
		push    edx
		push    8
		lea     edx, var_30
		push    edx
		mov     eax, dword_10003080
		mov     ecx, [eax + 0x14]
		push    ecx
		mov     edx, ProcessHandle
		push    edx
		mov     edx, dword_10003080
		mov     eax, [edx + 0x8]
		lea     edx, [esp + 0x68]
		int     2Eh

		add     esp, 0x14
		mov		ntstatus, eax
	}
		
	__asm {
		mov     eax, dword_10003080
		mov     ecx, [eax + 0x10]
		sub     ecx, [eax + 0x14]
		mov     eax, BaseAddress
		lea     eax, [ecx + eax - 0x8]
		mov		var_24, eax
		lea     ecx, var_18
		push    ecx
		push    8
		lea     edx, var_28
		push    edx
		mov     eax, dword_10003080
		mov     ecx, [eax + 0x14]
		push    ecx
		mov     edx, ProcessHandle
		push    edx
		mov     ecx, dword_10003080
		mov     eax, [ecx + 0x0C]
		lea     edx, [esp + 0x68]
		int     0x2E

		add     esp, 0x14
	}


loc_1000125F:
	ZwClose(ProcessHandle);
	ObDereferenceObject(Eprocess);

	return;
}