#include <Windows.h>
#include <TlHelp32.h>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <Shlwapi.h>
//Library needed by Linker to check file existance
#pragma comment(lib, "Shlwapi.lib")

using namespace std;

DWORD getPid(string szProcID);
DWORD getThreadID(DWORD dwPid);
HANDLE openProc(DWORD dwPid);
bool injectDll(HANDLE hProc, string szPath, string &sGameStatus);
bool unloadDll(HANDLE hProc, string szPath, string& sGameStatus);