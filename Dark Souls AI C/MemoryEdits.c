#include "MemoryEdits.h"

int GetProcessIdFromName(const char* ProcName) {
	PROCESSENTRY32 entry;
	entry.dwSize = sizeof(PROCESSENTRY32);
	int processid = -1;
	//get all running processes
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	//search through all running programs until we find the one that matches
	if (Process32First(hSnapshot, &entry)) {
		do {
			char processname[20];//just go with 20
			size_t charsConverted = 0;
			wcstombs_s(&charsConverted, processname, 20, entry.szExeFile, 19);
			//compare the process name and desired process name
			if (_stricmp(processname, ProcName) == 0){
				processid = entry.th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, &entry));
	}

	if (hSnapshot != INVALID_HANDLE_VALUE){
		CloseHandle(hSnapshot);
	}

	return processid;
}

ullong GetModuleBase(const int ProcessID, const char * ModuleName){
	//go through the programs loaded module's and find the primary one (same name as program process)
	void* hSnap;
	MODULEENTRY32 Mod32 = { 0 };

	if ((hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, ProcessID)) == INVALID_HANDLE_VALUE){
		return (ullong)(Mod32.modBaseAddr);
	}

	Mod32.dwSize = sizeof(MODULEENTRY32);
	while (Module32Next(hSnap, &Mod32)){
		char szModule[20];//just go with 20 for name length
		size_t charsConverted = 0;
		wcstombs_s(&charsConverted, szModule, 20, Mod32.szModule, 19);
		//if the module name matches the process name
		if (_stricmp(ModuleName, szModule) == 0){
			CloseHandle(hSnap);
			return (ullong)(Mod32.modBaseAddr);
		}
	}

	CloseHandle(hSnap);
	return 0 ;
}

ullong FindPointerAddr(HANDLE pHandle, const ullong baseaddr, const size_t length, const int * offsets){
	ullong address = baseaddr;
	size_t i = 0;
	do{
		ReadProcessMemory(pHandle, (LPCVOID)address, &address, 4, NULL);
		address += offsets[i];
		i++;
	} while (i < length);

	return address;
}