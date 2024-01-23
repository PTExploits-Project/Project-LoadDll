#include <Windows.h>
#include <iostream>
#include <ctime>
#include "Api.h"

using namespace std;

DWORD dwPid = 0;
HANDLE hProc = 0, hThread = 0;
bool bhProc;

int updateTime;
bool bConsoleUpdate;

char szPath[MAX_PATH];
char* Dll = (char*)"Dll_BPT.dll";

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
	POINT cursorPos;
	HWND hwnd = NULL, hwndPT = NULL;
	DWORD dwPid = 0;
	HANDLE hProc = NULL;

	while (true) {
		GetCursorPos(&cursorPos);

		hwnd = WindowFromPoint(cursorPos);

		if (hwnd != NULL) {
			hwndPT = FindWindowA(NULL, "Priston Tale");

			if (hwnd != hwndPT)
				sGameStatus = "Game Status -> Aguardando Priston Tale..";
			else
				GetWindowThreadProcessId(hwndPT, &dwPid);

			if (dwPid > 0) {
				hProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwPid);

				if (hProc != NULL) {
					if (injectDll(hProc, szPath, sGameStatus)) {
						sGameStatus = "Game Status -> Dll injetada com sucesso!";
						bConsoleUpdate = true;

						CloseHandle(hProc);
						Sleep(3000);
						ExitProcess(0);
					}
					else
						CloseHandle(hProc);
				}
				else {
					sGameStatus = "Game Status -> Erro ao abrir OpenProcess..";
					bConsoleUpdate = true;

					ExitThread(0);
				}
			}
		}
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