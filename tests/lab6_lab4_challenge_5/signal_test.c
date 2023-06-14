#include <lib.h>

#define SIGUSR1 10

sigset_t emptyset;
struct sigaction sig;
int pid, target;
int cnt;

void handler(int signo) {
	cnt++;
	debugf("%04x : handle signal #%d \t(count = %d)\n", pid, signo, cnt);
	if (cnt < 10 + (target == 0x800)) {
		kill(target, SIGUSR1);
		debugf("%04x : send SIGUSR1 to %04x\n", pid, target);
	}
}

int main(int argc, char **argv) {
	sigemptyset(&emptyset);
	sig.sa_handler = handler;
	sig.sa_mask = emptyset;
	panic_on(sigaction(SIGUSR1, &sig, NULL));
	pid = syscall_getenvid();
	target = pid ^ 0x800 ^ 0x1001;
	if (pid == 0x800) {
		debugf("%04x : init send SIGUSR1 to %04x\n", pid, target);
		kill(target, SIGUSR1);
	}
	while (cnt < 10) {
		continue;
	}
	return 0;
}
