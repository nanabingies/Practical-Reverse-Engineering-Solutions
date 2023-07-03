
#include "Header.h"

/// <summary>
/// Ques:
/// Write a driver to enumerate all work items on the system and
/// explain the problems you had to overcome in the process.
/// </summary>
/// <returns></returns>

auto EnumerateWorkItems(_In_ PDEVICE_OBJECT DeviceObject) -> NTSTATUS {

	DbgPrint("[+] Lets allocate 50 work items ...\n");

	auto* items = reinterpret_cast<PIO_WORKITEM*>
		(ExAllocatePool(NonPagedPool, sizeof PIO_WORKITEM * 50));
	if (items == nullptr)	return STATUS_INSUFFICIENT_RESOURCES;

	for (auto idx = 0; idx < 50; idx++) {
		items[idx] = IoAllocateWorkItem(DeviceObject);
		IoQueueWorkItem(items[idx], [](_In_ PDEVICE_OBJECT DeviceObject, _In_opt_ PVOID Context) {
				DbgPrint("DeviceObject : %p | Context : %p\n", DeviceObject, Context);
			}, 
			DelayedWorkQueue, (PVOID)items[idx]);

		// Lets pause on the deletion
	}

	// Now let's enumerate work items
	auto workitem = IoAllocateWorkItem(DeviceObject);
	NT_ASSERT(workitem != nullptr);
	DbgPrint("workitem : %p\n", static_cast<PVOID>(workitem));
	
	IoQueueWorkItem(workitem, [](_In_ PDEVICE_OBJECT, _In_opt_ PVOID) {
			DbgPrint("Test WorkItem\n");
		}, DelayedWorkQueue, nullptr);

	for (auto entry = workitem->WorkItem.List.Blink; (entry != &workitem->WorkItem.List) && (entry != nullptr);
		entry = entry->Blink) {

		auto item = CONTAINING_RECORD(entry, _IO_WORKITEM, WorkItem);
		DbgPrint("\t[Listing Work Item]: WorkItem %p\n", static_cast<PVOID>(item));

	}

	return STATUS_SUCCESS;
}
