
#include <Windows.h>
#include <iostream>

int main(int argc, char* argv[]) {
	const auto hHandle = ::CreateFileW(L"\\\\.\\HackSysExtremeVulnerableDriver", GENERIC_READ | GENERIC_WRITE, 
		FILE_SHARE_READ | FILE_SHARE_WRITE, __nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, __nullptr);
	if (hHandle == INVALID_HANDLE_VALUE) {
		std::cout << "[-] CreateFileW failed.\n";
		exit(-1);
	}

	std::cout << "[+] Opened handle to device with value " << std::hex << static_cast<PVOID>(hHandle) << std::endl;

	system("PAUSE");
	::CloseHandle(hHandle);

	return 0;
}