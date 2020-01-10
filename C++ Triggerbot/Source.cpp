#include <windows.h>
#include <psapi.h>
#include <Memoryapi.h>
#include <iostream>
#include <tlhelp32.h>

bool Enabled = false;
bool WasF4Held = false;
bool Right = false;
DWORD Address = 0xA71D70;
DWORD Offset = 0x85;
HANDLE ProgramHandle;

std::string GetProcessName(DWORD processID)
{
	CHAR szProcessName[MAX_PATH] = "<unknown>";
	HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, processID);
	if (hProcess)
	{
		if (NULL != hProcess)
		{
			HMODULE hMod;
			DWORD cbNeeded;

			if (EnumProcessModules(hProcess, &hMod, sizeof(hMod), &cbNeeded))
			{
				if (GetModuleBaseName(hProcess, hMod, szProcessName, sizeof(szProcessName) / sizeof(CHAR)) == 0)
				{
					//cout << "Error at GetModuleBaseName: " << GetLastError() << endl;
				}

				CloseHandle(hProcess);
				std::string str(szProcessName);
				return str;
			}
			else {
				//cout << "Error at EnumProcessModules: " << GetLastError() << endl;
			}
		}
		CloseHandle(hProcess);
	}


	std::string strr("hi");
	return strr;
}

int FindProcess(std::string modsearchname)
{
	DWORD aProcesses[1024], cProcesses, cbNeeded;
	unsigned int i;
	if (!EnumProcesses(aProcesses, sizeof(aProcesses), &cbNeeded))
	{
		return 0;
	}
	cProcesses = cbNeeded / sizeof(DWORD);
	for (i = 0; i < cProcesses; i++)
	{
		if (aProcesses[i] != 0)
		{
			std::string modname = GetProcessName(aProcesses[i]);
			std::size_t found = modname.find(modsearchname);
			if (found != std::string::npos) {
				return aProcesses[i];
			}
		}
	}
	return 0;
}

char ReadByte(DWORD Addr)
{
	char vall = 0;
	ReadProcessMemory(ProgramHandle, (void*)Addr, &vall, sizeof(vall), 0);
	return vall;
}

int ReadInt(DWORD Addr)
{
	int val = 0;
	ReadProcessMemory(ProgramHandle, (void*)Addr, &val, sizeof(val), 0);
	return val;
}

BYTE* GetModuleBaseAddress(DWORD dwProcID, const char* szModuleName)
{
	BYTE* ModuleBaseAddress = 0;
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, dwProcID);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		MODULEENTRY32 ModuleEntry32;
		ModuleEntry32.dwSize = sizeof(MODULEENTRY32);
		if (Module32First(hSnapshot, &ModuleEntry32))
		{
			do
			{
				if (strcmp(ModuleEntry32.szModule, szModuleName) == 0)
				{
					ModuleBaseAddress = ModuleEntry32.modBaseAddr;
					break;
				}
			} while (Module32Next(hSnapshot, &ModuleEntry32));
		}
		CloseHandle(hSnapshot);
	}
	
	return ModuleBaseAddress;
}

int main(int argc, char** argv)
{
	printf("Made by Jonni on 10.9.2019\n");
	POINT p;
	int ProcessID;
	BYTE* BaseAddr;
	DWORD SecondAddr;
	printf("Waiting for Frontend.exe...\n");
	
	while (1)
	{
		ProcessID = FindProcess("Frontend.exe");
		if (ProcessID)
		{
			printf("Found process at PID: %i\n",ProcessID);
			HANDLE phandle = OpenProcess(PROCESS_ALL_ACCESS, false, ProcessID);
			if (!phandle)
			{
				printf("Could not open Frontend.exe\n");
			}
			else
			{
				ProgramHandle = phandle;
				printf("Frontend.exe handle: %i\n", ProgramHandle);
			}
			break;
		}
		else {
			Sleep(1000);
		}
	}
	BaseAddr = GetModuleBaseAddress(ProcessID, "Frontend.exe");
	SecondAddr = ReadInt((DWORD)BaseAddr + Address);
	
	while (1)
	{
		if (GetKeyState(VK_LSHIFT) & 0x8000 && !WasF4Held && GetKeyState(VK_F4) & 0x8000)
		{
			Enabled = !Enabled;
			if (Enabled)
			{
				printf("FeTR Enabled\n");
			}
			else
			{
				printf("FeTR Disabled\n");
			}
		}
		if (GetKeyState(VK_F4) & 0x8000)
		{
			WasF4Held = true;
		}
		else {
			WasF4Held = false;
		}
		if (GetKeyState(VK_ESCAPE) & 0x800 && GetKeyState(VK_LSHIFT) & 0x800)
		{
			exit(0);
		}
		if (Enabled)
		{
			if (GetCursorPos(&p))
			{
				//BaseAddr = GetModuleBaseAddress(ProcessID, "Frontend.exe");
				SecondAddr = ReadInt((DWORD)BaseAddr + Address);
				char smell = ReadByte(SecondAddr + Offset);
				if (smell == 1)
				{
					if (Right)
					{
						mouse_event(MOUSEEVENTF_LEFTDOWN, p.x, p.y, 0, 0);
						Sleep(25);
						mouse_event(MOUSEEVENTF_LEFTUP, p.x, p.y, 0, 0);
						Right = !Right;
					}
					else
					{
						mouse_event(MOUSEEVENTF_RIGHTDOWN, p.x, p.y, 0, 0);
						Sleep(25);
						mouse_event(MOUSEEVENTF_RIGHTUP, p.x, p.y, 0, 0);
						Right = !Right;
					}

				}
			}
		}
		Sleep(1);
	}
	
	return 1;
}
// yes trash code thanks