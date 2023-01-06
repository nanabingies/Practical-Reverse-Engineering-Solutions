
#include "Header.h"

EXTERN_C void PcreateProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create) {
	auto _ProcessHandle = PsGetCurrentProcessId();
    IoGetCurrentProcess();
	auto _currProcess = IoGetCurrentProcess();
	auto _imageFileName = PsGetProcessImageFileName(_currProcess);

	if (Create) {
		KeEnterCriticalRegion();
		ExAcquireResourceExclusiveLite(static_cast<PERESOURCE>(&_eresource2), TRUE);

		for (auto* entry = qword_40A590.Flink; entry != &qword_40A590; entry = entry->Flink) {
			auto _imageName = reinterpret_cast<PCHAR>(*((PUCHAR)entry + 0x20));
			auto _imageHandle = reinterpret_cast<HANDLE>(*((PUCHAR)entry + 0x10));
			if (!stricmp(_imageFileName, _imageName) && (_processHandle == _imageHandle)) {
				auto _imageBaseAddress = reinterpret_cast<PVOID>(*((PUCHAR)entry + 0x1078));
				auto _imageMdl = reinterpret_cast<PMDL>(*((PUCHAR)entry + 0x1070));

				if (_imageBaseAddress) {
					MmUnmapLockedPages(_imageBaseAddress, _imageMdl);
					MmUnlockPages(_imageMdl);
					IoFreeMdl(_imageMdl);
				}

				auto _imagePoolBase = reinterpret_cast<PVOID>(*((PUCHAR)entry + 0x10B0));
				if (_imagePoolBase) {
					ExFreePoolWithTag(_imagePoolBase, POOL_TAG);
				}
				RemoveEntryList(entry);
				ExFreePoolWithTag(entry->Flink, POOL_TAG);
				break;
			}
		}

		ExReleaseResourceLite(static_cast<PERESOURCE>(&_eresource2));
		KeLeaveCriticalRegion();

		LARGE_INTEGER _li{};
		_li.QuadPart = 0xFFFFFFFFFF676980;
		KeDelayExecutionThread(KernelMode, FALSE, static_cast<PLARGE_INTEGER>(&_li));
	}

	return;
}