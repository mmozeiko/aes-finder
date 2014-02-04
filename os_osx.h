#pragma once

#include <stddef.h>
#include <stdint.h>

static void os_startup()
{
}

static bool os_enum_start()
{
    return false;
}

static uint32_t os_enum_next(const char* name)
{
    (void)name;

    return false;
}

static void os_enum_end()
{
}

static bool os_process_begin(uint32_t pid)
{
    (void)pid;

    return false;
}

static uint64_t os_process_next(uint64_t* size)
{
    (void)size;

    return 0;
}

static uint32_t os_process_read(uint64_t addr, void* buffer, uint32_t size)
{
    (void)addr;
    (void)buffer;
    (void)size;

    return 0;
}

static void os_process_end()
{
}
