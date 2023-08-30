#pragma once
#include "Header.h"

PVOID dword_10003080;
ULONG dword_10003020;
ULONG dword_10003040[0x10];

VOID DisableWP() {
	ULONG cr0 = __readcr0();
	cr0 &= 0xFFFEFFFF;
	__writecr0(cr0);
}

VOID EnableWP() {
	ULONG cr0 = __readcr0();
	cr0 |= 0x10000;
	__writecr0(cr0);
}

VOID sub_10001277(PVOID a1, PVOID a2, ULONG a3) {

}