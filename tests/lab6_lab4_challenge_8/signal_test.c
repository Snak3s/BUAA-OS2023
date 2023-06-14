#include <lib.h>

#define N (1 << 13)

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
	for (int i = 0; i < N; i++) {
		a[i] = (N - i) * (N - i) % ((1 << 16) + 1) * (i + 1) % ((1 << 16) + 1);
	}
	print();
	alarm(20);
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
	debugf("%d seconds remains\n", alarm(0));
	return 0;
}
