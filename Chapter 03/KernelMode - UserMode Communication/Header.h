#pragma once

#include <ntifs.h>
#include <ntddk.h>
#include <windef.h>

UNICODE_STRING usDriverName = RTL_CONSTANT_STRING(L"\\Device\\SharedMemory");
UNICODE_STRING usDosDeviceName = RTL_CONSTANT_STRING(L"\\DosDevices\\SharedMemory");
UNICODE_STRING usMemoryName = RTL_CONSTANT_STRING(L"\\BaseNamedObjects\\SharedMemoryMapping");

#define CREATE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define READ_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define WRITE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define CLOSE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x904, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

struct _shared_req {
	void* _data;
	SIZE_T _dataLen;
	HANDLE _pid;
	//void* _address;
	//SIZE_T _addressSize;
};


NTSTATUS ReadSharedMemory(_In_ struct _shared_req*);
NTSTATUS WriteSharedMemory(_In_ struct _shared_req*);


EXTERN_C NTSTATUS DefaultDispatch(_In_ PDEVICE_OBJECT, _In_ PIRP);
EXTERN_C NTSTATUS IoctlDipatch(_In_ PDEVICE_OBJECT, _In_ PIRP);
EXTERN_C VOID DriverUnload(_In_ PDRIVER_OBJECT);

HANDLE g_SectionHandle = NULL;
PVOID g_BaseAddress = NULL;
SIZE_T g_SectionSize = 1024000;
PDEVICE_OBJECT g_DeviceObject;
SECURITY_DESCRIPTOR SecDescriptor;
ULONG DaclLength;
PACL Dacl;

EXTERN_C
NTKERNELAPI
NTSTATUS NTAPI MmCopyVirtualMemory
(
	PEPROCESS SourceProcess,
	PVOID SourceAddress,
	PEPROCESS TargetProcess,
	PVOID TargetAddress,
	SIZE_T BufferSize,
	KPROCESSOR_MODE PreviousMode,
	PSIZE_T ReturnSize
);