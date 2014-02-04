#pragma once

#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#include <sys/types.h>
#include <sys/uio.h>
#include <dirent.h>
#include <unistd.h>
#include <errno.h>

static char os_self_name[16];

static void os_startup()
{
    sprintf(os_self_name, "%u", getpid());
}

static DIR* os_dir;

static bool os_enum_start()
{
    os_dir = opendir("/proc");
    return !!os_dir;
}

static uint32_t os_enum_next(const char* name)
{
    ssize_t name_len = strlen(name);

    for (;;)
    {
        struct dirent* de = readdir(os_dir);
        if (de == NULL)
        {
            return 0;
        }

        if (strcmp(de->d_name, "self") == 0 || strcmp(de->d_name, os_self_name) == 0)
        {
            continue;
        }

        char path[1024];
        snprintf(path, sizeof(path), "/proc/%s/exe", de->d_name);

        char link[1024];
        ssize_t link_len = readlink(path, link, sizeof(link));
        if (link_len == -1)
        {
            continue;
        }

        if (link_len < name_len || strncmp(link + link_len - name_len, name, name_len) != 0)
        {
            continue;
        }

        return atoi(de->d_name);
    }
}

static void os_enum_end()
{
    closedir(os_dir);
}

static FILE* os_maps;
static pid_t os_process_pid;

static bool os_process_begin(uint32_t pid)
{
    os_process_pid = pid;

    char path[1024];
    snprintf(path, sizeof(path), "/proc/%u/maps", pid);

    os_maps = fopen(path, "r");
    if (os_maps == NULL)
    {
        return false;
    }

    // check if we are allowed to read process memory
    {
        char buffer;
        struct iovec in = { 0, 1 };
        struct iovec out = { &buffer, 1 };

        process_vm_readv(pid, &out, 1, &in, 1, 0);

        if (errno == EPERM)
        {
            fclose(os_maps);
            return false;
        }
    }

    return true;
}

static uint64_t os_process_next(uint64_t* size)
{
    for (;;)
    {
        char line[1024];
        if (fgets(line, sizeof(line), os_maps) == NULL)
        {
            return 0;
        }

        unsigned long long start;
        unsigned long long end;
        char flag;
        if (sscanf(line, "%llx-%llx %c", &start, &end, &flag) != 3)
        {
            continue;
        }

        if (flag != 'r')
        {
            continue;
        }

        *size = end - start;
        return (uint64_t)start;
    }
}

static uint32_t os_process_read(uint64_t addr, void* buffer, uint32_t size)
{
    struct iovec in = { (void*)addr, (size_t)size };
    struct iovec out = { buffer, (size_t)size };

    ssize_t read = process_vm_readv(os_process_pid, &out, 1, &in, 1, 0);

    return read < 0 ? 0 : read;
}

static void os_process_end()
{
    fclose(os_maps);
}
