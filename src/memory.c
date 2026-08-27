#include "memory.h"
#include <stdio.h>
#include <sys/statvfs.h>
#include <unistd.h>

typedef struct {
    unsigned long long user, nice, system, idle, iowait, irq, softirq, steal;
} CPUData;

static void read_cpu_data(CPUData *data) {
    FILE *fp = fopen("/proc/stat", "r");
    if (!fp)
        return;
    char buffer[256];
    if (fgets(buffer, sizeof(buffer), fp)) {
        sscanf(buffer, "cpu %llu %llu %llu %llu %llu %llu %llu %llu",
               &data->user, &data->nice, &data->system, &data->idle,
               &data->iowait, &data->irq, &data->softirq, &data->steal);
    }
    fclose(fp);
}

double get_cpu_usage(void) {
    static CPUData prev = {0};
    CPUData curr;
    read_cpu_data(&curr);

    if (prev.user == 0 && prev.idle == 0) {
        prev = curr;
        return 0.0;
    }

    unsigned long long total1 = prev.user + prev.nice + prev.system +
                                prev.idle + prev.iowait + prev.irq +
                                prev.softirq + prev.steal;
    unsigned long long total2 = curr.user + curr.nice + curr.system +
                                curr.idle + curr.iowait + curr.irq +
                                curr.softirq + curr.steal;

    unsigned long long idle1 = prev.idle + prev.iowait;
    unsigned long long idle2 = curr.idle + curr.iowait;

    double total_diff = (double)(total2 - total1);
    double idle_diff = (double)(idle2 - idle1);

    prev = curr;

    if (total_diff == 0)
        return 0.0;
    return ((total_diff - idle_diff) / total_diff) * 100.0;
}

void get_memory_stats(MemoryStats *mem) {
    FILE *fp = fopen("/proc/meminfo", "r");
    if (!fp)
        return;

    char line[256];
    unsigned long long total_kb = 0, avail_kb = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (sscanf(line, "MemTotal: %llu kB", &total_kb) == 1)
            continue;
        if (sscanf(line, "MemAvailable: %llu kB", &avail_kb) == 1)
            continue;
    }
    fclose(fp);

    unsigned long long used_kb = total_kb - avail_kb;
    mem->total_gb = (double)total_kb / (1024 * 1024);
    mem->used_gb = (double)used_kb / (1024 * 1024);
    mem->percent = ((double)used_kb / total_kb) * 100.0;
}

void get_disk_stats(const char *path, DiskStats *disk) {
    struct statvfs stat;
    if (statvfs(path, &stat) != 0)
        return;

    unsigned long long total_bytes = stat.f_blocks * stat.f_frsize;
    unsigned long long free_bytes = stat.f_bavail * stat.f_frsize;
    unsigned long long used_bytes = total_bytes - free_bytes;

    disk->total_gb = (double)total_bytes / (1024 * 1024 * 1024);
    disk->used_gb = (double)used_bytes / (1024 * 1024 * 1024);
    disk->percent = ((double)used_bytes / total_bytes) * 100.0;
}
