#include <lib.h>

#define N (1 << 13)

sigset_t emptyset;
struct sigaction sig;

int cnt;

void handler(int signo) {
	cnt++;
	debugf("%02d : %02d\n", cnt / 60, cnt % 60);
	alarm(1);
}

int a[N];

void print() {
	for (int i = 0; i < N; i++) {
		if (i < 10 || i >= N - 10) {
			debugf("%d%c", a[i], " \n"[i == N - 1]);
		}
		if (i == 10) {
			debugf("... ");
		}
	}
}

int main(int argc, char **argv) {
	sigemptyset(&emptyset);
	sig.sa_handler = handler;
	sig.sa_mask = emptyset;
	panic_on(sigaction(SIGALRM, &sig, NULL));
	for (int i = 0; i < N; i++) {
		a[i] = (N - i) * (N - i) % ((1 << 16) + 1) * (i + 1) % ((1 << 16) + 1);
	}
	print();
	alarm(1);
	for (int i = 1; i <= N; i++) {
		for (int j = 0; j < N - i; j++) {
			if (a[j] > a[j + 1]) {
				int tmp = a[j];
				a[j] = a[j + 1];
				a[j + 1] = tmp;
			}
		}
	}
	for (int i = 0; i < N - 1; i++) {
		panic_on(a[i] > a[i + 1]);
	}
	debugf("executed bubble sort on %d numbers\n", N);
	print();
	return 0;
}
