#include <stdio.h>
#include <ucontext.h>
#include <stdlib.h>

#ifndef CETHREADS_H
#define CETHREADS_H     1

#define	LF_NOERROR	    0
#define	LF_MAXTHREADS	1
#define LF_MALLOCERROR	2
#define LF_CLONEERROR	3
#define	LF_INTHREAD     4
#define LF_SIGNALERROR	5

#define MAX_THREADS 1024
#define MAX_MUTEXES 1024
#define STACK_SIZE (1024*1024)

typedef struct {
	ucontext_t thread_context;
	int active;
	int thread_id;
	int waiting_for_pid;
	void* stack_ptr; // Original stack pointer. On Mac OS X, stack_t.ss_sp is changed.
} Thread;

typedef struct {
	int is_locked;
} Mutex;

/* Should be called before executing any of the other functions. */
extern void initFibers();

extern int CEthread_create(void (*func)(void));
extern void CEthread_end();
extern void CEthread_yield();
extern void CEthread_join(int thread_id);
extern void wait_for_all_threads();

extern Mutex* CEmutex_init();
extern int CEmutex_destroy();
extern int CEmutex_lock();
extern int CEmutex_unlock();

#endif
