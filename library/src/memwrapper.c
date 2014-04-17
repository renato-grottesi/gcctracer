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
 * @file memwrapper.c
 * @author Renato Grottesi
 * @date 7 Apr 2014
 * @brief Main file to include gcctrace's functionalities
 */

#include <stdlib.h>

#include <gcctracer.h>

/* global atomics */
static unsigned long int total_mem = 0;

/* protptypes for libc functions that are standard but hidden */
extern void *__libc_calloc(size_t nmemb, size_t size);
extern void *__libc_malloc(size_t size);
extern void *__libc_realloc(void *ptr, size_t size);
extern void __libc_free(void* ptr);

/* libc alloc prototypes marked for no instrumentation */
void* malloc(size_t size)
	__attribute__((no_instrument_function));
void free(void *ptr) 
	__attribute__((no_instrument_function));
void *calloc(size_t nmemb, size_t size) 
	__attribute__((no_instrument_function));
void *realloc(void *ptr, size_t size) 
	__attribute__((no_instrument_function));

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = __libc_calloc(nmemb, size+8);
	__sync_add_and_fetch(&total_mem, size);
	*((unsigned int *)(ptr)) = size;
	return (void*)(((unsigned char *)(ptr))+8);
}

void *realloc(void *ptr, size_t size)
{
	void *rptr;
	size_t previous_size = *((unsigned int *)(((unsigned char *)ptr)-8));

	__sync_sub_and_fetch(&total_mem, previous_size);
	rptr = __libc_realloc( (void *)(((unsigned char *)ptr)-8), size+8);
	__sync_add_and_fetch(&total_mem, size);
	*((unsigned int *)(rptr)) = size;
	return (void*)(((unsigned char *)(rptr))+8);
}

void *malloc(size_t size)
{
	void *ptr;

	ptr = __libc_malloc(size+8);
	__sync_add_and_fetch(&total_mem, size);
	*((unsigned int *)(ptr)) = size;
	return (void*)(((unsigned char *)(ptr))+8);
}

void free(void *ptr)
{
	if(ptr)
	{
		size_t previous_size;
		previous_size = *((unsigned int *)(((unsigned char *)ptr)-8));
		__sync_sub_and_fetch(&total_mem, previous_size);
		__libc_free( (void *)(((unsigned char *)ptr)-8));
	}
}

unsigned long int _gcc_trace_get_heap_memory()
{
	return total_mem;
}

