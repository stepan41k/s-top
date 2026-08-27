#include "process.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <ctype.h>
#include <signal.h>

static void read_proc_status(int pid, ProcessInfo *proc) {
    char path[256];
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    FILE *fp = fopen(path, "r");
    if (!fp) return;

    proc->pid = pid;
    proc->rss_mb = 0;
    strcpy(proc->name, "unknown");

    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name:\t%255s", proc->name);
        } else if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State:\t%c", &proc->state);
        } else if (strncmp(line, "VmRSS:", 6) == 0) {
            long kb;
            if (sscanf(line, "VmRSS:\t%ld", &kb) == 1) {
                proc->rss_mb = kb / 1024;
            }
        }
    }
    fclose(fp);
}

int scan_processes(ProcessInfo *procs, int max_procs) {
    DIR *dir = opendir("/proc");
    if (!dir) return 0;

    struct dirent *entry;
    int count = 0;

    while ((entry = readdir(dir)) != NULL && count < max_procs) {
        if (isdigit(entry->d_name[0])) {
            int pid = atoi(entry->d_name);
            read_proc_status(pid, &procs[count]);
            count++;
        }
    }
    closedir(dir);
    return count;
}

int kill_process_by_pid(int pid) {
    if (kill(pid, SIGTERM) == 0) return 1;
    if (kill(pid, SIGKILL) == 0) return 1;
    return 0;
}