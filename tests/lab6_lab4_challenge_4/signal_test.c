#include <lib.h>

sigset_t set, emptyset;
struct sigaction sig;

void handler(int signo) {
	static int cnt = 0;
	cnt++;
	debugf("#%d : handle signal #%d\n", cnt, signo);
}

int main(int argc, char **argv) {
	int n = 8;
	sigemptyset(&set);
	sigemptyset(&emptyset);
	for (int i = 1; i <= n; i++) {
		sigaddset(&set, i);
	}
	sig.sa_handler = handler;
	sig.sa_mask = set;
	for (int i = 1; i <= n; i++) {
		panic_on(sigaction(i, &sig, NULL));
	}
	panic_on(sigprocmask(SIG_BLOCK, &set, NULL));
	for (int i = 1; i <= n; i++) {
		kill(0, i);
	}
	panic_on(sigprocmask(SIG_UNBLOCK, &set, NULL));
	for (int i = 1; i <= 10000000; i++) {
		continue;
	}
	return 0;
}
