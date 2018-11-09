#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <time.h>

#if defined(WIN32)
#include "os_windows.h"
#elif defined(__ANDROID__)
#include "os_android.h"
#elif defined(__linux__)
#include "os_linux.h"
#elif defined(__APPLE__)
#include "os_osx.h"
#else
#error Unknown OS!
#endif

#define BUFFER_SIZE 8192

static void dump_mem(uint32_t pid, char* output_dir)
{

    printf("Dumping PID %u ...\n", pid);

    if (!os_process_begin(pid))
    {
        printf("Failed to open process\n");
        return;
    }

    char output_path[255];
    snprintf(output_path, sizeof(output_path), "%s/%u.bin", output_dir, pid);

    FILE *fp = fopen(output_path, "wb");
    if (fp == NULL) {
        printf("Failed to open output file");
        return;
    }

	uint8_t buffer[BUFFER_SIZE];
    uint64_t pos = 0;

    uint64_t region = 0;
    uint64_t region_size = 0;
    uint64_t size_remaining = 0;
	uint64_t read = 0;

    uint64_t addr = 0;

    clock_t t0 = clock();
    uint64_t total = 0;

    for (;;)
    {

        if (size_remaining == 0)
        {
            uint64_t next_size;
            uint64_t next = os_process_next(&next_size);
            
            if (next == 0)
            {
                break;
            }

            if (region + region_size != next)
            {
                pos = 0;
            }

            addr = region = next;
            size_remaining = region_size = next_size;
        }
		
		read = BUFFER_SIZE;
		if (read > size_remaining) {
			read = size_remaining;
		}
        read = os_process_read(region, buffer, (uint32_t)read);

        if (read == 0)
        {
            pos = 0;
            size_remaining = 0;

            continue;
		}
		else {
			// Write out the region
			fwrite(buffer, 1, read, fp);
		}

        total += read;
        region += read;
        size_remaining -= read;
        pos += read;
    }

    clock_t t1 = clock();
    double time = double(t1 - t0) / CLOCKS_PER_SEC;
    const double MB = 1024.0 * 1024.0;
    printf("Processed %.2f MB, speed = %.2f MB/s\n", total / MB, total / MB / time);

    fclose(fp);
    os_process_end();
}

int main(int argc, char** argv) {
	
	if (argc != 4)
    {
        printf("Usage: memdump [-p pid | -n process-name] <output folder>\n");
        return EXIT_FAILURE;
    }

	char *output_dir = argv[3];

    if (strcmp(argv[1], "-p") == 0) {
        /* PID */
        char* cpid = argv[2];
        uint32_t pid = atoi(cpid);
        dump_mem(pid, output_dir);

    } else if (strcmp(argv[1], "-n") == 0) {
        /* Process Name */

        os_startup();

        if (os_enum_start())
        {
            while (uint32_t pid = os_enum_next(argv[2]))
            {
                dump_mem(pid, output_dir);
            }
            os_enum_end();
        }
        
    } else {
        printf("Unknown command switch %s provided\n", argv[1]);
        return EXIT_FAILURE;
    }

	return 0;
}
