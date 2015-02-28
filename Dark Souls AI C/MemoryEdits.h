#ifndef MemoryEdits_H
#define MemoryEdits_H

#include <Windows.h>
#include <tlhelp32.h>

typedef unsigned long long ullong;

//get the process id from the name
int GetProcessIdFromName(const char* ProcName);

//find base address of process
ullong GetModuleBase(const int ProcessID, const char * ModuleName);

//add the pointer offsets to the address
ullong FindPointerAddr(HANDLE pHandle, const ullong baseaddr, const size_t length, const int * offsets);

#endif