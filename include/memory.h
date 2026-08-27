#ifndef MEMORY_H
#define MEMORY_H

typedef struct {
    double total_gb;
    double used_gb;
    double percent;
} MemoryStats;

typedef struct {
    double total_gb;
    double used_gb;
    double percent;
} DiskStats;

void get_memory_stats(MemoryStats *mem);
void get_disk_stats(const char *path, DiskStats *disk);
double get_cpu_usage(void);

#endif
