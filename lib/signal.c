#include <signal.h>
#include <string.h>

void sigemptyset(sigset_t *set) {
	memset(set, 0, sizeof(set));
}

void sigfillset(sigset_t *set) {
	memset(set, 0xFF, sizeof(set));
}

void sigaddset(sigset_t *set, int signum) {
	set->sig[(signum - 1) / 32] |= 1 << ((signum - 1) % 32);
}

void sigdelset(sigset_t *set, int signum) {
	set->sig[(signum - 1) / 32] &= ~(1 << ((signum - 1) % 32));
}

int sigismember(const sigset_t *set, int signum) {
	return (set->sig[(signum - 1) / 32] >> ((signum - 1) % 32)) & 1;
}
