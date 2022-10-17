#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

int main(int argc, char** argv) {

    const uint64_t gb = ((uint64_t)1) * 1024 * 1024 * 1024;
    const uint64_t page_size = 4 * 1024;

    void* allocated = 0;
    void* page_addr = (void*)(((uintptr_t)0xffffffffffffffff) - 2 * gb);

    allocated = mmap(
        page_addr,
        page_size,
        PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE,
        -1,
        0
    );

    if(!allocated) {
        printf("NOT ALLOCED:%p\n", page_addr);
    }

    int allocated_correct = allocated == page_addr;
    if(!allocated_correct) {
        printf("GOT %p not %p : INCORRECT\n", allocated, page_addr);
    }

    printf("Done\n");
}