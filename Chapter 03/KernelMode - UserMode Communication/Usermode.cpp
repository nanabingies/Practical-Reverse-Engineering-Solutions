#include <AclAPI.h>
#include <iostream>
#include <assert.h>
#include <Windows.h>
#include <AccCtrl.h>
#pragma warning(disable : 6387)

#define CREATE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x901, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define READ_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x902, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define WRITE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x903, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)
#define CLOSE_REQ	CTL_CODE(FILE_DEVICE_UNKNOWN, 0x904, METHOD_BUFFERED, FILE_READ_DATA | FILE_WRITE_DATA)

#define usDosDeviceName L"\\\\.\\SharedMemory"
#define usMemoryName L"Global\\SharedMemoryMapping"
#define NT_SUCCESS(x) ((x) >= 0 ? true : false)

typedef enum _req {
	Create,
	Read,
	Write,
	Close,
} req;

struct _shared_req {
	void* _data;
	SIZE_T _dataLen;
	HANDLE _pid;
	//void* _address;
	//SIZE_T _addressSize;
};

void createSecuritydesc() {
	DWORD dwRes;
	SECURITY_ATTRIBUTES sa;
	PSECURITY_DESCRIPTOR pSD = NULL;
	SID_IDENTIFIER_AUTHORITY SIDAuthWorld = SECURITY_WORLD_SID_AUTHORITY;
	PACL pAcl = NULL;
	PSID pEveryoneSID = NULL;
	EXPLICIT_ACCESS ea[1];

	if (!AllocateAndInitializeSid(&SIDAuthWorld, 1, SECURITY_WORLD_RID, 0, 0, 0, 0, 0, 0, 0, &pEveryoneSID)) {
		std::cout << "AllocateAndInitializeSid failed with error : " << ::GetLastError() << std::endl;
		exit(-1);
	}

	ZeroMemory(&ea, sizeof(EXPLICIT_ACCESS));
	ea[0].grfAccessPermissions = SPECIFIC_RIGHTS_ALL | STANDARD_RIGHTS_ALL;
	ea[0].grfAccessMode = SET_ACCESS;
	ea[0].grfInheritance = NO_INHERITANCE;
	ea[0].Trustee.TrusteeForm = TRUSTEE_IS_SID;
	ea[0].Trustee.TrusteeType = TRUSTEE_IS_WELL_KNOWN_GROUP;
	ea[0].Trustee.ptstrName = (LPTSTR)pEveryoneSID;

	dwRes = SetEntriesInAcl(1, ea, NULL, &pAcl);

	pSD = (PSECURITY_DESCRIPTOR)LocalAlloc(LPTR, SECURITY_DESCRIPTOR_MIN_LENGTH);

	sa.nLength = sizeof(SECURITY_ATTRIBUTES);
	sa.lpSecurityDescriptor = pSD;
	sa.bInheritHandle = FALSE;
}

auto CreateRequest(HANDLE hHandle) -> BOOL {
	DWORD bytesReturned = 0;

	auto res = ::DeviceIoControl(hHandle, CREATE_REQ, __nullptr, 0, __nullptr, 0, &bytesReturned, __nullptr);
	return res;
};

auto ReadRequest(HANDLE hHandle, _shared_req req, size_t size) -> BOOL {
	DWORD bytesReturned = 0;

	auto res = ::DeviceIoControl(hHandle, READ_REQ, &req, size, __nullptr, 0,
		&bytesReturned, __nullptr);
	return res;
};

auto WriteRequest(HANDLE hHandle, _shared_req req, size_t size) -> BOOL {
	DWORD bytesReturned = 0;

	auto res = ::DeviceIoControl(hHandle, WRITE_REQ, &req, size, __nullptr, 0,
		&bytesReturned, __nullptr);
	return res;
};

auto CloseRequest(HANDLE hHandle) -> BOOL {
	DWORD bytesReturned = 0;

	auto res = ::DeviceIoControl(hHandle, CLOSE_REQ, __nullptr, 0, __nullptr, 0, &bytesReturned, __nullptr);
	return res;
}

int main(int argc, char* argv[]) {
	auto _exit = [](const wchar_t* msg, const int error) -> VOID {
		std::wcout << msg << " " << error << std::endl;
		exit(-1);
	};

	auto hHandle = ::CreateFile(usDosDeviceName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE,
		__nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	assert(hHandle != INVALID_HANDLE_VALUE, "CreateFile failed.\n");

	createSecuritydesc();

	auto IoctlRet = CreateRequest(hHandle);
	if (IoctlRet == FALSE) _exit(L"CreateRequest Failed", ::GetLastError());
	std::cout << "[Debugging] CreateRequest successful.\n";


	wchar_t buffer[0x10];
	struct _shared_req shared_req {};
	::RtlZeroMemory(&buffer, sizeof(buffer));
	::RtlZeroMemory(&shared_req, sizeof(shared_req));
	shared_req._dataLen = 13;
	shared_req._pid = (HANDLE)::GetCurrentProcessId();
	IoctlRet = ReadRequest(hHandle, shared_req, sizeof(shared_req));
	if (IoctlRet == FALSE) _exit(L"ReadRequest Failed", ::GetLastError());
	std::cout << "ReadRequest succeeded\n";

	auto hMap = ::OpenFileMapping(FILE_MAP_ALL_ACCESS, TRUE, usMemoryName);
	if (hMap == INVALID_HANDLE_VALUE || hMap == NULL) _exit(L"OpenFileMapping Failed", ::GetLastError());
	std::cout << "[Debugging] Opened handle to file mapping with value " << std::hex << hMap << std::endl;

	auto dataPtr = reinterpret_cast<LPVOID>(::MapViewOfFile(hMap, FILE_MAP_ALL_ACCESS, 0, 0, 1024000));
	if (dataPtr == NULL) _exit(L"MapViewOfFile Failed", ::GetLastError());
	std::cout << "[Debugging] Mapped file view with value " << std::hex << dataPtr << std::endl;

	::RtlZeroMemory(&buffer, sizeof(buffer));
	::RtlCopyMemory(buffer, dataPtr, sizeof(buffer));
	std::cout << "[MapViewOfFile] Buffer : " << (PCHAR)buffer << " ; size : " << sizeof(buffer) << std::endl;

	/*char msg[] = "Mew Message";
	void* msgAddr = malloc(strlen(msg));
	memcpy(msgAddr, msg, strlen(msg));
	printf("\t\t\t%s \n", (PCHAR)msgAddr);
	printf("\t\t\t%s \n", msg);
	::RtlZeroMemory(&shared_req, sizeof(shared_req));
	shared_req._data = msg;
	shared_req._dataLen = strlen(msg) * sizeof(wchar_t);
	IoctlRet = WriteRequest(hHandle, shared_req, sizeof(shared_req));
	if (IoctlRet == FALSE) _exit(L"WriteRequest Failed", ::GetLastError());
	std::cout << "WriteRequest succeeded\n";*/

	CHAR newMessage[] = "Okay this is a new messgae\n";
	::RtlCopyMemory(dataPtr, newMessage, strlen(newMessage));
	::RtlZeroMemory(&buffer, sizeof(buffer));
	::RtlCopyMemory(buffer, dataPtr, sizeof(buffer));
	std::cout << "[MapViewOfFile] Buffer : " << (PCHAR)buffer << " ; size : " << sizeof(buffer) << std::endl;

	IoctlRet = CloseRequest(hHandle);
	if (IoctlRet == FALSE) _exit(L"CloseRequest Failed", ::GetLastError());

	::UnmapViewOfFile(dataPtr);
	::CloseHandle(hMap);
	::CloseHandle(hHandle);

	return 0;
}