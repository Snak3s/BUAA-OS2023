#include <drivers/dev_rtc.h>
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

u_int gettime(u_int *usec) {
	*(u_int *)(KSEG1 | DEV_RTC_ADDRESS | DEV_RTC_TRIGGER_READ) = 0;
	u_int sec = *(u_int *)(KSEG1 | DEV_RTC_ADDRESS | DEV_RTC_SEC);
	if (usec != NULL) {
	       *usec = *(u_int *)(KSEG1 | DEV_RTC_ADDRESS | DEV_RTC_USEC);
	}
	return sec;
}

int signal_kill(void *e, int sig) {
	struct Env *env = e;
	if (is_illegal_signum(sig)) {
		return -E_NO_SIG;
	}
	if (env->env_sig_queue_len >= NSIG) {
		return -E_NO_SIG;
	}
	env->env_sig_queue[env->env_sig_queue_len++] = sig;
	return 0;
}

int signal_pending(void *e) {
	struct Env *env = e;
	if (env->env_sig_alarm_seconds) {
		u_int usec;
		u_int sec = gettime(&usec);
		u_int duration = (sec - env->env_sig_alarm_start_sec) - (usec >= env->env_sig_alarm_start_usec ? 0 : 1);
		if (duration >= env->env_sig_alarm_seconds) {
			int r = signal_kill(e, SIGALRM);
			if (r == 0) {
				env->env_sig_alarm_seconds = 0;
			}
		}
	}
	if (env->env_sig_queue_len == 0) {
		return 0;
	}
	sigset_t set = env->env_sig_procmask;
	for (int i = 1; i <= NSIG; i++) {
		if (sigismember(&env->env_sig_handling, i)) {
			sigmergeset(&set, &env->env_sig_action[i - 1].sa_mask);
		}
	}
	sigdelset(&set, SIGKILL);
	for (int i = env->env_sig_queue_len - 1; i >= 0; i--) {
		if (sigismember(&set, env->env_sig_queue[i])) {
			continue;
		}
		return env->env_sig_queue[i];
	}
	return 0;
}

int signal_handle(void *e, int sig) {
	struct Env *env = e;
	if (is_illegal_signum(sig)) {
		return -E_NO_SIG;
	}
	int sig_pri = env->env_sig_queue_len - 1;
	while (sig_pri >= 0 && env->env_sig_queue[sig_pri] != sig) {
		sig_pri--;
	}
	if (sig_pri < 0) {
		return -E_NO_SIG;
	}
	for (int i = sig_pri; i < NSIG - 1; i++) {
		env->env_sig_queue[i] = env->env_sig_queue[i + 1];
	}
	env->env_sig_queue[NSIG - 1] = 0;
	env->env_sig_queue_len--;
	if (env->env_sig_cnt[sig - 1] == 0) {
		sigaddset(&env->env_sig_handling, sig);
	}
	env->env_sig_cnt[sig - 1]++;
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
	env->env_sig_cnt[sig - 1]--;
	if (env->env_sig_cnt[sig - 1] == 0) {
		sigdelset(&env->env_sig_handling, sig);
	}
	return 0;
}

void do_signal(struct Trapframe *tf) {
	struct Trapframe tmp_tf;
	int sig;
	if (tf->cp0_epc >= ULIM) {
		return;
	}
	if (curenv->env_signal_entry == 0) {
		return;
	}
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
	tf->regs[4] = sig;
	tf->regs[5] = tf->regs[29];
	tf->regs[29] -= sizeof(tf->regs[4]) + sizeof(tf->regs[5]);
	tf->cp0_epc = curenv->env_signal_entry;
}
