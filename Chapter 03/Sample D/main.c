#include "IrpRoutines.h"
#include "Process.h"

NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	UNREFERENCED_PARAMETER(RegistryPath);

	UNICODE_STRING drvName, dosName;
	NTSTATUS status;
	PDEVICE_OBJECT DeviceObject;

	RtlInitUnicodeString(&drvName, L"\\Device\\ProcPanama");
	status = IoCreateDevice(DriverObject, 0x1C, &drvName, FILE_DEVICE_UNKNOWN, NULL, FALSE, (PDEVICE_OBJECT*)&DeviceObject);
	if (NT_SUCCESS(status)) {
		RtlInitUnicodeString(&dosName, L"\\DosDevices\\ProcPanama");

		status = IoCreateSymbolicLink(&dosName, &drvName);
		if (!NT_SUCCESS(status))
			IoDeleteDevice(DeviceObject);
	}

	DriverObject->MajorFunction[IRP_MJ_CREATE] = &IrpCreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = &IrpCreateClose;
    DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = &IrpDeviceControl;

	PsSetCreateProcessNotifyRoutine(PcreateProcessNotifyRoutine, FALSE);

	return STATUS_SUCCESS;
}