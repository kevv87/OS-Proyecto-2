#include "CEthreads.h"
#include <stdio.h>

#include <unistd.h>

int mutex_access_indicator = 0;
Mutex *mutex;

void fiber1() {
	CEthread_join(1);
	for (int i = 0; i < 5; ++ i ){
		printf( "Hey, I'm fiber #1: %d\n", i );
		CEthread_yield();
	}
	return;
}

void fibonacchi() {
	int fib[2] = { 0, 1 };
	
	printf( "fibonacchi(0) = 0\nfibonnachi(1) = 1\n" );
	for(int i = 2; i < 15; ++ i ) {
		int nextFib = fib[0] + fib[1];
		printf( "fibonacchi(%d) = %d\n", i, nextFib );
		fib[0] = fib[1];
		fib[1] = nextFib;
		CEthread_yield();
	}
}

void squares() {
	for (int i = 0; i < 10; ++ i ){
		printf( "%d*%d = %d\n", i, i, i*i );
		CEthread_yield();
	}
}

void test_mutex_0() {
	for(int i=0; i<10; i++) {
		CEmutex_lock(mutex);
		mutex_access_indicator = 0;
		CEthread_yield();
		printf("Test mutex 0, el valor deberia ser 0: %d\n", mutex_access_indicator);
		CEthread_yield();
		CEmutex_unlock(mutex);
		CEthread_yield();
	}
}

void test_mutex_1() {
	for(int i=0; i<10; i++) {
		CEmutex_lock(mutex);
		mutex_access_indicator = 1;
		CEthread_yield();
		printf("Test mutex 1, el valor deberia ser 1: %d\n", mutex_access_indicator);
		CEmutex_unlock(mutex);
		CEthread_yield();
	}
}

int main(){
	mutex = CEmutex_init();
	/* Go fibers! */
	CEthread_create( &fibonacchi );
	CEthread_create( &fiber1 );
	CEthread_create( &squares );
	CEthread_create(&test_mutex_0);
	CEthread_create(&test_mutex_1);

	/* Since these are nonpre-emptive, we must allow them to run */
	wait_for_all_threads();
	
	/* The program quits */
	return 0;
}