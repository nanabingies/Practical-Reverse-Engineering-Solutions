#include "Header.h"
#pragma warning(disable : 4100)

NTSTATUS GetHandle(HANDLE PID, PHANDLE ProcessHandle) {
	*ProcessHandle = NULL;
	CLIENT_ID clientId{};
	clientId.UniqueProcess = PID;

	OBJECT_ATTRIBUTES objectAttr{};
	InitializeObjectAttributes(&objectAttr, NULL, OBJ_CASE_INSENSITIVE | OBJ_KERNEL_HANDLE, NULL, NULL);

	HANDLE pHandle{};
	auto ns = ZwOpenProcess(&pHandle, PROCESS_ALL_ACCESS, &objectAttr, &clientId);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] ZwOpenProcess failed with error code : %X\n", ns);
		return ns;
	}

	*ProcessHandle = pHandle;

	return STATUS_SUCCESS;
}

NTSTATUS CreateSection() {
	DbgPrint("[%s] ==> \n", __FUNCTION__);

	if (g_SectionHandle) {
		//ZwUnmapViewOfSection(PsGetCurrentProcessId(), &g_BaseAddress);
		ZwClose(g_SectionHandle);
	}

	auto ns = RtlCreateSecurityDescriptor(&SecDescriptor, SECURITY_DESCRIPTOR_REVISION);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("RtlCreateSecurityDescriptor failed : %X\n", ns);
		return ns;
	}

	DaclLength = sizeof(ACL) + sizeof(ACCESS_ALLOWED_ACE) * 3 + RtlLengthSid(SeExports->SeLocalSystemSid) + RtlLengthSid(SeExports->SeAliasAdminsSid) +
		RtlLengthSid(SeExports->SeWorldSid);

	Dacl = reinterpret_cast<PACL>(ExAllocatePoolWithTag(PagedPool, DaclLength, 'lcaD'));

	if (Dacl == NULL) {
		DbgPrint("ExAllocatePoolWithTag  failed  : %X\n", ns);
		return STATUS_INSUFFICIENT_RESOURCES;
	}

	ns = RtlCreateAcl(Dacl, DaclLength, ACL_REVISION);

	if (!NT_SUCCESS(ns)) {
		ExFreePool(Dacl);
		DbgPrint("RtlCreateAcl  failed  : %X\n", ns);
		return ns;
	}

	ns = RtlAddAccessAllowedAce(Dacl, ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeWorldSid);
	if (!NT_SUCCESS(ns)) {
		ExFreePool(Dacl);
		DbgPrint("RtlAddAccessAllowedAce SeWorldSid failed  : %X\n", ns);
		return ns;
	}

	ns = RtlAddAccessAllowedAce(Dacl, ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeAliasAdminsSid);
	if (!NT_SUCCESS(ns)) {
		ExFreePool(Dacl);
		DbgPrint("RtlAddAccessAllowedAce SeAliasAdminsSid failed  : %X\n", ns);
		return ns;
	}

	ns = RtlAddAccessAllowedAce(Dacl, ACL_REVISION, FILE_ALL_ACCESS, SeExports->SeLocalSystemSid);
	if (!NT_SUCCESS(ns)) {
		ExFreePool(Dacl);
		DbgPrint("RtlAddAccessAllowedAce SeLocalSystemSid failed  : %X\n", ns);
		return ns;
	}

	ns = RtlSetDaclSecurityDescriptor(&SecDescriptor, TRUE, Dacl, FALSE);
	if (!NT_SUCCESS(ns)) {
		ExFreePool(Dacl);
		DbgPrint("RtlSetDaclSecurityDescriptor failed  : %X\n", ns);
		return ns;
	}

	OBJECT_ATTRIBUTES oa{};
	InitializeObjectAttributes(&oa, &usMemoryName, OBJ_CASE_INSENSITIVE, __nullptr, &SecDescriptor);
	LARGE_INTEGER li{};
	li.QuadPart = g_SectionSize;

	ns = ZwCreateSection(&g_SectionHandle, SECTION_ALL_ACCESS, &oa, &li, PAGE_EXECUTE_READWRITE,
		SEC_COMMIT, NULL);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] ZwCreateSection failed with error code : %X.\n", ns);
		return ns;
	}

	DbgPrint("[+] Section created with handle : %p\n", g_SectionHandle);

	PVOID SectionObject{};
	ns = ObReferenceObjectByHandle(g_SectionHandle, SECTION_ALL_ACCESS, NULL, KernelMode, &SectionObject, __nullptr);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] ObReferenceObjectByHandle failed with error code : %X.\n", ns);
		ZwClose(g_SectionHandle);
		return ns;
	}

	DbgPrint("[+] Object to section handle created with value : %p\n", SectionObject);

	ExFreePool(Dacl);

	ns = ZwMapViewOfSection(g_SectionHandle, NtCurrentProcess(), &g_BaseAddress, 0, g_SectionSize, NULL,
		&g_SectionSize, ViewShare, 0, PAGE_READWRITE | PAGE_NOCACHE);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("ZwMapViewOfSection fail! Status: %X\n", ns);
		ZwClose(g_SectionHandle);
		return ns;
	}
	PCHAR msg = "nana bingies";
	RtlCopyMemory(g_BaseAddress, msg, strlen(msg));
	DbgPrint("[+] Shared memory read data in createsection : %s\n", (PCHAR)g_BaseAddress);

	DbgPrint("[+] Successfully created section.\n");
	DbgPrint("[%s] <== \n", __FUNCTION__);

	return ns;
}

NTSTATUS ReadSharedMemory(_In_ struct _shared_req* data) {
	DbgPrint("[%s] ==> \n", __FUNCTION__);
	
	if (!g_SectionHandle)
		return STATUS_INVALID_PARAMETER;

	if (g_BaseAddress)
		ZwUnmapViewOfSection(NtCurrentProcess(), g_BaseAddress);

	// map section and read data from memory
	auto ns = ZwMapViewOfSection(g_SectionHandle, NtCurrentProcess(), &g_BaseAddress, 0, g_SectionSize, NULL,
		(PSIZE_T)&g_SectionSize, ViewShare, 0, PAGE_READWRITE | PAGE_NOCACHE);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] ZwMapViewOfSection failed with error code : %X.\n", ns);
		return ns;
	}

	DbgPrint("[+] g_BaseAddress : 0x%p\n", g_BaseAddress);
	DbgPrint("Shared memory read data: %s\n", (PCHAR)g_BaseAddress);
	
	DbgPrint("[%s] <== \n", __FUNCTION__);
	return ns;
}

NTSTATUS WriteSharedMemory(_In_ struct _shared_req* data) {
	DbgPrint("[*] %s ==> \n", __FUNCTION__);
	if (data == __nullptr) {
		return STATUS_INVALID_PARAMETER;
	}

	if (!g_SectionHandle)
		return STATUS_INVALID_PARAMETER;

	if (g_BaseAddress)
		ZwUnmapViewOfSection(NtCurrentProcess(), g_BaseAddress);

	// map section and write data to memory
	auto ns = ZwMapViewOfSection(g_SectionHandle, NtCurrentProcess(), &g_BaseAddress, 0, g_SectionSize, NULL,
		(PSIZE_T)&g_SectionSize, ViewShare, 0, PAGE_READWRITE | PAGE_NOCACHE);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] ZwMapViewOfSection failed with error code : %X.\n", ns);
		return ns;
	}

	DbgPrint("[+] Writing string : %s with value : %X\n", data->_data, data->_dataLen);
	RtlCopyMemory(g_BaseAddress, data->_data, data->_dataLen);
	DbgPrint("[+] g_BaseAddress : 0x%p\n", g_BaseAddress);
	DbgPrint("Shared memory write data: %s\n", (PCHAR)g_BaseAddress);

	DbgPrint("[*] %s <== \n", __FUNCTION__);
	return ns;
}

EXTERN_C NTSTATUS IoctlDipatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	DbgPrint("[%s] ==> \n", __FUNCTION__);

	auto ntstatus = STATUS_SUCCESS;
	auto ioStackLocation = IoGetCurrentIrpStackLocation(Irp);
	NT_ASSERT(ioStackLocation != __nullptr);

	auto ioCtlCode = ioStackLocation->Parameters.DeviceIoControl.IoControlCode;
	auto inputBufferLen = ioStackLocation->Parameters.DeviceIoControl.InputBufferLength;
	auto systemBuffer = reinterpret_cast<struct _shared_req*>(Irp->AssociatedIrp.SystemBuffer);

	switch (ioCtlCode) {
	case CREATE_REQ: 
		ntstatus = CreateSection();
		break;

	case READ_REQ: 
		ntstatus = ReadSharedMemory(systemBuffer);
		break;

	case WRITE_REQ: 
		ntstatus = WriteSharedMemory(systemBuffer);
		break;

	case CLOSE_REQ:
		ntstatus = ZwClose(g_SectionHandle);
		break;

	default:
		DbgPrint("[-] Invalid request.\n");
		break;
	}

	DbgPrint("[%s] <== \n", __FUNCTION__);

	Irp->IoStatus.Information = inputBufferLen;
	Irp->IoStatus.Status = ntstatus;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return ntstatus;
}

EXTERN_C NTSTATUS DefaultDispatch(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp) {
	Irp->IoStatus.Information = 0;
	Irp->IoStatus.Status = STATUS_SUCCESS;
	IofCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}

EXTERN_C VOID DriverUnload(_In_ PDRIVER_OBJECT DriverObject) {
	IoDeleteSymbolicLink(&usDosDeviceName);
	IoDeleteDevice(DriverObject->DeviceObject);
}

EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	DbgPrint("[*] %s ==> \n", __FUNCTION__);

	auto ns = IoCreateDevice(DriverObject, 0, &usDriverName, FILE_DEVICE_UNKNOWN, FILE_DEVICE_SECURE_OPEN, FALSE,
		&g_DeviceObject);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] IoCreateDevice failed with error code : %X.\n", ns);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	ns = IoCreateSymbolicLink(&usDosDeviceName, &usDriverName);
	if (!NT_SUCCESS(ns)) {
		DbgPrint("[-] IoCreateSymbolicLink failed with error code : %X.\n", ns);
		return STATUS_FAILED_DRIVER_ENTRY;
	}

	DbgPrint("[+] DeviceObject created successfully : 0x%p\n", (PVOID)g_DeviceObject);

	for (auto idx = 0; idx < IRP_MJ_MAXIMUM_FUNCTION; ++idx) {
		DriverObject->MajorFunction[idx] = DefaultDispatch;
	}

	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = IoctlDipatch;
	DriverObject->DriverUnload = DriverUnload;

	//CreateSection();

	DbgPrint("[*] %s <== \n", __FUNCTION__);
	return STATUS_SUCCESS;
}