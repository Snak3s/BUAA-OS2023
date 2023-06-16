#include <lib.h>

sigset_t emptyset;
struct sigaction sig;

int cnt;

void handler(int signo) {
	cnt++;
	printf("%02d : %02d\n", cnt / 60, cnt % 60);
	alarm(1);
}

int main(int argc, char **argv) {
	sigemptyset(&emptyset);
	sig.sa_handler = handler;
	sig.sa_mask = emptyset;
	panic_on(sigaction(SIGALRM, &sig, NULL));
	alarm(1);
	while (cnt < 10) {
		continue;
	}
	return 0;
}
