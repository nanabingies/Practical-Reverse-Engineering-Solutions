
#include "Header.h"

EXTERN_C void PcreateProcessNotifyRoutine(_In_ HANDLE ParentId, _In_ HANDLE ProcessId, _In_ BOOLEAN Create) {
	auto _ProcessHandle = PsGetCurrentProcessId();
}