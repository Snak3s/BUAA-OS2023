#include <env.h>
#include <mmu.h>
#include <pmap.h>
#include <signal.h>

extern struct Env *curenv;

void sigmergeset(sigset_t *dst, const sigset_t *src) {
	for (int i = 1; i < NSIG; i++) {
		if (sigismember(src, i)) {
			sigaddset(dst, i);
		}
	}
}

void sigdiffset(sigset_t *dst, const sigset_t *src) {
	for (int i = 1; i < NSIG; i++) {
		if (sigismember(src, i)) {
			sigdelset(dst, i);
		}
	}
}

int signal_kill(void *e, int sig) {
	struct Env *env = e;
	if (is_illegal_signum(sig)) {
		return -E_NO_SIG;
	}
	if (sigismember(&env->env_sig_pending, sig)) {
		return 0;
	}
	int sig_pri = 0;
	for (int i = 0; i < NSIG; i++) {
		if (env->env_sig_queue[i] > sig_pri) {
			sig_pri = env->env_sig_queue[i];
		}
	}
	sig_pri++;
	env->env_sig_queue[sig - 1] = sig_pri;
	sigaddset(&env->env_sig_pending, sig);
	return 0;
}

int signal_pending(void *e) {
	struct Env *env = e;
	int sig_pri = NSIG + 1;
	int sig = 0;
	sigset_t set = env->env_sig_procmask;
	sigmergeset(&set, &env->env_sig_handling);
	for (int i = 1; i <= NSIG; i++) {
		if (sigismember(&env->env_sig_handling, i)) {
			sigmergeset(&env->env_sig_procmask, &env->env_sig_action[i - 1].sa_mask);
		}
	}
	sigdelset(&set, SIGKILL);
	for (int i = 1; i <= NSIG; i++) {
		if (!sigismember(&env->env_sig_pending, i)) {
			continue;
		}
		if (sigismember(&set, i)) {
			continue;
		}
		if (env->env_sig_queue[i - 1] < sig_pri) {
			sig_pri = env->env_sig_queue[i - 1];
			sig = i;
		}
	}
	return sig;
}

int signal_handle(void *e, int sig) {
	struct Env *env = e;
	if (is_illegal_signum(sig)) {
		return -E_NO_SIG;
	}
	if (!sigismember(&env->env_sig_pending, sig)) {
		return -E_NO_SIG;
	}
	if (sigismember(&env->env_sig_handling, sig)) {
		return -E_NO_SIG;
	}
	sigaddset(&env->env_sig_handling, sig);
	sigdelset(&env->env_sig_pending, sig);
	int sig_pri = env->env_sig_queue[sig - 1];
	env->env_sig_queue[sig - 1] = 0;
	for (int i = 0; i < NSIG; i++) {
		if (env->env_sig_queue[i] >= sig_pri) {
			env->env_sig_queue[i]--;
		}
	}
	return 0;
}

int signal_return(void *e, int sig) {
	struct Env *env = e;
	if (is_illegal_signum(sig)) {
		return -E_NO_SIG;
	}
	if (!sigismember(&env->env_sig_handling, sig)) {
		return -E_NO_SIG;
	}
	sigdelset(&env->env_sig_handling, sig);
	return 0;
}

void do_signal(struct Trapframe *tf) {
	struct Trapframe tmp_tf;
	int sig;
	tmp_tf = *tf;
	sig = signal_pending((void *)curenv);
	if (!sig) {
		return;
	}
	signal_handle((void *)curenv, sig);
	if (tf->regs[29] < USTACKTOP || tf->regs[29] >= UXSTACKTOP) {
		tf->regs[29] = UXSTACKTOP;
	}
	tf->regs[29] -= sizeof(struct Trapframe);
	*(struct Trapframe *)tf->regs[29] = tmp_tf;
	if (curenv->env_signal_entry) {
		tf->regs[4] = sig;
		tf->regs[5] = tf->regs[29];
		tf->regs[29] -= sizeof(tf->regs[4]) + sizeof(tf->regs[5]);
		tf->cp0_epc = curenv->env_signal_entry;
	} else {
		panic("no signal handler registered");
	}
}
