#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

int main(int argc, char** argv) {

    const uint64_t gb = ((uint64_t)1) * 1024 * 1024 * 1024;
    const uint64_t page_size = 4 * 1024;

    void* allocated = 0;
    const uint64_t alloc_start = 10 * page_size;
    const uint64_t alloc_size = 6 * gb - alloc_start;

    allocated = mmap(
        (void*) alloc_start,
        alloc_size,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED,
        -1,
        0
    );

    if(!allocated) {
        printf("NOT ALLOCED:%p\n", (void*) alloc_start);
    }

    int allocated_correct = allocated == (void*) alloc_start;
    if(!allocated_correct) {
        printf("GOT %p not %p : INCORRECT\n", allocated, (void*) alloc_start);
    }

    printf("Done\n");
}