#include <Windows.h>
#include <iostream>
#include "Api.h"

using namespace std;

DWORD dwPid = 0;
HANDLE hProc = 0, hThread = 0;
bool bhProc;

int updateTime;
bool bConsoleUpdate;

char szPath[MAX_PATH];
char* Dll = (char*)"ZForce.dll";

string sGameStatus;

DWORD GetPid()
{
	HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

	if (hSnapShot != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 pe32;
		pe32.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(hSnapShot, &pe32))
		{
			do {
				std::string full_name(pe32.szExeFile);
				if (full_name.find(".exe ") != -1)
				{
					CloseHandle(hSnapShot);
					return pe32.th32ProcessID;
				}
			} while (Process32Next(hSnapShot, &pe32));
		}
	}
	CloseHandle(hSnapShot);
	return 0;
}

void status() {

	while (true) {
		dwPid = GetPid();

		if (dwPid > 0) {
			if (!bhProc) {
				hProc = openProc(dwPid);

				if (hProc > 0) {
					sGameStatus = "Game Status -> Permissao concedida!";
					bConsoleUpdate = true, bhProc = true;
				}
				else {
					sGameStatus = "Game Status -> Permissao negada!";
					bConsoleUpdate = true;
				}

				DWORD dwThreadId = getThreadID(dwPid);
				hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, dwThreadId);
			}
			else {
				SuspendThread(hThread);
				bool bInject = injectDll(hProc, szPath, sGameStatus);

				if (!bInject)
					bConsoleUpdate = true;
				else {
					sGameStatus = "Game Status -> Dll injetada!";
					bConsoleUpdate = true;
				}

				Sleep(5000);

				ResumeThread(hThread);
				//unloadDll(hProc, "Dll_Client", sGameStatus);
				ExitProcess(0);
			}
		}
		else
			sGameStatus = "Game Status -> Aguardando Priston Tale..";
	}
}

int main() {
	updateTime = clock();
	bConsoleUpdate = true;

	GetFullPathName(Dll, MAX_PATH, szPath, 0);

	CreateThread(0, 0, (LPTHREAD_START_ROUTINE)status, 0, 0, 0);

	while (true) {
		if (bConsoleUpdate || clock() - updateTime > 3000) {
			updateTime = clock();
			system("cls");

			cout << "Welcome to Brazil Priston Tale - Dll Injector\n";
			cout << "Version 1.0.0\n";
			cout << "Developed by dwSize\n\n";
			cout << sGameStatus;

			bConsoleUpdate = false;
		}
	}
}