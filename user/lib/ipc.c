// User-level IPC library routines

#include <env.h>
#include <lib.h>
#include <mmu.h>

// Send val to whom.  This function keeps trying until
// it succeeds.  It should panic() on any error other than
// -E_IPC_NOT_RECV.
//
// Hint: use syscall_yield() to be CPU-friendly.
void ipc_send(u_int whom, u_int val, const void *srcva, u_int perm) {
	int r;
	while ((r = syscall_ipc_try_send(whom, val, srcva, perm)) == -E_IPC_NOT_RECV) {
		syscall_yield();
	}
	user_assert(r == 0);
}

// Receive a value.  Return the value and store the caller's envid
// in *whom.
//
// Hint: use env to discover the value and who sent it.
u_int ipc_recv(u_int *whom, void *dstva, u_int *perm) {
	int r = syscall_ipc_recv(dstva);
	if (r != 0) {
		user_panic("syscall_ipc_recv err: %d", r);
	}

	if (whom) {
		*whom = env->env_ipc_from;
	}

	if (perm) {
		*perm = env->env_ipc_perm;
	}

	return env->env_ipc_value;
}

// for lab 5-1 exam

#define DEV_RTC_ADDRESS 0x15000000

#define DEV_RTC_TRIGGER_READ 0x0000
#define DEV_RTC_SEC 0x0010
#define DEV_RTC_USEC 0x0020

u_int get_time(u_int *us) {
	u_int s = 0;
	syscall_write_dev(&s, DEV_RTC_ADDRESS + DEV_RTC_TRIGGER_READ, 4);
	syscall_read_dev(&s, DEV_RTC_ADDRESS + DEV_RTC_SEC, 4);
	syscall_read_dev(us, DEV_RTC_ADDRESS + DEV_RTC_USEC, 4);
	return s;
}

void usleep(u_int us) {
	u_int start, end;
	get_time(&start);
	while (1) {
		get_time(&end);
		if (end - start >= us) {
			break;
		} else {
			syscall_yield();
		}
	}
}
