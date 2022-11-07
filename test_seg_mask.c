/* Type your code here, or load an example. */
#include <stdint.h>
#include <stdio.h>
#include <stdio.h>
#include <inttypes.h>

uint64_t test(uint64_t base, uint32_t offset1, uint32_t offset2) {
    uint64_t ret;
    asm volatile(
        "wrgsbase %1\n"
        "mov %%gs:(%2, %3), %0\n"
        : "=r"(ret)
        : "r"(base), "r"(offset1), "r"(offset2)
        : "memory"
    );
    return ret;
}

int main(int argc, char** argv){
    uint64_t buff[10] = { 11,12,13,14,15,16,17,18,19,20 };
    uint32_t a = UINT32_MAX;
    uint32_t b = 9;
    uint64_t ret = test((uintptr_t) buff, a, b);
    printf("Ret: %" PRIu64 "\n", ret);
    return 0;
}