#pragma once
#include "Header.h"

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