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
__thread call_stack thread_stack = { 0, NULL };

/* TODO: make atomic*/
static unsigned long int total_mem = 0;
static const unsigned int max_call_stack_deep = 1024;

static void _gcc_trace_init(void) __attribute__((constructor));
static void _gcc_trace_finish(void) __attribute__((destructor));

void _gcc_trace_init(void)
{
}

void _gcc_trace_finish(void)
{
	/* TODO: place to dump the whole trace circular buffer to a file */
}

/* The following functions are standard but hidden */
extern void *__libc_calloc(size_t nmemb, size_t size);
extern void *__libc_malloc(size_t size);
extern void *__libc_realloc(void *ptr, size_t size);
extern void __libc_free(void* ptr);

void *calloc(size_t nmemb, size_t size)
{
	void *ptr;

	ptr = __libc_calloc(nmemb, size+8);
	total_mem += size;
	*((unsigned int *)(ptr)) = size;
	return (void*)(((unsigned char *)(ptr))+8);
}

void *realloc(void *ptr, size_t size)
{
	void *rptr;

	total_mem -= *((unsigned int *)(((unsigned char *)ptr)-8));
	rptr = __libc_realloc( (void *)(((unsigned char *)ptr)-8), size+8);
	total_mem += size;
	*((unsigned int *)(rptr)) = size;
	return (void*)(((unsigned char *)(rptr))+8);
}

void *malloc(size_t size)
{
	void *ptr;

	ptr = __libc_malloc(size+8);
	total_mem += size;
	*((unsigned int *)(ptr)) = size;
	return (void*)(((unsigned char *)(ptr))+8);
}

void free(void *ptr)
{
	if(ptr)
	{
		total_mem -= *((unsigned int *)(((unsigned char *)ptr)-8));
		__libc_free( (void *)(((unsigned char *)ptr)-8));
	}
}

static inline int _gcc_trace_get_tid()
{
	if(!TLS_tid)
	{
		TLS_tid = (unsigned long int)pthread_self();
	}

	return TLS_tid;
}

static void _gcc_trace_func_name(
	const void *this_fn, 
	char* str_out, 
	unsigned int str_size)
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

static inline void _gcc_trace_format_string(
	char* str, 
	const char* word, 
	call_stack* stack, 
	int frame_index)
{
	int i = 0;
	char tabs[1024];
	char func_name[1024];

	str[0] = '\0';

	_gcc_trace_func_name(
		stack->frames[frame_index].this_fn, 
		func_name, 
		1024);

	for(i=0; i<frame_index&& i<1023; i++) tabs[i]='\t';
	tabs[i] = '\0';

	snprintf(
		str, 
		4096, 
		"%ld [%ld bytes] %s [%ld] #%d %s %p (%s) from %p\n", 
		stack->frames[frame_index].time, 
		stack->frames[frame_index].used_bytes,
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
	stack_frame* f;

	if(NULL==thread_stack.frames)
	{
		thread_stack.frames = malloc(
				max_call_stack_deep * sizeof(stack_frame));
	}

	f = &(thread_stack.frames[thread_stack.num_frames]);

	f->this_fn = this_fn;
	f->call_site = call_site;
	f->time = _gcc_trace_get_time();
	f->thread = _gcc_trace_get_tid();
	f->used_bytes = total_mem;

	_gcc_trace_format_string(
		str, 
		"entering", 
		&thread_stack, 
		thread_stack.num_frames);

	thread_stack.num_frames++;

	if ( thread_stack.num_frames > max_call_stack_deep )
	{
		fprintf(
			stderr, 
			"\nInvariant Mismatch! Stack %d deeper than MAX %d\n", 
			thread_stack.num_frames,
			max_call_stack_deep
			);
	}

	fprintf(stderr, "%s", str);
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	char str[4096];
	stack_frame* f;

	thread_stack.num_frames--;

	f = &(thread_stack.frames[thread_stack.num_frames]);

	f->time=_gcc_trace_get_time();
	f->used_bytes=total_mem;

	if (this_fn != f->this_fn || call_site != f->call_site)
	{
		fprintf(
			stderr, 
			"\nInvariant Mismatch! %p from %p\n", 
			this_fn, 
			call_site);
	}

	_gcc_trace_format_string(str, 
		"returning", 
		&thread_stack, 
		thread_stack.num_frames);
	fprintf(stderr, "%s", str);
}

void _gcc_trace_get_call_stack(call_stack* stack)
{
	if(stack)
	{
		unsigned int i;

		stack->num_frames = thread_stack.num_frames;
		stack->frames = malloc(
				stack->num_frames * sizeof(stack_frame));
		
		for(i=0; i<stack->num_frames; i++)
		{
			stack_frame* f_out = &(stack->frames[i]);
			stack_frame* f_in = &(thread_stack.frames[i]);

			f_out->this_fn = f_in->this_fn;
			f_out->call_site = f_in->call_site;
			f_out->time = f_in->time;
			f_out->thread = f_in->thread;
			f_out->used_bytes = f_in->used_bytes;
		}
	}
}

void _gcc_trace_print_call_stack(call_stack* stack)
{
	unsigned int i;

	fprintf(stderr, "\n\n----\n");
	for(i=0; i<stack->num_frames; i++)
	{
		char str[4096];
		_gcc_trace_format_string(str, "", stack, i);
		fprintf(stderr, "%s", str);
	}
	fprintf(stderr, "\n----\n\n");
}

