#ifndef SIGNAL_H
#define SIGNAL_H

#include <types.h>

#define NSIG 64

struct sigset_t {
	int sig[NSIG / 32];
};
typedef struct sigset_t sigset_t;

struct sigaction {
	void (*sa_handler)(int);
	sigset_t sa_mask;
};
typedef struct sigaction sigaction_t;

#define SIG_BLOCK 0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

#define SIGKILL 9
#define SIGSEGV 11
#define SIGALRM 14
#define SIGTERM 15

#define is_illegal_signum(signum) ((signum) <= 0 || (signum) > NSIG)

void sigemptyset(sigset_t *set);
void sigfillset(sigset_t *set);
void sigaddset(sigset_t *set, int signum);
void sigdelset(sigset_t *set, int signum);
int sigismember(const sigset_t *set, int signum);

void sigmergeset(sigset_t *dst, const sigset_t *src);
void sigdiffset(sigset_t *dst, const sigset_t *src);

int signal_kill(void *e, int sig);
int signal_pending(void *e);
int signal_handle(void *e, int sig);
int signal_return(void *e, int sig);

void do_signal();

u_int gettime(u_int *usec);

#endif
