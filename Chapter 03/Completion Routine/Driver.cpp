
#include <ntddk.h>
#include <hidport.h>
#pragma warning(disable : 6387)
#pragma warning(disable : 4100) 
#pragma warning(disable : 4189)


UNICODE_STRING drvName = RTL_CONSTANT_STRING(L"\\Device\\IoComp");
UNICODE_STRING dosName = RTL_CONSTANT_STRING(L"\\DosDevices\\IoComp");
_DEVICE_OBJECT* g_DevObj;
_DEVICE_OBJECT* g_ExtDevObj;

EXTERN_C NTSTATUS DefaultDispatch(_In_ _DEVICE_OBJECT*, _In_ _IRP*);
EXTERN_C VOID DriverUnload(_In_ _DRIVER_OBJECT*);
EXTERN_C NTSTATUS IoCompletionRoutine(_In_ DEVICE_OBJECT*, _In_ _IRP*, _In_opt_ PVOID);

EXTERN_C NTSTATUS DriverEntry(_In_ _DRIVER_OBJECT* DriverObject, _In_ _UNICODE_STRING*) {

	auto ns = IoCreateDevice(DriverObject, 0, static_cast<UNICODE_STRING*>(&drvName), FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN,
		FALSE, static_cast<PDEVICE_OBJECT*>(&g_DevObj));
	NT_ASSERT(g_DevObj != __nullptr);
	DbgPrint("[+] Successfully created device object.\n");
	IoCreateSymbolicLink(static_cast<PUNICODE_STRING>(&dosName), static_cast<PUNICODE_STRING>(&drvName));

	UNICODE_STRING usString{};
	RtlInitUnicodeString(static_cast<PUNICODE_STRING>(&usString), L"\\Device\\HackSysExtremeVulnerableDriver");
	IoAttachDevice(static_cast<PDEVICE_OBJECT>(g_DevObj), static_cast<PUNICODE_STRING>(&usString), 
		static_cast<PDEVICE_OBJECT*>(&g_ExtDevObj));
	if (g_ExtDevObj == __nullptr) {
		DbgPrint("[-] IoAttachDevice failed.\n");
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrint("[+] Device driver %p has been successfully attached to.\n", static_cast<PVOID>(g_ExtDevObj));

	for (auto x = 0; x < IRP_MJ_MAXIMUM_FUNCTION; x++) {
		DriverObject->MajorFunction[x] = DefaultDispatch;
	}

	DriverObject->DriverUnload = DriverUnload;
	
	return ns;
}

EXTERN_C NTSTATUS DefaultDispatch(_In_ _DEVICE_OBJECT* DeviceObject, _In_ _IRP* Irp) {
	DbgPrint("[%s] called\n", __FUNCTION__);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 8;

	auto currStack = IoGetCurrentIrpStackLocation(Irp);
	DbgPrint("[+] currStack: %p\n", static_cast<PVOID>(currStack));
	IoCopyCurrentIrpStackLocationToNext(Irp);
	auto nextStack = IoGetNextIrpStackLocation(Irp);
	DbgPrint("[+] nextStack: %p\n", static_cast<PVOID>(nextStack));

	UINT64 context = 0x45454545;
	IoSetCompletionRoutine(Irp, static_cast<PIO_COMPLETION_ROUTINE>(IoCompletionRoutine), reinterpret_cast<PVOID>(context),
		TRUE, TRUE, TRUE);
	
	auto ns = IoCallDriver(static_cast<PDEVICE_OBJECT>(g_ExtDevObj), static_cast<PIRP>(Irp));
	return ns;
}

EXTERN_C NTSTATUS IoCompletionRoutine(_In_ _DEVICE_OBJECT* DeviceObject, _In_ _IRP* Irp, _In_opt_ PVOID Context) {
	DbgPrint("[%s] => \n", __FUNCTION__);
	DbgPrint("[>] DeviceObject: %p\n", static_cast<PVOID>(DeviceObject));
	DbgPrint("[>] Irp: %p\n", static_cast<PVOID>(Irp));
	DbgPrint("[>] Context: %p\n", Context);

	return STATUS_SUCCESS;
}

EXTERN_C VOID DriverUnload(_In_ _DRIVER_OBJECT* DriverObject) {
	IoDetachDevice(static_cast<PDEVICE_OBJECT>(g_ExtDevObj));
	IoDeleteDevice(DriverObject->DeviceObject);
}