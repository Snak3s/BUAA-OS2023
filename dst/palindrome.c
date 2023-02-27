#include <stdio.h>

int is_palindrome(int n) {
	int buffer[16];
	int len = 0;
	while (n) {
		buffer[len++] = n % 10;
		n /= 10;
	}
	for (int i = 0; i < len; i++) {
		if (buffer[i] != buffer[len - i - 1]) {
			return 0;
		}
	}
	return 1;
}

int main() {
	int n;
	scanf("%d", &n);

	if (is_palindrome(n)) {
		printf("Y\n");
	} else {
		printf("N\n");
	}
	return 0;
}
