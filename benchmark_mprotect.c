#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

#include "wasm-rt.h"

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define release_assert(test) if (!(test)) { printf("Assert failed: %s:%d, %s\n", __FILE__, __LINE__, STRINGIFY(test)); fflush(stdout); abort(); }

int main(int argc, char** argv) {

    wasm_rt_memory_t memory;
    const uint32_t initial_pages = 10;
    const uint32_t max_pages = 65536;
    const bool success = wasm_rt_allocate_memory(&memory, initial_pages, max_pages);
    release_assert(success);

    for (uint64_t current_pages = initial_pages; current_pages < max_pages; current_pages++) {
        uint32_t old_pages = wasm_rt_grow_memory(&memory, 1);
        // printf("page %" PRIu64 "\n", current_pages);
        release_assert(old_pages == current_pages);
    }

    return 0;
}