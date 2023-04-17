#include <env.h>
#include <pmap.h>
#include <printk.h>

/* Overview:
 *   Implement a round-robin scheduling to select a runnable env and schedule it using 'env_run'.
 *
 * Post-Condition:
 *   If 'yield' is set (non-zero), 'curenv' should not be scheduled again unless it is the only
 *   runnable env.
 *
 * Hints:
 *   1. The variable 'count' used for counting slices should be defined as 'static'.
 *   2. Use variable 'env_sched_list', which contains and only contains all runnable envs.
 *   3. You shouldn't use any 'return' statement because this function is 'noreturn'.
 */
void schedule(int yield) {
	static int count = 0; // remaining time slices of current env
	struct Env *e = curenv;
	static int user_time[5];

	/* We always decrease the 'count' by 1.
	 *
	 * If 'yield' is set, or 'count' has been decreased to 0, or 'e' (previous 'curenv') is
	 * 'NULL', or 'e' is not runnable, then we pick up a new env from 'env_sched_list' (list of
	 * all runnable envs), set 'count' to its priority, and schedule it with 'env_run'. **Panic
	 * if that list is empty**.
	 *
	 * (Note that if 'e' is still a runnable env, we should move it to the tail of
	 * 'env_sched_list' before picking up another env from its head, or we will schedule the
	 * head env repeatedly.)
	 *
	 * Otherwise, we simply schedule 'e' again.
	 *
	 * You may want to use macros below:
	 *   'TAILQ_FIRST', 'TAILQ_REMOVE', 'TAILQ_INSERT_TAIL'
	 */
	/* Exercise 3.12: Your code here. */
	count--;
	if (yield || count == 0 || e == NULL || e->env_status == ENV_NOT_RUNNABLE) {
		do {
			panic_on(TAILQ_EMPTY(&env_sched_list));
			// e = TAILQ_FIRST(&env_sched_list);
			u_int user_env_cnt[5];
			for (int i = 0; i < 5; i++) {
				user_env_cnt[i] = 0;
			}
			struct Env *env;
			TAILQ_FOREACH(env, &env_sched_list, env_sched_link) {
				user_env_cnt[env->env_user]++;
			}
			int user = -1;
			for (int i = 0; i < 5; i++) {
				if (user_env_cnt[i] == 0) {
					continue;
				}
				if (user == -1 || user_time[i] < user_time[user]) {
					user = i;
				}
			}
			panic_on(user == -1);
			TAILQ_FOREACH(env, &env_sched_list, env_sched_link) {
				if (env->env_user == user) {
					e = env;
					break;
				}
			}
			TAILQ_REMOVE(&env_sched_list, (e), env_sched_link);
			if (e->env_status == ENV_RUNNABLE) {
				count = e->env_pri;
				user_time[e->env_user] += e->env_pri;
				TAILQ_INSERT_TAIL(&env_sched_list, (e), env_sched_link);
			}
		} while (count == 0 || e == NULL || e->env_status == ENV_NOT_RUNNABLE);
	}
	env_run(e);
}
