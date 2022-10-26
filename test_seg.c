#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

uint64_t testMov(uint64_t*);
uint64_t* getTls();
void writeTLSVar(uint64_t data, int64_t var_loc, uint64_t offset);
void writeTLSVarAddOne(uint64_t data, int64_t var_loc, uint64_t offset);
void setGsBase(void* ptr);
void* getGsBase();

_Thread_local uint64_t tlsvar = 45;

#define RELEASE_ASSERT(cond, msg)         \
    if (!(cond)) {                        \
        printf("ASSERT FAILED" msg "\n"); \
        abort();                          \
    }

int main(int argc, char const *argv[])
{
    uint64_t test = 34;
    uint64_t val = testMov(&test);
    printf("Hello world: %" PRIu64 "\n", val);

    uint64_t* tls = getTls();
    int64_t varloc = ((char*)&tlsvar) - ((char*)tls);
    printf("TLS: %p, &tlsvar: %p, varloc: %" PRId64 "\n", tls, &tlsvar, varloc);

    printf("Tlsvar val: %" PRId64 "\n", tlsvar);


    printf("Running writeTLSVar\n");
    writeTLSVar(test++, varloc, 0);
    printf("Tlsvar val: %" PRId64 "\n", tlsvar);

    printf("Running writeTLSVarAddOne\n");
    writeTLSVarAddOne(test++, varloc - 1, 0);
    printf("Tlsvar val: %" PRId64 "\n", tlsvar);

    size_t buffer_size = 1024 * 1024;
    char* gs_buffer = (char*) malloc(buffer_size);

    printf("Running setGsBase. Curr Gs base: %p\n", getGsBase());
    setGsBase(gs_buffer);
    printf("Curr Gs base: %p\n", getGsBase());

    free(gs_buffer);

    return 0;
}
