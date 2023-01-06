#pragma once

#include <ntifs.h>
#include <ntddk.h>

#define POOL_TAG 'ssss'
using BYTE = unsigned char;

struct _qword_40A940 {
	PVOID a1; // 0x00
	PVOID a2; // 0x08
	PVOID a3; // 0x10
	PVOID a4; // 0x18
};

struct __pool_struct_40AA58 {
	LIST_ENTRY ListHead;	// 0x00
	PVOID param2;			// 0x10
};

struct _r13_struct {
	LIST_ENTRY ListHead;	// 0x00
	PVOID param2;			// 0x10
};

PDEVICE_OBJECT g_lowerDeviceObject{};
ULONG dword_40AA24, dword_40AECC, dword_40AEFC, dword_40AF00, dword_40AEF8, dword_40AA48, dword_40AA1C;
ULONG dword_40A9A4, dword_40A9A0;
ULONG64 qword_40AED8, qword_40AEE0;
ULONG64 qword_40AEF0;
PVOID64 Dispatch_Irp_Mj_Internal_Dev_Ctl, Dispatch_Irp_Mj_Dev_Ctl;
ERESOURCE _eresource1{}, _eresource2{};
LIST_ENTRY qword_40A590;

EXTERN_C PDRIVER_DISPATCH sub_402334;
EXTERN_C void PcreateProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create);
EXTERN_C void PloadImageNotifyRoutine(_In_opt_ PUNICODE_STRING FullImageName, _In_ HANDLE ProcessId, _In_ PIMAGE_INFO ImageInfo);
EXTERN_C PVOID sub_404298();
EXTERN_C PVOID sub_4052AC();

auto sub_4031D8(PDRIVER_OBJECT, PVOID, PVOID, PVOID) -> BOOLEAN;
auto sub_401360(PVOID, PDRIVER_OBJECT) -> NTSTATUS;
auto sub_401714(ULONG, PVOID, PVOID, PVOID) -> BOOLEAN;
auto sub_403384(PDRIVER_OBJECT, PVOID, PVOID, ULONG, PVOID, ULONG) -> PVOID;
auto sub_4015B4(ULONG, PVOID, PVOID) -> NTSTATUS;
auto sub_404718(ULONG64, PVOID, PVOID, PVOID) -> PVOID;