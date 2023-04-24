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

	/*
	if (env->env_ipc_broadcast & IPC_BROADCAST) {
		u_int va = dstva;
		if (!(env->env_ipc_broadcast & IPC_PAGE_SENT)) {
			va = 0;
		}
		ipc_broadcast(env->env_ipc_value, va, env->env_ipc_perm);
	}
	*/

	return env->env_ipc_value;
}

void ipc_broadcast(u_int val, void *srcva, u_int perm) {
	int sent[NENV];
	for (int i = 0; i < NENV; i++) {
		sent[i] = 0;
	}
	sent[ENVX(env->env_id)] = env->env_id;
	int cnt = 1;
	while (cnt) {
		cnt = 0;
		for (int i = 0; i < NENV; i++) {
			if (envs[i].env_status != ENV_FREE &&
			    sent[ENVX(envs[i].env_parent_id)] == envs[i].env_parent_id) {
				cnt++;
				sent[ENVX(envs[i].env_id)] = envs[i].env_id;
				ipc_send(envs[i].env_id, val, srcva, perm | IPC_BROADCAST);
			}
		}
	}
}
