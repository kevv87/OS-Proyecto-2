/* On Mac OS X, _XOPEN_SOURCE must be defined before including ucontext.h.
Otherwise, getcontext/swapcontext causes memory corruption. See:

http://lists.apple.com/archives/darwin-dev/2008/Jan/msg00229.html */
#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#include "CEthreads.h"
#include <stdlib.h>
#include <ucontext.h>

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

static Thread thread_list[MAX_THREADS];
static Mutex* volatile mutexes[MAX_THREADS];
static int current_thread_idx = -1;

/* A boolean flag indicating if we are in the main process or if we are in a Thread */
static int is_thread = 0;
static int active_threads = 0;
static int active_mutexes = 0;
static int exited_threads_cnt = 0;
static ucontext_t main_execution_context;


void CEthread_yield() {
	/* If we are in a fiber, switch to the main process */
	if ( is_thread ) {
		/* Switch to the main context */
		LF_DEBUG_OUT1( "libfiber debug: Fiber %d yielding the processor...", current_thread_idx );
		swapcontext( &thread_list[current_thread_idx].thread_context, &main_execution_context );
	}
	/* Else, we are in the main process and we need to dispatch a new fiber */
	else {
		if ( active_threads == 0 ) return;
		/* Saved the state so call the next fiber */
		current_thread_idx = (current_thread_idx + 1) % active_threads;
		
		LF_DEBUG_OUT1( "Switching to fiber %d.", current_thread_idx );
		is_thread = 1;
		swapcontext( &main_execution_context, &thread_list[ current_thread_idx ].thread_context );
		is_thread = 0;
		LF_DEBUG_OUT1( "Fiber %d switched to main context.", current_thread_idx );
		
		if ( thread_list[current_thread_idx].active == 0 ) {
			LF_DEBUG_OUT1( "Fiber %d is finished. Cleaning up.\n", current_thread_idx );
			/* Free the "current" fiber's stack */
			free( thread_list[current_thread_idx].stack_ptr );
			
			/* Swap the last fiber with the current, now empty, entry */
			-- active_threads;
			if ( current_thread_idx != active_threads ) {
				thread_list[ current_thread_idx ] = thread_list[ active_threads ];
			}
			thread_list[ active_threads ].active = 0;		
		}
	}
	return;
}

static void thread_start(void (*func)(void)) {
	thread_list[current_thread_idx].active = 1;
	func();
	thread_list[current_thread_idx].active = 0;
	/* Yield control, but because active == 0, this will free the fiber */
	CEthread_end();
}

int CEthread_create(void (*func)(void)) {
	if (active_threads == MAX_THREADS) return LF_MAXTHREADS;
	/* Add the new function to the end of the Thread list */
	getcontext(&thread_list[active_threads].thread_context);

	/* Set the thread_context to a newly allocated stack_ptr */
	thread_list[active_threads].thread_context.uc_link = 0;
	thread_list[active_threads].stack_ptr = malloc(STACK_SIZE);
	thread_list[active_threads].thread_context.uc_stack.ss_sp = thread_list[active_threads].stack_ptr;
	thread_list[active_threads].thread_context.uc_stack.ss_size = STACK_SIZE;
	thread_list[active_threads].thread_context.uc_stack.ss_flags = 0;
	
	if (thread_list[active_threads].stack_ptr == 0) {
		LF_DEBUG_OUT("Error: Could not allocate stack.");
		return LF_MALLOCERROR;
	}
	
	/* Create the thread_context. The thread_context calls thread_start(func). */
	makecontext(&thread_list[active_threads].thread_context, (void (*)(void)) &thread_start, 1, func);
	thread_list[active_threads].thread_id = active_threads;
	active_threads++;
	
	return active_threads;
}

void CEthread_end() {
    Thread current_thread = thread_list[current_thread_idx];
	if(current_thread.waiting_for_pid != 0) {
		CEthread_yield();
	}

    free(current_thread.stack_ptr);
    current_thread.thread_context.uc_stack.ss_sp = NULL;
	current_thread.thread_context.uc_stack.ss_size = 0;
	current_thread.thread_context.uc_stack.ss_flags = 0;
    current_thread.thread_context.uc_link = NULL;
    for (int i=(current_thread_idx+1); i<active_threads; i++) {
        thread_list[i-1] = thread_list[i];
    }
    active_threads--;
	broadcast_to_waiters(current_thread.thread_id);
}

int CEthread_join(int thread_id) {
    Thread current_thread = thread_list[current_thread_idx];
	current_thread.waiting_for_pid = thread_id;
}

void broadcast_to_waiters(int thread_id) {
	for(int i=0; i<MAX_THREADS; i++) {
		if(thread_list[i].waiting_for_pid == thread_id)
			thread_list[i].waiting_for_pid = 0;
	}
}

int CEmutex_init() {
	if (active_mutexes > MAX_MUTEXES) {
		fprintf(stderr, "Mutex number out of range.\n");
		return -1;
	}

	Mutex *mutex = malloc(sizeof(Mutex));
	mutex->is_locked = 0;
	mutexes[active_mutexes] = mutex;
	active_mutexes++;
	return mutex;
}

int CEmutex_lock(Mutex *mutex) {
	while(mutex->is_locked) { }
	mutex->is_locked = 1;
  	return 0;
}

int CEmutex_unlock(Mutex *mutex) {
	mutex->is_locked = 0;
	return 0;
}

int CEmutex_destroy(Mutex *mutex) {
	for(int i=0; i<MAX_MUTEXES; i++) {
		if(mutexes[i] == mutex) {
			mutexes[i] = NULL;
		}
	}
	free(mutex);
}