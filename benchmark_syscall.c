#include <inttypes.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <seccomp.h>
#include <errno.h>
#include <linux/audit.h>
#include <linux/bpf.h>
#include <linux/filter.h>
#include <linux/seccomp.h>
#include <linux/unistd.h>
#include <sys/prctl.h>

#define STRINGIFY(x) STRINGIFY2(x)
#define STRINGIFY2(x) #x
#define release_assert(test) if (!(test)) { printf("Assert failed: %s:%d, %s\n", __FILE__, __LINE__, STRINGIFY(test)); fflush(stdout); abort(); }

int install_seccompbpf_filter() {
    int nr = __NR_write;
    int arch = AUDIT_ARCH_X86_64;
    int error = EPERM;

    // disallow a certain syscall
    struct sock_filter filter[] = {
        BPF_STMT(BPF_LD + BPF_W + BPF_ABS, (offsetof(struct seccomp_data, arch))),
        BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, arch, 0, 3),
        BPF_STMT(BPF_LD + BPF_W + BPF_ABS, (offsetof(struct seccomp_data, nr))),
        BPF_JUMP(BPF_JMP + BPF_JEQ + BPF_K, nr, 0, 1),
        BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_ERRNO | (error & SECCOMP_RET_DATA)),
        BPF_STMT(BPF_RET + BPF_K, SECCOMP_RET_ALLOW),
    };
    struct sock_fprog prog = {
        .len = (unsigned short)(sizeof(filter) / sizeof(filter[0])),
        .filter = filter,
    };
    if (prctl(PR_SET_NO_NEW_PRIVS, 1, 0, 0, 0)) {
        perror("prctl(NO_NEW_PRIVS)");
        return 1;
    }
    if (prctl(PR_SET_SECCOMP, 2, &prog)) {
        perror("prctl(PR_SET_SECCOMP)");
        return 1;
    }
    return 0;
}

int install_seccomp() {
    scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ERRNO(5));
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(open), 0)) {
        perror("seccomp_rule_add: open");
        return 1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(openat), 0)) {
        perror("seccomp_rule_add: open");
        return 1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0)) {
        perror("seccomp_rule_add: close");
        return 1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(read), 0)) {
        perror("seccomp_rule_add: read");
        return 1;
    }
    if (seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0)) {
        perror("seccomp_rule_add: exit");
        return 1;
    }
    if (seccomp_load(ctx)) {
        perror("seccomp_load");
        return 1;
    }
    return 0;
}

int main(int argc, char** argv) {
#ifdef SECCOMP_FILTER
    release_assert(install_seccomp() == 0);
#elif defined SECCOMP_BPF_FILTER
    release_assert(install_seccompbpf_filter() == 0);
#endif

    for (int i = 0; i < 100000; i++) {
        FILE* f = fopen("./data.txt", "r");
        release_assert(f);

        char * line = NULL;
        size_t len = 0;
        ssize_t read;

        release_assert((read = getline(&line, &len, f)) != -1);
        release_assert(strcmp(line, "Hello") == 0);

        if (line) {
            free(line);
        }

        fclose(f);
    }
}

// void install_seccomp() {
//     scmp_filter_ctx ctx = seccomp_init(SCMP_ACT_ERRNO(5));
//     release_assert(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(close), 0) == 0);
//     release_assert(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(dup), 0) == 0);
//     release_assert(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(write), 0) == 0);
//     release_assert(seccomp_rule_add(ctx, SCMP_ACT_ALLOW, SCMP_SYS(exit), 0) == 0);
//     release_assert(seccomp_load(ctx) == 0);
// }

// int main(int argc, char** argv) {
//     install_seccomp();

//     for (int i = 0; i < 10000; i++) {
//         FILE* f = fopen("/tmp/abc.txt", "w+");
//         release_assert(f);
//         fputs("Hello\n", f);
//         fclose(f);
//     }
// }
