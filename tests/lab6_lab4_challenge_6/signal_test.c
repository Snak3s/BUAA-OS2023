#include <lib.h>

sigset_t set, emptyset;
struct sigaction sig;

int n = 8;

char buf[1024];
char *p = buf;

void puts(char *s) {
	while (*s) {
		*p++ = *s++;
	}
}

char num[2];
void handler(int signo) {
	puts("start handle signal ");
	num[0] = signo + '0';
	puts(num);
	puts("\n");

	if (signo < n) {
		puts("kill signal ");
		num[0] = signo + 1 + '0';
		puts(num);
		puts("...\n");
		kill(0, signo + 1);
	}

	puts("end handle signal ");
	num[0] = signo + '0';
	puts(num);
	puts("\n");
}

int main(int argc, char **argv) {
	sigemptyset(&set);
	sigemptyset(&emptyset);
	for (int i = 1; i <= n; i++) {
		sigaddset(&set, i);
	}
	sig.sa_handler = handler;
	sig.sa_mask = emptyset;
	for (int i = 1; i <= n; i++) {
		panic_on(sigaction(i, &sig, NULL));
	}
	debugf("i am main\n");
	debugf("kill signal 1\n");
	kill(0, 1);
	for (int i = 1; i <= 10000000; i++) {
		continue;
	}
	debugf("%s", buf);
	debugf("main reaches end\n");
	return 0;
}
