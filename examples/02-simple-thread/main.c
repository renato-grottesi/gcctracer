/****************************************************************************
 * Copyright (C) 2014 by Renato Grottesi                                    *
 *                                                                          *
 * This file is part of gcctrace.                                           *
 *                                                                          *
 *   gcctrace is free software: you can redistribute it and/or modify it    *
 *   under the terms of the GNU Lesser General Public License as published  *
 *   by the Free Software Foundation, either version 3 of the License, or   *
 *   (at your option) any later version.                                    *
 *                                                                          *
 *   gcctrace is distributed in the hope that it will be useful,            *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of         *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the          *
 *   GNU Lesser General Public License for more details.                    *
 *                                                                          *
 *   You should have received a copy of the GNU Lesser General Public       *
 *   License along with gcctrace.                                           *
 *   If not, see <http://www.gnu.org/licenses/>.                            *
 ****************************************************************************/

/**
 * @file main.c
 * @author Renato Grottesi
 * @date 7 Apr 2014
 * @brief Main file to include gcctrace's functionalities
 */

#include <gcctracer.h>
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

void pstack()
{
	call_stack my_stack;  
	_gcc_trace_clone_current_call_stack(&my_stack);
	_gcc_trace_print_call_stack(&my_stack);
	_gcc_trace_free_call_stack(&my_stack);
}

void increment_int_ptr(int* int_ptr)
{
	*int_ptr = *int_ptr+1;
	if(*int_ptr==2)
	{
		pstack();
	}
}

void thread_do_internal_work(int* int_ptr)
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

#ifdef ARM
	/* linaro compiler bug */
	return 0;
#endif
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
