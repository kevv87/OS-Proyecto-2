#include <stdio.h>

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

/* Should be called before executing any of the other functions. */
extern void initFibers();

extern int CEthread_create(void (*func)(void));
extern void CEthread_end();
extern void CEthread_yield();
extern int CEthread_join(int thread_id);

extern int CEmutex_init();
extern int CEmutex_destroy();
extern int CEmutex_lock();
extern int CEmutex_unlock();

#endif
