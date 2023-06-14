#include <env.h>
#include <lib.h>
#include <mmu.h>
#include <signal.h>

// syscall operations

int sigaction(int signum, const struct sigaction *act, struct sigaction *oldact) {
	if (oldact != NULL) {
		memset(oldact, 0, sizeof(struct sigaction));
	}
	return syscall_sigaction(signum, act, oldact);
}

int sigprocmask(int how, const sigset_t *set, sigset_t *oldset) {
	if (oldset != NULL) {
	       memset(oldset, 0, sizeof(sigset_t));
	}
	return syscall_sigprocmask(how, set, oldset);
}

int kill(u_int envid, int sig) {
	return syscall_kill(envid, sig);
}

u_int alarm(u_int seconds) {
	return syscall_alarm(seconds);
}

// callback entry

void sig_handler_ignore(int signum) {}

void __attribute__((noreturn)) sig_handler_kill(int signum) {
	exit();
}

void (*sig_default_handlers[NSIG])(int) = {
	[0 ... NSIG - 1] = sig_handler_ignore,
	[SIGKILL] = sig_handler_kill,
	[SIGSEGV] = sig_handler_kill,
	[SIGTERM] = sig_handler_kill,
};

static void __attribute__((noreturn)) signal_entry(int signum, struct Trapframe *tf) {
	void (*sig_handler)(int);
	if (env->env_sig_action[signum - 1].sa_handler != NULL) {
		sig_handler = env->env_sig_action[signum - 1].sa_handler;
	} else {
		sig_handler = sig_default_handlers[signum - 1];
	}
	sig_handler(signum);
	syscall_sigreturn(signum);
	int r = syscall_set_trapframe(0, tf);
	user_panic("syscall_set_trapframe returned %d", r);
}

void siginit() {
	syscall_set_signal_entry(signal_entry);
}
