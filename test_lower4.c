#include <stdio.h>
#include <stdint.h>
#include <sys/mman.h>

int main(int argc, char** argv) {

    const uint64_t four_gb = ((uint64_t)4) * 1024 * 1024 * 1024;
    const uint64_t page_size = 4 * 1024;

    for(uint64_t page_addr_loc = page_size; page_addr_loc < four_gb; page_addr_loc+= page_size) {
        void* allocated = 0;
        void* page_addr = (void*)(uintptr_t)page_addr_loc;

        for (int retry = 0; retry < 10; retry++) {
            allocated = mmap(
                page_addr,
                page_size,
                PROT_READ | PROT_WRITE,
                MAP_ANONYMOUS | MAP_PRIVATE | MAP_FIXED_NOREPLACE,
                -1,
                0
            );
            if (allocated) {
                break;
            }
        }

        int allocated_correct = allocated == page_addr;

        if(!allocated || !allocated_correct) {
            printf("%p : FAILED\n", page_addr);
        }

    }
        printf("Done\n");
}