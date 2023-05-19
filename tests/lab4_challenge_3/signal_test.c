#include <lib.h>

sigset_t set2;

int main(int argc, char **argv) {
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, 1);
    sigaddset(&set, 2);
    panic_on(sigprocmask(0, &set, NULL));
    sigdelset(&set, 2);
    int ret = fork();
    if (ret != 0) {
        panic_on(sigprocmask(0, &set2, &set));
        debugf("Father: %d.\n", sigismember(&set, 2));
    } else {
        debugf("Child: %d.\n", sigismember(&set, 2));
    }
    return 0;
}

