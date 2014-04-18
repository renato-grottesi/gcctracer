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

/* global constants */
static const size_t hd = 8; /* hidden data */
static const size_t hd_mask = 0xF; /* hidden data mask */
static const size_t hd_imask = ~0xF; /* hidden data inverted mask */

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

static inline size_t _gcc_trace_align_address(size_t size)
{
	/* allocate hd extra bytes for hidden data */
	/* align to */
	return (size + hd + hd_mask) & hd_imask;
}

static inline void *_gcc_trace_hidden_pointer(void *ptr)
{
	return (void*)(((unsigned char *)(ptr))+hd);
}

static inline void *_gcc_trace_original_pointer(void *ptr)
{
	return (void *)(((unsigned char *)ptr)-hd);
}

static inline void _gcc_trace_set_hidden_size(size_t size, void* ptr)
{
	*((unsigned int *)(ptr)) = size;
}

static inline size_t _gcc_trace_get_hidden_size(void* ptr)
{
	return *((unsigned int *)(((unsigned char *)ptr)-hd));
}

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;
	size_t nsize = _gcc_trace_align_address(size);

	ptr = __libc_calloc(nmemb, nsize);
	__sync_add_and_fetch(&total_mem, size);
	_gcc_trace_set_hidden_size(size, ptr);
	return _gcc_trace_hidden_pointer(ptr);
}

void *realloc(void *ptr, size_t size)
{
	void *rptr;
	size_t nsize = _gcc_trace_align_address(size);
	size_t previous_size = _gcc_trace_get_hidden_size(ptr);

	__sync_sub_and_fetch(&total_mem, previous_size);
	rptr = __libc_realloc(_gcc_trace_original_pointer(ptr), nsize);
	__sync_add_and_fetch(&total_mem, size);
	_gcc_trace_set_hidden_size(size, rptr);
	return _gcc_trace_hidden_pointer(rptr);
}

void *malloc(size_t size)
{
	void *ptr;
	size_t nsize = _gcc_trace_align_address(size);

	ptr = __libc_malloc(nsize);
	__sync_add_and_fetch(&total_mem, size);
	_gcc_trace_set_hidden_size(size, ptr);
	return _gcc_trace_hidden_pointer(ptr);
}

void free(void *ptr)
{
	if(ptr)
	{
		size_t previous_size = _gcc_trace_get_hidden_size(ptr);
		__sync_sub_and_fetch(&total_mem, previous_size);
		__libc_free(_gcc_trace_original_pointer(ptr));
	}
}

unsigned long int _gcc_trace_get_heap_memory()
{
	return total_mem;
}

