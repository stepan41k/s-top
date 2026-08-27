#include "memory.h"
#include "process.h"
#include "ui.h"

int main(void) {
    ProcessInfo procs[MAX_PROCS];
    MemoryStats mem;
    DiskStats disk;

    int proc_count = 0;
    int selected = 0;
    int running = 1;

    ui_init();

    while (running) {
        get_memory_stats(&mem);
        get_disk_stats("/", &disk);
        double cpu = get_cpu_usage();
        proc_count = scan_processes(procs, MAX_PROCS);

        if (selected >= proc_count)
            selected = proc_count - 1;
        if (selected < 0)
            selected = 0;

        ui_render(&mem, &disk, cpu, procs, proc_count, selected);

        int ch = ui_get_input();
        switch (ch) {
        case 'q':
        case 'Q':
            running = 0;
            break;
        case KEY_UP:
            if (selected > 0)
                selected--;
            break;
        case KEY_DOWN:
            if (selected < proc_count - 1)
                selected++;
            break;
        case 'k':
        case 'K':
            if (proc_count > 0) {
                kill_process_by_pid(procs[selected].pid);
            }
            break;
        }
    }

    ui_cleanup();
    return 0;
}
