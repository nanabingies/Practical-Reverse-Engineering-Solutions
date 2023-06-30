
#include "Header.h"

/// <summary>
/// Ques
/// Explain how we were able to determine that ExpWorkerThread is the system
/// thread responsible for dequeueing work items and executing them.
/// </summary>
/// <returns></returns>

auto WorkerThread(_In_ PDEVICE_OBJECT DeviceObject) -> NTSTATUS {

	NT_ASSERT(DeviceObject != nullptr);

	auto WorkItem = IoAllocateWorkItem(DeviceObject);
	if (WorkItem == nullptr)	return STATUS_INSUFFICIENT_RESOURCES;

	DbgPrint("WorkItem allocated at %p\n", static_cast<PVOID>(WorkItem));
	DbgPrint("Queueing work item ... \n");

	IoQueueWorkItem(WorkItem, static_cast<PIO_WORKITEM_ROUTINE>(WorkerRoutine),
		DelayedWorkQueue, (PVOID)0x1);

	IoFreeWorkItem(WorkItem);

	return STATUS_SUCCESS;
}


/// <summary>
/// Worker Routine Callback. 
/// We will determine if ExpWorkerThread really dequeues the queue and executes it
/// </summary>
/// <param name="DeviceObject"></param>
/// <param name="Context"></param>
/// <returns></returns>

auto WorkerRoutine(_In_ PDEVICE_OBJECT DeviceObject, _In_opt_ PVOID Context) -> void {

	DbgPrint("In WorkerRoutine ...\n");
	DbgPrint("DeviceObject : %p | Context : %p\n",
		static_cast<PVOID>(DeviceObject), Context);

	return;
}

/*
* 1: kd > kb
* # RetAddr           : Args to Child : Call Site
* 00 fffff804`52274ddf : ffff9c85`6938c300 ffff9c85`6938c300 ffff9c85`68bfde00 fffff804`555f5020 : WorkItems + 0x10ce
* 01 fffff804`522ca645 : ffff9c85`6547a450 ffff9c85`67840040 fffff804`52274ce0 de2ebc92`0000000c : nt!IopProcessWorkItem + 0xff
* 02 fffff804`52337715 : ffff9c85`67840040 00000000`00000080 ffff9c85`65487380 0000246f`bd9bbdff : nt!ExpWorkerThread + 0x105
* 03 fffff804`523d56ea : fffff804`5211d180 ffff9c85`67840040 fffff804`523376c0 7ad06001`0000003e : nt!PspSystemThreadStartup + 0x55
* 04 00000000`00000000 : fffff60b`bd631000 fffff60b`bd62b000 00000000`00000000 00000000`00000000 : nt!KiStartSystemThread + 0x2a
*
* The call stack after executing the Worker Routine even shows that ExpWorkerThread was executed
* before calling IopProcessWorkItem to process the work item
* 
*/