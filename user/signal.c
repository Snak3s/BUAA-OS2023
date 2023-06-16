#include <lib.h>

sigset_t emptyset;
struct sigaction sig;

int n, v[64];
char **s;
int cnt;

char buf[1024];
char *p = buf;

void puts(char *s) {
	while (*s) {
		*p++ = *s++;
	}
}

void handler(int signo) {
	int cur = ++cnt;
	puts("start handle signal ");
	puts(s[cur]);
	puts("\n");

	if (cur < n) {
		puts("kill signal ");
		puts(s[cur + 1]);
		puts("\n");
		kill(0, v[cur + 1]);
	}

	puts("end handle signal ");
	puts(s[cur]);
	puts("\n");
}

int main(int argc, char **argv) {
	n = argc - 1;
	s = argv;
	for (int i = 1; i <= n; i++) {
		for (int j = 0; s[i][j]; j++) {
			v[i] = v[i] * 10 + s[i][j] - '0';
		}
	}
	sigemptyset(&emptyset);
	sig.sa_handler = handler;
	sig.sa_mask = emptyset;
	for (int i = 1; i <= 64; i++) {
		panic_on(sigaction(i, &sig, NULL));
	}
	printf("i am main\n");
	printf("kill signal %d\n", v[1]);
	kill(0, v[1]);
	for (int i = 1; i <= 10000000; i++) {
		continue;
	}
	printf("%s", buf);
	printf("main reaches end\n");
	return 0;
}
