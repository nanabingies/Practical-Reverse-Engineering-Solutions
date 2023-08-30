#pragma once
#include <ntifs.h>
#include <ntddk.h>
#include <intrin.h>

typedef unsigned char BYTE;
typedef short WORD;

struct _v2 {
	INT		Index;			// 0x00
	ULONG	Param1;			// 0x04
	ULONG	Param2;			// 0x08
	PVOID	Param3;			// 0x0C
	HANDLE	ProcessHandle;	// 0x10
	ULONG	DesiredAccess;	// 0x14
	HANDLE	ProcessId;		// 0x18
	PVOID	FuncAddr;		// 0x1C
};

typedef struct _SYSTEM_SERVICE_TABLE {
	PULONG	ServiceTable;
	PULONG	fidle2;
	ULONG	NumEntries;
	ULONG	ArgumentTable;
}SYSTEM_SERVICE_TABLE, * PSYSTEM_SERVICE_TABLE;

__declspec(dllimport) PSYSTEM_SERVICE_TABLE KeServiceDescriptorTable;