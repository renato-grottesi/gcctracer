#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

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

static inline void _gcc_trace_format_string(char* str, const char* word, call_stack* stack, int frame_index)
{
	int i = 0;
	char tabs[1024];

	str[0] = '\0';

	for(i=0; i<frame_index&& i<1023; i++) tabs[i]='\t';
	tabs[i] = '\0';
	snprintf(str, 4096, "%ld% s[%d] #%d %s %p from %p\n", 
		stack->frames[frame_index].time, 
		tabs, 
		stack->frames[frame_index].thread,
		frame_index, 
		word, 
		stack->frames[frame_index].this_fn, 
		stack->frames[frame_index].call_site);
}

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	char str[4096];

	thread_stack.frames[thread_stack.num_frames].this_fn=this_fn;
	thread_stack.frames[thread_stack.num_frames].call_site=call_site;
	thread_stack.frames[thread_stack.num_frames].time=clock();
	thread_stack.frames[thread_stack.num_frames].thread=_gcc_trace_get_tid();
	thread_stack.frames[thread_stack.num_frames].used_memory_kb=0;

	_gcc_trace_format_string(str, "entering", &thread_stack, thread_stack.num_frames);

	thread_stack.num_frames++;

	fprintf(stderr, str);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	char str[4096];
	thread_stack.num_frames--;
	_gcc_trace_format_string(str, "returning", &thread_stack, thread_stack.num_frames);
	fprintf(stderr, str);
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
		fprintf(stderr, str);
	}
	fprintf(stderr, "\n----\n\n");
}

