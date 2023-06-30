
#include "Header.h"

EXTERN_C
auto DriverUnload(_In_ PDRIVER_OBJECT DriverObject) -> void {
	if (DriverObject->DeviceObject != nullptr)
		IoDeleteDevice(DriverObject->DeviceObject);

	return;
}

EXTERN_C
auto DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING) -> NTSTATUS {

	UNICODE_STRING usDevName;
	RtlInitUnicodeString(&usDevName, L"\\Device\\workitem");

	PDEVICE_OBJECT DeviceObject{};
	auto ns = IoCreateDevice(DriverObject, 0, &usDevName, FILE_DEVICE_UNKNOWN,
		FILE_DEVICE_SECURE_OPEN, false, static_cast<PDEVICE_OBJECT*> (&DeviceObject));

	DriverObject->DriverUnload = DriverUnload;

	EnumerateWorkItems(DeviceObject);

	return ns;
}