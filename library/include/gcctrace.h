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
 * @file gcctrace.h
 * @author Renato Grottesi
 * @date 7 Apr 2014
 * @brief Main file to include gcctrace's functionalities
 */

#include <stdlib.h>

/**
 * @brief Stack frame structure
 *
 * Stack frame containing the traced data
 */
typedef struct stack_frame
{
	unsigned long int time; /**< Timestamp */
	unsigned long int thread; /**< Thread ID */
	unsigned long int used_bytes; /**< Allocated memory in bytes */
	void* this_fn; /**< Pointer to the invoked function */
	void* call_site; /**< Place in the code where this_fn was called */
} stack_frame;

/**
 * @brief Full calling stack
 *
 * Full calling stack 
 */
typedef struct call_stack
{
	unsigned int num_frames; /**< How deep is the stack */
	stack_frame *frames; /**< Stack frames array */
} call_stack;

/**
 * @brief enter function
 *
 * This function will be called before any other function can start
 *
 * @param this_fn Function getting called
 * @param call_site Place in the source code where @p this_func 
 * is getting called
 */
void __cyg_profile_func_enter(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));

/**
 * @brief exit function
 *
 * This function will be called before any other function returns
 *
 * @param this_fn Function that was called
 * @param call_site Place in the source code where this_func was called
 */
void __cyg_profile_func_exit(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));

/**
 * libc malloc function wrapper
 *
 * @param size How many bytes to allocate
 */
void* malloc(size_t size)
	__attribute__((no_instrument_function));

/**
 * libc free function wrapper
 *
 * @param ptr The pointer to free
 */
void free(void *ptr) 
	__attribute__((no_instrument_function));

/**
 * libc calloc function wrapper
 *
 * @param nmemb Numer of members to allocate
 * @param size How many bytes to allocate
 */
void *calloc(size_t nmemb, size_t size) 
	__attribute__((no_instrument_function));

/**
 * libc realloc function wrapper
 *
 * @param ptr The pointer to free
 * @param size How many bytes to allocate
 */
void *realloc(void *ptr, size_t size) 
	__attribute__((no_instrument_function));

/**
 * Copy the current call stack inside the input parameter.
 * This function allocates stack's internal memory.
 * Please call _gcc_trace_free_call_stack to free the internal memory.
 * 
 * @param stack The call stack object where to clone the curren call stack
 * @see _gcc_trace_free_call_stack
 */
void _gcc_trace_clone_current_call_stack(call_stack* stack);

/**
 * Free the internal memory allocated by _gcc_trace_clone_current_call_stack
 * 
 * @param stack The call stack object to free
 * @see _gcc_trace_clone_current_call_stack
 */
void _gcc_trace_free_call_stack(call_stack* stack);

/**
 * Prints a call stack cloned by _gcc_trace_clone_current_call_stack.
 *
 * @param stack The stack to print in stderr
 * @see _gcc_trace_clone_current_call_stack
 */
void _gcc_trace_print_call_stack(call_stack* stack);

/* TODO: Add functions to dump the circular buffer */
