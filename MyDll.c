#include <windows.h>
#include <stdio.h>

extern __declspec(dllexport) void MsgBoxPayload() {
	MessageBoxA(NULL, "Hacking with 0xdaemon", "Wowo!!", MB_OK | MB_ICONINFORMATION);
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID lpReserved) {
	switch (dwReason) {
	case DLL_PROCESS_ATTACH: {
		MsgBoxPayload();
		break;
	};
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}

	return TRUE;
}
