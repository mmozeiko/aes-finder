#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0502
#include <windows.h>
#include <tlhelp32.h>

static void os_startup()
{
    HANDLE hToken;
    if (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken))
    {
        TOKEN_PRIVILEGES tp;
        if (LookupPrivilegeValue(NULL, "SeDebugPrivilege", &tp.Privileges[0].Luid))
        {
            tp.PrivilegeCount = 1;
            tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;

            AdjustTokenPrivileges(hToken, FALSE, &tp, 0, NULL, 0);
        }

        CloseHandle(hToken);
    }
}

static HANDLE os_snapshot;
static bool os_entry_first;
static PROCESSENTRY32 os_entry;

static bool os_enum_start()
{
    os_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (os_snapshot == INVALID_HANDLE_VALUE)
    {
        return false;
    }

    os_entry_first = true;
    os_entry.dwSize = sizeof(os_entry);

    return !!Process32First(os_snapshot, &os_entry);
}

static uint32_t os_enum_next(const char* name)
{
    if (!os_entry_first)
    {
        if (!Process32Next(os_snapshot, &os_entry))
        {
            return 0;
        }
    }
    else
    {
        os_entry_first = false;
    }

    do
    {
        if (_stricmp(os_entry.szExeFile, name) == 0)
        {
            return os_entry.th32ProcessID;
        }
    }
    while (Process32Next(os_snapshot, &os_entry));

    return 0;
}

static void os_enum_end()
{
    CloseHandle(os_snapshot);
}

static HANDLE os_process;
static MEMORY_BASIC_INFORMATION os_process_info;

static bool os_process_begin(uint32_t pid)
{
    os_process_info.BaseAddress = NULL;
    os_process_info.RegionSize = 0;

    os_process = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, pid);
    if (os_process == NULL)
    {
        return false;
    }

#if defined(_M_IX86) || defined(__i386__)
    BOOL wow64;
    if (IsWow64Process(GetCurrentProcess(), &wow64) && wow64)
    {
        if (IsWow64Process(os_process, &wow64) && !wow64)
        {
            // 32-bit (wow) process should not touch 64-bit (non-wow) process
			// In other words, if the dumper is 32bit, cannot open 64 bit processes
            CloseHandle(os_process);
            return false;
        }
    }
#endif

    return true;
}

static uint64_t os_process_next(uint64_t* size)
{
    for (;;)
    {
        LPCVOID addr = (char*)os_process_info.BaseAddress + os_process_info.RegionSize;

        if (VirtualQueryEx(os_process, addr, &os_process_info, sizeof(os_process_info)) == FALSE)
        {
            return 0;
        }

        if ((os_process_info.Protect & (PAGE_READONLY | PAGE_READWRITE)) == 0)
        {
            continue;
        }

        *size = os_process_info.RegionSize;
        return (uint64_t)os_process_info.BaseAddress;
    }
}

static uint32_t os_process_read(uint64_t addr, void* buffer, uint32_t size)
{
    SIZE_T read = size;
    if (!ReadProcessMemory(os_process, (LPCVOID)addr, buffer, read, &read))
    {
        return 0;
    }
    return (uint32_t)read;
}

static void os_process_end()
{
    CloseHandle(os_process);
}
