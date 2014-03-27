#include <gcctrace.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

static void increment_int_ptr(int* int_ptr)
{
	*int_ptr = *int_ptr+1;
}

static void thread_do_internal_work(int* int_ptr)
{
	while((*int_ptr) < 8)
	{
		increment_int_ptr(int_ptr);
	}
}

void *thread_func(void *int_ptr)
{
	int *x_ptr = (int *)int_ptr;
	thread_do_internal_work(x_ptr);
}

int main()
{
	int x = 0, y = 0;
	pthread_t inc_x_thread;

	printf("x: %d, y: %d\n", x, y);

	if(pthread_create(&inc_x_thread, NULL, thread_func, &x)) 
	{
		fprintf(stderr, "Error creating thread\n");
		return 1;

	}

	while(y < 16)
	{
		increment_int_ptr(&y);
	}

	if(pthread_join(inc_x_thread, NULL)) 
	{
		fprintf(stderr, "Error joining thread\n");
		return 2;

	}

	printf("x: %d, y: %d\n", x, y);

	return 0;
}
