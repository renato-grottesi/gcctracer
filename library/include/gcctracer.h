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
 * @file gcctracer.h
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
 * Structure that contains a full calling stack 
 */
typedef struct call_stack
{
	unsigned int num_frames; /**< How deep is the stack */
	stack_frame *frames; /**< Stack frames array */
} call_stack;

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

/**
 * Dump the internal circular buffer containing the history of the last n
 * function invocations, where n is an hardcoded size for the buffer capacity.
 *
 * @param file_name The name of the file where to dump the data
 */
void _gcc_trace_dump_history_buffer(const char* file_name);

/**
 * Return the total heap memory allocated
 *
 * @return total bytes allocated in heap
 */
unsigned long int _gcc_trace_get_heap_memory();

