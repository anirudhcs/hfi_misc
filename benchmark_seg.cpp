#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>

using namespace std::chrono;

#define RELEASE_ASSERT(cond, msg)         \
    if (!(cond)) {                        \
        printf("ASSERT FAILED" msg "\n"); \
        abort();                          \
    }

#define SCALE 2
#define OFFSET -10
const int64_t INDEX = 5;

template<typename T_DataSize>
void write_to_gs_buffer(size_t buffer_size) {
    T_DataSize counter = 2;
    for (size_t base = 0; base < buffer_size; base+=sizeof(T_DataSize)) {
        asm(
            "mov %0, %%gs:-10(%1, %2, 2)\n"
            : /* writes */
            : /* reads */    "r"(counter), "r"(base), "r"(INDEX)
            : /* clobbers */ "memory"
        );
        counter++;
    }
}

template<typename T_DataSize>
void write_to_direct_buffer(size_t buffer_size, uint8_t* direct_buffer) {
    T_DataSize counter = 2;
    uint8_t* base_lim = direct_buffer + buffer_size;
    for (uint8_t* base = direct_buffer; base < base_lim; base+=sizeof(T_DataSize)) {
        asm(
            "mov %0, -10(%1, %2, 2)\n"
            : /* writes */
            : /* reads */    "r"(counter), "r"(base), "r"(INDEX)
            : /* clobbers */ "memory"
        );
        counter++;
    }
}

template<typename T_DataSize>
void runBenchmark(uint64_t test_iterations, bool print, size_t buffer_size, uint8_t* direct_buffer, uint8_t* gs_buffer)
{
    {
        auto enter_time = high_resolution_clock::now();
        for (uint64_t i = 0; i < test_iterations; i++) {
            write_to_direct_buffer<T_DataSize>(buffer_size, direct_buffer);
        }
        auto exit_time = high_resolution_clock::now();
        int64_t ns = duration_cast<nanoseconds>(exit_time - enter_time).count();
        if(print) {
            printf("direct write time: %lld\n", (long long) (ns));
        }
    }

    {
        auto enter_time = high_resolution_clock::now();
        for (uint64_t i = 0; i < test_iterations; i++) {
            write_to_gs_buffer<T_DataSize>(buffer_size);
        }
        auto exit_time = high_resolution_clock::now();
        int64_t ns = duration_cast<nanoseconds>(exit_time - enter_time).count();
        if(print) {
            printf("gs write time: %lld\n", (long long) (ns));
        }
    }

    int same = memcmp(direct_buffer, gs_buffer, buffer_size);
    RELEASE_ASSERT(same == 0, "buffers not the same");
}

int main(int argc, char const *argv[])
{
    int64_t diff = (INDEX * SCALE) + (OFFSET);
    RELEASE_ASSERT(diff == 0, "diff not 0");

    size_t buffer_size = 32 * 1024 * 1024;
    uint8_t* direct_buffer = (uint8_t*) malloc(buffer_size);
    uint8_t* gs_buffer = (uint8_t*) malloc(buffer_size);

    const uint64_t test_iterations = 10;

    for(uint64_t i = 0; i < test_iterations; i++) {
        (void) high_resolution_clock::now();
    }

    asm volatile("wrgsbase %0\n"
        : /* writes */
        : /* reads */  "r" (gs_buffer)
    );

    // WARMUP first, real next
    printf("Size: uint8_t\n");
    runBenchmark<uint8_t>(test_iterations, false /*no print*/, buffer_size, direct_buffer, gs_buffer);
    runBenchmark<uint8_t>(test_iterations, true /* print*/, buffer_size, direct_buffer, gs_buffer);
    printf("-----------\n");
    // WARMUP first, real next
    printf("Size: uint16_t\n");
    runBenchmark<uint16_t>(test_iterations, false /*no print*/, buffer_size, direct_buffer, gs_buffer);
    runBenchmark<uint16_t>(test_iterations, true /* print*/, buffer_size, direct_buffer, gs_buffer);
    printf("-----------\n");
    // WARMUP first, real next
    printf("Size: uint32_t\n");
    runBenchmark<uint32_t>(test_iterations, false /*no print*/, buffer_size, direct_buffer, gs_buffer);
    runBenchmark<uint32_t>(test_iterations, true /* print*/, buffer_size, direct_buffer, gs_buffer);
    printf("-----------\n");
    // WARMUP first, real next
    printf("Size: uint64_t\n");
    runBenchmark<uint64_t>(test_iterations, false /*no print*/, buffer_size, direct_buffer, gs_buffer);
    runBenchmark<uint64_t>(test_iterations, true /* print*/, buffer_size, direct_buffer, gs_buffer);
    printf("-----------\n");


    free(gs_buffer);
    free(direct_buffer);

    printf("Done!\n");
    return 0;
}
