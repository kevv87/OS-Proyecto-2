/* On Mac OS X, _XOPEN_SOURCE must be defined before including ucontext.h.
Otherwise, getcontext/swapcontext causes memory corruption. See:

http://lists.apple.com/archives/darwin-dev/2008/Jan/msg00229.html */
#ifdef __APPLE__
#define _XOPEN_SOURCE
#endif

#include "CEthreads.h"

static Thread* thread_list[MAX_THREADS];
static Mutex* volatile mutexes[MAX_THREADS];
static int current_thread_idx = -1;

/* A boolean flag indicating if we are in the main process or if we are in a Thread */
static int is_thread = 0;
static int active_threads = 0;
static int active_mutexes = 0;
static ucontext_t main_execution_context;

static void thread_start(void (*func)(void)) {
	thread_list[current_thread_idx]->active = 1;
	func();
	thread_list[current_thread_idx]->active = 0;
	CEthread_yield();
}

int CEthread_create(void (*func)(void)) {
	if (active_threads == MAX_THREADS) return LF_MAXTHREADS;

	Thread *thread = malloc(sizeof(Thread));
	thread_list[active_threads] = thread;
	/* Add the new function to the end of the Thread list */
	getcontext(&thread->thread_context);

	/* Set the thread_context to a newly allocated stack_ptr */
	thread->thread_context.uc_link = 0;
	thread->stack_ptr = malloc(STACK_SIZE);
	thread->thread_context.uc_stack.ss_sp = thread->stack_ptr;
	thread->thread_context.uc_stack.ss_size = STACK_SIZE;
	thread->thread_context.uc_stack.ss_flags = 0;
	
	if (thread->stack_ptr == 0) {
		return LF_MALLOCERROR;
	}
	
	/* Create the thread_context. The thread_context calls thread_start(func). */
	makecontext(&thread->thread_context, (void (*)(void)) &thread_start, 1, func);
	active_threads++;
	thread->thread_id = active_threads;
	
	return active_threads;
}

void broadcast_to_waiters(int thread_id) {
	for(int i=0; i<active_threads; i++) {
		if(thread_list[i]->waiting_for_pid == thread_id)
			thread_list[i]->waiting_for_pid = 0;
	}
}

void CEthread_yield() {
	/* If we are in a thread, switch to the main process */
	if (is_thread) {
		swapcontext( &thread_list[current_thread_idx]->thread_context, &main_execution_context );
	}
	/* Else, we are in the main process and we need to dispatch a new thread */
	else {
		if ( active_threads == 0 ) return;

		/* Saved the state so call the next thread */
		current_thread_idx = (current_thread_idx + 1) % active_threads;
		while(thread_list[current_thread_idx]->waiting_for_pid) {			// Search for threads that are not waiting
			current_thread_idx = (current_thread_idx + 1) % active_threads;
		}
		
		is_thread = 1;
		swapcontext( &main_execution_context, &thread_list[ current_thread_idx ]->thread_context );
		is_thread = 0;
		
		if (thread_list[current_thread_idx]->active==0 && thread_list[current_thread_idx]->waiting_for_pid==0) {
			CEthread_end();	
		}
	}
	return;
}

void CEthread_end() {
    Thread *current_thread = thread_list[current_thread_idx];
    free(current_thread->stack_ptr);
    current_thread->thread_context.uc_stack.ss_sp = NULL;
	current_thread->thread_context.uc_stack.ss_size = 0;
	current_thread->thread_context.uc_stack.ss_flags = 0;
    current_thread->thread_context.uc_link = NULL;
    for (int i=(current_thread_idx+1); i<active_threads; i++) {
        thread_list[i-1] = thread_list[i];
    }
    active_threads--;
	broadcast_to_waiters(current_thread->thread_id);
}

void CEthread_join(int thread_id) {
    Thread *current_thread = thread_list[current_thread_idx];
	current_thread->waiting_for_pid = thread_id;
	CEthread_yield();
}

Mutex * CEmutex_init() {
	if (active_mutexes > MAX_MUTEXES) {
		fprintf(stderr, "Mutex number out of range.\n");
		return NULL;
	}

	Mutex *mutex = malloc(sizeof(Mutex));
	mutex->is_locked = 0;
	mutexes[active_mutexes] = mutex;
	active_mutexes++;
	return mutex;
}

int CEmutex_lock(Mutex *mutex) {
	while(mutex->is_locked) {
		CEthread_yield();
	}
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
	active_mutexes--;
	return 0;
}

void wait_for_all_threads() {
	int remaining_threads = 0;
	
	/* If we are in a thread, wait for all the *other* threads to quit */
	if (is_thread) remaining_threads = 1;	
	/* Execute the threads until they quit */
	while (active_threads > remaining_threads) {
		CEthread_yield();
	}
	
	return;
}