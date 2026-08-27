#ifndef UI_H
#define UI_H

#include <ncurses.h>

#include "memory.h"
#include "process.h"

void ui_init(void);
void ui_cleanup(void);
void ui_render(const MemoryStats *mem, const DiskStats *disk, double cpu,
               const ProcessInfo *procs, int proc_count, int selected_idx);
int ui_get_input(void);

#endif
