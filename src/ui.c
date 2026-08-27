#include "ui.h"
#include <ncurses.h>

void ui_init(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(1000); 
}

void ui_cleanup(void) {
    endwin();
}

void ui_render(const MemoryStats *mem, const DiskStats *disk, double cpu, 
               const ProcessInfo *procs, int proc_count, int selected_idx) {
    clear();

    mvprintw(0, 0, "SYSTEM MONITOR");
    mvprintw(1, 0, "RAM:  %5.2f / %5.2f GB (%4.1f%%)", mem->used_gb, mem->total_gb, mem->percent);
    mvprintw(2, 0, "Disk: %5.2f / %5.2f GB (%4.1f%%)", disk->used_gb, disk->total_gb, disk->percent);
    mvprintw(3, 0, "CPU:  %4.1f%%", cpu);

    int start_row = 5;
    attron(A_BOLD | A_REVERSE);
    mvprintw(start_row, 0, " PID      | Status | Memory (MB) | Process Name");
    attroff(A_BOLD | A_REVERSE);

    int max_display = LINES - start_row - 2;
    for (int i = 0; i < proc_count && i < max_display; i++) {
        if (i == selected_idx) {
            attron(A_REVERSE);
        }

        mvprintw(start_row + 1 + i, 0, " %-8d | %-9c | %-11ld | %s",
                 procs[i].pid, procs[i].state, procs[i].rss_mb, procs[i].name);

        if (i == selected_idx) {
            attroff(A_REVERSE);
        }
    }

    mvprintw(LINES - 1, 0, "[Arrows]: Navigation | [K]: Kill process | [Q]: Quit");
    refresh();
}

int ui_get_input(void) {
    return getch();
}