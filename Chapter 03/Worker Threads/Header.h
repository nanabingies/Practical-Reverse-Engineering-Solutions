#pragma once
#include "stdafx.h"

struct _IO_WORKITEM {
    struct _WORK_QUEUE_ITEM WorkItem;                                       //0x0
    VOID(*Routine)(VOID* arg1, VOID* arg2, struct _IO_WORKITEM* arg3);     //0x20
    VOID* IoObject;                                                         //0x28
    VOID* Context;                                                          //0x30
    struct _ETHREAD* WorkOnBehalfThread;                                    //0x38
    ULONG Type;                                                             //0x40
    struct _GUID ActivityId;                                                //0x44
};

auto WorkerThread(_In_ PDEVICE_OBJECT)->NTSTATUS;

auto WorkerRoutine(_In_ PDEVICE_OBJECT DeviceObject,
	_In_opt_ PVOID Context)->void;

auto EnumerateWorkItems(_In_ PDEVICE_OBJECT)->NTSTATUS;