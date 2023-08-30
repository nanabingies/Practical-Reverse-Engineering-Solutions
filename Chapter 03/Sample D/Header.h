#pragma once
#include <ntddk.h>

typedef unsigned char BYTE;

struct _v2 {
	BYTE	Param1[0x18];	// 0x00
	HANDLE	ProcessId;		// 0x18
	PVOID	Addr;			// 0x1C
};