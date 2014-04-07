#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/time.h>

/* includes for dladdr */
#define _GNU_SOURCE
#define __USE_GNU
#include <dlfcn.h>

#include <gcctrace.h>

__thread unsigned long int TLS_tid = 0;
__thread call_stack thread_stack = { 0, };

static inline int _gcc_trace_get_tid()
{
	if(!TLS_tid)
	{
		TLS_tid = (unsigned long int)pthread_self();
	}

	return TLS_tid;
}

static void _gcc_trace_func_name(const void *this_fn, char* str_out, unsigned int str_size)
{
	Dl_info info;

	str_out[0] = str_out[1] = str_out[2] = '?';
	str_out[3] = '\0';

	/* this doesn't work for inlined or static functions */
	if (dladdr(this_fn, &info))
	{
		if (info.dli_sname)
		{
			strncpy(str_out, info.dli_sname, str_size);
		}
	}
}

static inline void _gcc_trace_format_string(char* str, const char* word, call_stack* stack, int frame_index)
{
	int i = 0;
	char tabs[1024];
	char func_name[1024];

	str[0] = '\0';

	_gcc_trace_func_name(stack->frames[frame_index].this_fn, func_name, 1024);

	for(i=0; i<frame_index&& i<1023; i++) tabs[i]='\t';
	tabs[i] = '\0';

	snprintf(str, 4096, "%ld %s [%ld] #%d %s %p (%s) from %p\n", 
		stack->frames[frame_index].time, 
		tabs, 
		stack->frames[frame_index].thread,
		frame_index, 
		word, 
		stack->frames[frame_index].this_fn, 
		func_name,
		stack->frames[frame_index].call_site);
}

static inline unsigned long int _gcc_trace_get_time()
{
	struct timeval time;
	gettimeofday(&time, NULL);
	return time.tv_sec*1000+time.tv_usec;
}

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	char str[4096];

	thread_stack.frames[thread_stack.num_frames].this_fn=this_fn;
	thread_stack.frames[thread_stack.num_frames].call_site=call_site;
	thread_stack.frames[thread_stack.num_frames].time=_gcc_trace_get_time();
	thread_stack.frames[thread_stack.num_frames].thread=_gcc_trace_get_tid();
	thread_stack.frames[thread_stack.num_frames].used_memory_kb=0;

	_gcc_trace_format_string(str, "entering", &thread_stack, thread_stack.num_frames);

	thread_stack.num_frames++;

	fprintf(stderr, "%s", str);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	char str[4096];

	thread_stack.num_frames--;

	thread_stack.frames[thread_stack.num_frames].time=_gcc_trace_get_time();

	if (
		this_fn != thread_stack.frames[thread_stack.num_frames].this_fn ||
		call_site != thread_stack.frames[thread_stack.num_frames].call_site
	   )
	{
		fprintf(stderr, "\nInvariant Mismatch! %p from %p\n", this_fn, call_site);
	}

	_gcc_trace_format_string(str, "returning", &thread_stack, thread_stack.num_frames);
	fprintf(stderr, "%s", str);
}

void _gcc_trace_get_call_stack(call_stack* stack)
{
	if(stack)
	{
		int i;

		stack->num_frames = thread_stack.num_frames;
		
		for(i=0; i<stack->num_frames; i++)
		{
			stack->frames[i].this_fn = thread_stack.frames[i].this_fn;
			stack->frames[i].call_site = thread_stack.frames[i].call_site;
			stack->frames[i].time = thread_stack.frames[i].time;
			stack->frames[i].thread = thread_stack.frames[i].thread;
			stack->frames[i].used_memory_kb = thread_stack.frames[i].used_memory_kb;
		}
	}
}

void _gcc_trace_print_call_stack(call_stack* stack)
{
	int i;

	fprintf(stderr, "\n\n----\n");
	for(i=0; i<stack->num_frames; i++)
	{
		char str[4096];
		_gcc_trace_format_string(str, "", stack, i);
		fprintf(stderr, "%s", str);
	}
	fprintf(stderr, "\n----\n\n");
}

