#include "ui.h"
#include <ncurses.h>

void ui_init(void) {
    initscr();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    curs_set(0);
    timeout(50);
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
    if (max_display <= 0) max_display = 1;

    static int scroll_offset = 0;

    if (selected_idx < scroll_offset) {
        scroll_offset = selected_idx;
    } else if (selected_idx >= scroll_offset + max_display) {
        scroll_offset = selected_idx - max_display + 1;
    }
    
    for (int i = 0; i < max_display && (i + scroll_offset) < proc_count; i++) {
        int idx = i + scroll_offset;
        
        if (idx == selected_idx) {
            attron(A_REVERSE);
        }

        mvprintw(start_row + 1 + i, 0, " %-8d | %-9c | %-11ld | %s",
                 procs[idx].pid, procs[idx].state, procs[idx].rss_mb, procs[idx].name);

        if (idx == selected_idx) {
            attroff(A_REVERSE);
        }
    }

    mvprintw(LINES - 1, 0, "[Proccesses: %d/%d] | [Arrows]: Navigation | [K]: Kill process | [Q]: Quit", selected_idx + 1, proc_count);
    refresh();
}

int ui_get_input(void) {
    return getch();
}