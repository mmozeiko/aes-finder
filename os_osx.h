#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

#include <sys/sysctl.h>
#include <errno.h>
#include <mach/mach_init.h>
#include <mach/mach_traps.h>
#include <mach/mach_port.h>
#include <mach/vm_map.h>

static void os_startup()
{
}

static kinfo_proc* os_processes;
static size_t os_process_count;
static size_t os_process_idx;

static bool os_enum_start()
{
    static const int name[] = { CTL_KERN, KERN_PROC, KERN_PROC_ALL, 0 };

    size_t length;

    os_processes = NULL;

    int err;
    bool done = false;
    do
    {
        err = sysctl((int*)name, (sizeof(name)/sizeof(*name)) - 1, NULL, &length, NULL, 0);
        if (err == -1)
        {
            break;
        }

        os_processes = (kinfo_proc*)realloc(os_processes, length);
        if (os_processes == NULL)
        {
            err = -1;
            break;
        }

        err = sysctl((int*)name, (sizeof(name)/sizeof(*name)) - 1, os_processes, &length, NULL, 0);
        if (err == -1 && errno == ENOMEM)
        {
           continue;
        }

        break;
    }
    while (err == 0 && ! done);

    if (err != 0)
    {
        free(os_processes);
        return false;
    }

    os_process_count = length / sizeof(kinfo_proc);
    os_process_idx = 0;

    return true;
}

static uint32_t os_enum_next(const char* name)
{
    while (os_process_idx < os_process_count)
    {
        kinfo_proc* p = os_processes + os_process_idx++;

        if (strcmp(name, p->kp_proc.p_comm) == 0)
        {
            return p->kp_proc.p_pid;
        }
    }

    return 0;
}

static void os_enum_end()
{
    free(os_processes);
}

static task_t os_process_task;
static vm_address_t os_process_addr;
static vm_size_t os_process_size;

static bool os_process_begin(uint32_t pid)
{
    kern_return_t kr = task_for_pid(mach_task_self(), pid, &os_process_task);

    if (kr != KERN_SUCCESS)
    {
        return false;
    }

    os_process_addr = 0;
    os_process_size = 0;

    return true;
}

static uint64_t os_process_next(uint64_t* size)
{
    for (;;)
    {
        os_process_addr += os_process_size;

        uint32_t depth = 1;
        vm_region_submap_info_64 info;
        mach_msg_type_number_t count = VM_REGION_SUBMAP_INFO_COUNT_64;

        kern_return_t kr = vm_region_recurse_64(os_process_task, &os_process_addr, &os_process_size, &depth, (vm_region_info_64_t)&info, &count);
        if (kr != KERN_SUCCESS)
        {
            return 0;
        }

        if ((info.protection & VM_PROT_READ) == 0)
        {
            continue;
        }

                
        *size = os_process_size;
        return os_process_addr;
    }
}

static uint32_t os_process_read(uint64_t addr, void* buffer, uint32_t size)
{
    vm_size_t read = size;
    if (vm_read_overwrite(os_process_task, addr, size, (vm_address_t)buffer, &read) != KERN_SUCCESS)
    {
        return 0;
    }

    return (uint32_t)read;
}

static void os_process_end()
{
    mach_port_deallocate(mach_task_self(), os_process_task);
}
