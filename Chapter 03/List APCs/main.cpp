
#include "Process.hpp"
#pragma warning(disable : 4100)

UNICODE_STRING g_DrvName = RTL_CONSTANT_STRING(L"\\Device\\APC");
PDEVICE_OBJECT g_DevObj{};

EXTERN_C DRIVER_UNLOAD DriverUnload;
EXTERN_C DRIVER_DISPATCH DefaultDispatch;


EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

	auto ntStatus = IoCreateDevice(DriverObject, 0, &g_DrvName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE, (PDEVICE_OBJECT*)&g_DevObj);
	if (!NT_SUCCESS(ntStatus)) {
		DbgPrint(__FUNCTION__" [IoCreateDevice] failed with error code %X\n", ntStatus);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrint(__FUNCTION__" Created Device Object.\n");

	DriverObject->DriverUnload = DriverUnload;
	for (int i = 0; i < IRP_MJ_MAXIMUM_FUNCTION; i++) {
		DriverObject->MajorFunction[i] = DefaultDispatch;
	}

	EnumerateAPC();

	return STATUS_SUCCESS;
}

EXTERN_C VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject){
	IoDeleteDevice(DriverObject->DeviceObject);
}

EXTERN_C NTSTATUS DefaultDispatch(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;

	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}