#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>

static void *(*real_malloc)(size_t) = NULL;
static void (*real_free)(void *) = NULL;

struct StageMetrics {
    char *stage_name;
    int alloc_count;
    int heap_size;
    int peak_heap_size;
};

struct StageMetrics stages[10] = {0};
int current_stage = -1;

void *malloc(size_t size)
{
    if (!real_malloc)
        real_malloc = dlsym(RTLD_NEXT, "malloc");

    void *p = real_malloc(size);

    if (current_stage != -1) {
        struct mallinfo2 mi = mallinfo2();
        int heap_size = mi.uordblks;
        if (heap_size > stages[current_stage].peak_heap_size) {
            stages[current_stage].peak_heap_size = heap_size;
        }
        stages[current_stage].alloc_count += 1;
    }

    return p;
}

void free(void *ptr)
{
    if (!real_free)
        real_free = dlsym(RTLD_NEXT, "free");

    real_free(ptr);
}
