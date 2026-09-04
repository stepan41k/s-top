#include "memory.h"
#include "process.h"
#include "ui.h"
#include <time.h>

static long long get_time_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

int main(void) {
    ProcessInfo procs[MAX_PROCS];
    MemoryStats mem;
    DiskStats disk;
    double cpu = 0.0;

    int proc_count = 0;
    int selected = 0;
    int running = 1;

    ui_init();

    long long last_poll_time = 0;
    const long long poll_interval_ms = 1000;

    while (running) {
        long long now = get_time_ms();

        if (now - last_poll_time >= poll_interval_ms || last_poll_time == 0) {
            get_memory_stats(&mem);
            get_disk_stats("/", &disk);
            cpu = get_cpu_usage();
            proc_count = scan_processes(procs, MAX_PROCS);
            last_poll_time = now;
        }

        if (selected >= proc_count) selected = proc_count - 1;
        if (selected < 0) selected = 0;

        ui_render(&mem, &disk, cpu, procs, proc_count, selected);

        int ch = ui_get_input();
        switch (ch) {
            case 'q':
            case 'Q':
                running = 0;
                break;
            case KEY_UP:
                if (selected > 0) selected--;
                break;
            case KEY_DOWN:
                if (selected < proc_count - 1) selected++;
                break;
            case 'k':
            case 'K':
                if (proc_count > 0) {
                    kill_process_by_pid(procs[selected].pid);
                    last_poll_time = 0;
                }
                break;
        }
    }

    ui_cleanup();
    return 0;
}