#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <thread>
#include <unordered_map>

ULONGLONG clear_timer = 0;
std::unordered_map<int, bool> known_ids = {};

DWORD GetProcessId()
{
	DWORD dwProcessId = 0;
	HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (hSnap && hSnap != INVALID_HANDLE_VALUE)
	{
		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hSnap, &procEntry))
		{
			do
			{
				auto ws = std::wstring(procEntry.szExeFile);
				auto ss = std::string(ws.begin(), ws.end());
				if (strstr(ss.c_str(), "Arithra2-Client.exe"))
				{
					if (known_ids[procEntry.th32ProcessID] == true)
					{
						dwProcessId = 0;
						continue;
					}
					dwProcessId = procEntry.th32ProcessID;
					break;
				}
			} while (Process32Next(hSnap, &procEntry));
		}
		CloseHandle(hSnap);
	}
	return dwProcessId;
}
void inject(int pid)
{
	std::string sDLLName = "arithra_hwid_raper.dll";
	char cDLLPath[MAX_PATH];
	GetFullPathNameA(sDLLName.c_str(), MAX_PATH, cDLLPath, 0);

	auto hProcess = OpenProcess(PROCESS_ALL_ACCESS, 0, pid);

	if (hProcess && hProcess != INVALID_HANDLE_VALUE)
	{
		LPVOID lpBaseAddress = VirtualAllocEx(hProcess, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
		if (lpBaseAddress)
		{
			if (WriteProcessMemory(hProcess, lpBaseAddress, cDLLPath, strlen(cDLLPath) + 1, 0))
			{
				if (CreateRemoteThread(hProcess, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, lpBaseAddress, 0, 0))
				{
					std::cout << "bin loaded for " << pid << std::endl;
					Sleep(1500);
					CloseHandle(hProcess);
				}
			}
		}
	}
}
int main()
{
	while (1)
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(100));

		if (GetTickCount64() > clear_timer)
		{
			clear_timer = GetTickCount64() + 1000 * (60 * 5);//5m
			known_ids.clear();
		}

		auto pid = GetProcessId();
		if (pid != 0)
		{
			known_ids[pid] = true;
			std::this_thread::sleep_for(std::chrono::milliseconds(500));
			inject(pid);
		}
	}
}