#include "Question 1.h"

EXTERN_C
auto DriverUnload(_In_ PDRIVER_OBJECT DriverObject) -> void {
	if (DriverObject->DeviceObject != nullptr)
		IoDeleteDevice(DriverObject->DeviceObject);

	return;
}

EXTERN_C
auto DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING) -> NTSTATUS {

	DriverObject->DriverUnload = DriverUnload;

	auto ns = KernelModeApc();
	if (!NT_SUCCESS(ns))	return STATUS_FAILED_DRIVER_ENTRY;

	auto pid = reinterpret_cast<HANDLE>(3908);
	ns = UserModeApc(pid);
	if (!NT_SUCCESS(ns))	return STATUS_FAILED_DRIVER_ENTRY;

	return STATUS_SUCCESS;
}