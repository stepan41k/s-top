#ifndef PROCESS_H
#define PROCESS_H

#define MAX_PROCS 1024

typedef struct {
    int pid;
    char name[256];
    char state;
    long rss_mb;
} ProcessInfo;

int scan_processes(ProcessInfo *procs, int max_procs);
int kill_process_by_pid(int pid);

#endif
