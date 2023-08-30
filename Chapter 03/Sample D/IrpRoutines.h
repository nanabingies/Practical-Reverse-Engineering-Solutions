#pragma once
#include "Header.h"
#include "Utils.h"

NTSTATUS IrpCreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

NTSTATUS IrpDeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION ioStackLocation;
	NTSTATUS status = STATUS_INFO_LENGTH_MISMATCH;
	ULONG IoControlCode, InputBufferLength, OutputBufferLength;
	ULONG var_4, var_8, var_C, var_10, var_24;
	PVOID SystemBuffer;
	OBJECT_ATTRIBUTES ObjectAttributes;
	CLIENT_ID ClientId;
	BOOLEAN Ok;

	SystemBuffer = Irp->AssociatedIrp.SystemBuffer;		// Buffered I/O
	ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
	IoControlCode = ioStackLocation->Parameters.DeviceIoControl.IoControlCode;
	InputBufferLength = ioStackLocation->Parameters.DeviceIoControl.InputBufferLength;
	OutputBufferLength = ioStackLocation->Parameters.DeviceIoControl.OutputBufferLength;

	switch (IoControlCode) {
	case 0x22E00C: {
		if (InputBufferLength < 0x20)	break;

		InitializeObjectAttributes(&ObjectAttributes, NULL, 0, NULL, NULL);
		struct _v2* v2 = (struct _v2*)SystemBuffer;
		ClientId.UniqueProcess = v2->ProcessId;
		ClientId.UniqueThread = 0;

		Ok = MmIsAddressValid(v2->FuncAddr);
		if (!Ok)	break;
		var_4 = 0x20;

		PULONG ServiceTable = KeServiceDescriptorTable->ServiceTable;
		ULONG Idx = v2->Index;
		PVOID ServiceFuncAddress = (PVOID)ServiceTable[Idx + 1];
		Ok = MmIsAddressValid(ServiceFuncAddress);
		if (!Ok)	break;

		DisableWP();
		*(PVOID*)ServiceFuncAddress = v2->FuncAddr;
		sub_10001277(&var_24, &v2->FuncAddr, 0xA);
		sub_10001277(&v2->FuncAddr, &v2->Param1, 0xA);

		status = ZwOpenProcess(&v2->ProcessHandle, v2->DesiredAccess, &ObjectAttributes, &ClientId);
		sub_10001277(&v2->FuncAddr, &var_24, 0xA);

		*(PULONG*)ServiceTable[Idx + 1] = *(PULONG)ServiceFuncAddress;
		EnableWP();
	}
				 break;

	case 0x22E010: {
		if (InputBufferLength < 0xC)	break;
		struct _v2* v2 = (struct _v2*)SystemBuffer;
		Ok = MmIsAddressValid(v2->FuncAddr);
		if (!Ok)	break;

		ULONG Idx = v2->Index;
		if (Idx > KeServiceDescriptorTable->NumEntries)		break;
		if (v2->Param1 == v2->Param2)		break;

		PULONG addr = KeServiceDescriptorTable->ServiceTable[Idx];
		v2->Param2 = *(PULONG)addr;
		DisableWP();
		*(PULONG*)KeServiceDescriptorTable->ServiceTable[Idx] = v2->Param1;
		EnableWP();
		var_4 = 0xC;
		var_8 = 0;
	}
				 break;

	case 0x22E014: {
		if (dword_10003080 != 0)	break;
		dword_10003080 = ExAllocatePoolWithTag(PagedPool, InputBufferLength + 0x28A0, 0);
		*(PVOID*)Irp->UserBuffer = 0;
		Irp->UserBuffer = dword_10003080;
		if (Irp->UserBuffer == NULL)	break;

		sub_10001277(&dword_10003080, Irp->AssociatedIrp.SystemBuffer, InputBufferLength);
		dword_10003020 = (ULONG)dword_10003080 + *(PULONG)dword_10003080;
		*(PVOID*)((PUCHAR)dword_10003080 + 0x38) = InputBufferLength;
		*(PVOID*)Irp->UserBuffer = 1;
		var_4 = 4;
		var_8 = 0;
	}
				 break;

	case 0x22E018: {
		if (dword_10003020 == NULL)	break;
		if (*(ULONG*)dword_10003020 > 0x14) {
			var_4 = 4;
			break;
		}
		var_10 = 0;
		if (*(ULONG*)dword_10003020 <= 0) {
			ULONG ops;
			ULONG ecx = *(ULONG*)dword_10003020;
		loc_100013BB:
			ops = ecx * 0x208;
			ops = dword_10003020 + ops + 4;
			sub_10001277(&ops, Irp->AssociatedIrp.SystemBuffer, InputBufferLength);
			dword_10003020 = ecx + 1;
			ops = ecx * 0x208;
			ops = (ULONG)dword_10003080 + ops + 4;
			*(PULONG*)((PUCHAR)dword_10003080 + 0x38) = ops;
			var_4 = 4;
			break;
		}

		var_C = *(ULONG*)(dword_10003020 + 4);
		var_10 = 0;

		do {
			var_4 = (ULONG)SystemBuffer;

			while (*(BYTE*)var_4 == *(BYTE*)var_C) {
				if (*(BYTE*)var_4 == NULL)	break;
				Irp->Size = *(WORD*)(var_4 + 2);
				if (*(WORD*)var_4 != *(WORD*)var_C)		break;

				var_4 += 4;
				var_C += 4;
			}

			var_10++;
			var_C += 0x208;
			if (var_10 > *(ULONG*)dword_10003020)	goto loc_100013BB;
		} while (1);
	}
				 break;

	case 0x22E01C: {
		if (OutputBufferLength != 0x4)	break;
		ULONG idx = 0;
		for (idx = 0; idx < 0x10; ++idx) {
			if (dword_10003040[idx] == 0)	break;
		}

		dword_10003040[idx] = SystemBuffer;
	}
				 break;

	default:
		break;
	}
}