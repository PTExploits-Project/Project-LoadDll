#include "Api.h"

void HookFunc(HANDLE hProc, DWORD dst, DWORD src, byte* Value = 0, unsigned int size = 0)
{
	int Call = 0xE9;
	int Hook = DWORD(dst) - DWORD(src) - 5;

	WriteProcessMemory(hProc, (void*)src, &Call, 1, NULL);
	WriteProcessMemory(hProc, (void*)(src + 1), &Hook, sizeof(Hook), NULL);

	if (Value != NULL)
		WriteProcessMemory(hProc, (void*)(src + 5), Value, size, NULL);
}

DWORD getPid(string szProcID) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);

	DWORD dwPid = 0;

	if (hSnap != INVALID_HANDLE_VALUE) {
		if (!Process32First(hSnap, &pe32))
			return 0;
		
		do {
			if (strcmp(pe32.szExeFile, szProcID.c_str()) == 0) {
				dwPid = pe32.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnap, &pe32));

		if (dwPid == pe32.th32ParentProcessID) {
			CloseHandle(hSnap);
			return pe32.th32ProcessID;
		}
	}

	CloseHandle(hSnap);
	return 0;
}

DWORD getThreadID(DWORD dwPid) {
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, dwPid);
	DWORD Tid = 0;

	if (hSnap != INVALID_HANDLE_VALUE && dwPid > 0) {
		THREADENTRY32 te32;
		te32.dwSize = sizeof(THREADENTRY32);

		if (Thread32First(hSnap, &te32)) {
			do {
				if (te32.th32OwnerProcessID == dwPid)
				{
					Tid = te32.th32ThreadID;
					break;
				}
			} while (Thread32Next(hSnap, &te32));
		}
	}
	CloseHandle(hSnap);
	return Tid;
}

HANDLE openProc(DWORD dwPid) {
	return OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_OPERATION | PROCESS_VM_WRITE | PROCESS_VM_READ, FALSE, dwPid);
}

bool injectDll(HANDLE hProc, string szPath, string &sGameStatus) {
	int teste = 0xC3;
	if (!PathFileExists(szPath.c_str())) {
		sGameStatus = "Dll Status -> Caminho nao encontrado!";
		return false;
	}

	void* pAlloc = VirtualAllocEx(hProc, 0, szPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (pAlloc > 0) {
		if (!WriteProcessMemory(hProc, pAlloc, szPath.c_str(), szPath.length() + 1, NULL)) {
			sGameStatus = "Dll Status -> Falha ao escrever!";
			return false;
		}

		LPTHREAD_START_ROUTINE pLoadLibraryA = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "LoadLibraryA");

		HANDLE hThread = CreateRemoteThread(hProc, 0, 0, pLoadLibraryA, pAlloc, 0, 0);

		if (hThread == NULL) {
			sGameStatus = "Dll Status -> Falha ao chamar a API!";
			return false;
		}

		CloseHandle(hThread);
		return true;
	}
	else
		sGameStatus = "Dll Status -> Falha na alocacao de espaco!";

	return false;
}

bool unloadDll(HANDLE hProc, string szPath, string& sGameStatus) {

	LPTHREAD_START_ROUTINE pFreeLibrary = (LPTHREAD_START_ROUTINE)GetProcAddress(LoadLibraryA("kernel32"), "FreeLibrary");

	void* pAlloc = VirtualAllocEx(hProc, 0, szPath.length() + 1, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE);

	if (pAlloc > 0) {
		if (!WriteProcessMemory(hProc, pAlloc, szPath.c_str(), szPath.length() + 1, NULL))
		{
			sGameStatus = "Unload Dll -> Falha ao escrever!";
			return false;
		}
		else {
			HANDLE hThread = CreateRemoteThread(hProc, 0, 0, pFreeLibrary, pAlloc, 0, 0);

			if (hThread == NULL) {
				sGameStatus = "Unload Dll -> Falha ao chamar a API";
				return false;
			}

			CloseHandle(hThread);
			return true;
		}
	}
	return false;
}