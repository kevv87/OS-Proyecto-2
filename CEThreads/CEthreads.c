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
	void* stack_ptr; // Original stack pointer. On Mac OS X, stack_t.ss_sp is changed.
} thread;

static thread thread_list[MAX_THREADS];
static int current_thread_idx = -1;
thread waiting_threads[MAX_THREADS];
int exited_threads[MAX_THREADS];

/* A boolean flag indicating if we are in the main process or if we are in a thread */
static int is_thread = 0;
static int active_threads = 0;
static int exited_threads_cnt = 0;

static ucontext_t main_execution_context;


static void thread_start(void (*func)(void)) {
	thread_list[current_thread_idx].active = 1;
	func();
	thread_list[current_thread_idx].active = 0;
	/* Yield control, but because active == 0, this will free the fiber */
	fiberYield();
}

int CEthread_create(void (*func)(void)) {
	if (active_threads == MAX_THREADS) return LF_MAXTHREADS;
	/* Add the new function to the end of the thread list */
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
	active_threads++;
	
	return active_threads;
}

void CEthread_end() {
    thread current_thread = thread_list[current_thread_idx];
    free(current_thread.stack_ptr);
    current_thread.thread_context.uc_stack.ss_sp = NULL;
	current_thread.thread_context.uc_stack.ss_size = 0;
	current_thread.thread_context.uc_stack.ss_flags = 0;
    current_thread.thread_context.uc_link = NULL;
    for (int i=(current_thread_idx+1); i<active_threads; i++) {
        thread_list[i-1] = thread_list[i];
    }
    active_threads--;
}

int CEthread_join(int thread_id) {
    int dont_join = 0;

	for (int i=0; i<exited_threads_cnt; i++)
		if(thread_id == exited_threads[i])
			dont_join = 1;

	if(dont_join != 1) {

	}
}
