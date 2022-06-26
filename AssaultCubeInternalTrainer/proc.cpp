#include "pch.h"
#include "proc.h"



DWORD GetProcId(const wchar_t* procName) {
	DWORD procId = 0;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hsnap != INVALID_HANDLE_VALUE) {

		PROCESSENTRY32 procEntry;
		procEntry.dwSize = sizeof(procEntry);

		if (Process32First(hsnap, &procEntry)) {
			do
			{
				if (!_wcsicmp(procEntry.szExeFile, procName)) {

					procId = procEntry.th32ProcessID;
					break;
				}

			} while (Process32Next(hsnap, &procEntry));
		}

	}
	CloseHandle(hsnap);
	return procId;
}

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName) {
	uintptr_t modBaseAddr = 0;
	HANDLE hsnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
	if (hsnap != INVALID_HANDLE_VALUE) {
		MODULEENTRY32 modEntry;
		modEntry.dwSize = sizeof(modEntry);
		if (Module32First(hsnap, &modEntry)) {
			do {
				if (!_wcsicmp(modEntry.szModule, modName)) {
					modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
					break;
				}

			} while (Module32Next(hsnap, &modEntry));

		}

	}
	CloseHandle(hsnap);
	return modBaseAddr;
}