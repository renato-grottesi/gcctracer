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

typedef struct call_event
{
	stack_frame frame; /**< structure containing the common frame data */
	unsigned char is_enter; /**< 1=enter 0=exit */
	long int delta_mem; /**< how much memory was allocated/freed */
	unsigned int stack_depth; /**< depth of the stack fo this event */
} call_event;

/* global thread local storage variables */
__thread unsigned long int TLS_tid = 0;
__thread call_stack thread_stack = { 0, NULL };

/* global const */
static const unsigned int max_call_stack_deep = 1024;
static const unsigned int c_buff_size = 1024;

/* global atomics */
static unsigned long int total_mem = 0;
static unsigned long int c_buff_begin = 0;

/* global variables */
static call_event *c_buff = NULL;

/* contructors and destructors marked for no instrumentation */
static void _gcc_trace_init(void) __attribute__((constructor));
static void _gcc_trace_finish(void) __attribute__((destructor));

/* protptypes for libc functions that are standard but hidden */
extern void *__libc_calloc(size_t nmemb, size_t size);
extern void *__libc_malloc(size_t size);
extern void *__libc_realloc(void *ptr, size_t size);
extern void __libc_free(void* ptr);

/* cyg_profile prototypes marked for no instrumentation */
void __cyg_profile_func_enter(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));

/* libc alloc prototypes marked for no instrumentation */
void* malloc(size_t size)
	__attribute__((no_instrument_function));
void free(void *ptr) 
	__attribute__((no_instrument_function));
void *calloc(size_t nmemb, size_t size) 
	__attribute__((no_instrument_function));
void *realloc(void *ptr, size_t size) 
	__attribute__((no_instrument_function));

/* -------- implementation of gcctrace functions starts here -------- */

void _gcc_trace_init(void)
{
	c_buff = __libc_calloc(c_buff_size, sizeof(call_event));
	c_buff_begin = 0;
}

void _gcc_trace_finish(void)
{
	_gcc_trace_dump_history_buffer("gcctrace.dump");

	if(NULL!=thread_stack.frames)
	{
		__libc_free(thread_stack.frames);
		thread_stack.frames = NULL;
	}

	if(NULL!=c_buff)
	{
		__libc_free(c_buff);
		c_buff = NULL;
	}
}

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
	unsigned long int c_buff_idx = 0;

	if(NULL==thread_stack.frames)
	{
		size_t frames_size = max_call_stack_deep * sizeof(stack_frame);
		thread_stack.frames = __libc_malloc(frames_size);
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

	c_buff_idx = __sync_fetch_and_add(&c_buff_begin, 1) % c_buff_size;
	c_buff[c_buff_idx].frame = *f;
	c_buff[c_buff_idx].is_enter = 1;
	c_buff[c_buff_idx].delta_mem = 0;
	c_buff[c_buff_idx].stack_depth = thread_stack.num_frames;

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
	unsigned long int c_buff_idx = 0;
	unsigned long int used_memory_at_exit = total_mem;
	long int delta_mem;

	thread_stack.num_frames--;

	f = &(thread_stack.frames[thread_stack.num_frames]);

	delta_mem = used_memory_at_exit - f->used_bytes;

	f->time=_gcc_trace_get_time();
	f->used_bytes=used_memory_at_exit;

	if (this_fn != f->this_fn || call_site != f->call_site)
	{
		fprintf(
			stderr, 
			"\nInvariant Mismatch! %p from %p\n", 
			this_fn, 
			call_site);
	}

	c_buff_idx = __sync_fetch_and_add(&c_buff_begin, 1) % c_buff_size;
	c_buff[c_buff_idx].frame = *f;
	c_buff[c_buff_idx].is_enter = 0;
	c_buff[c_buff_idx].delta_mem = delta_mem;
	c_buff[c_buff_idx].stack_depth = thread_stack.num_frames;

	_gcc_trace_format_string(str, 
		"returning", 
		&thread_stack, 
		thread_stack.num_frames);
	fprintf(stderr, "%s", str);
}

void _gcc_trace_clone_current_call_stack(call_stack* stack)
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

void _gcc_trace_free_call_stack(call_stack* stack)
{
	if(stack)
	{
		free(stack->frames);
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

void _gcc_trace_dump_history_buffer(const char* file_name)
{
	FILE* f = fopen(file_name, "w+");
	if(NULL!=f)
	{
		unsigned int i;

		for(i=0; i<c_buff_size; i++)
		{
			unsigned int c_buff_idx = (i+c_buff_begin)%c_buff_size;
			call_event* event = &(c_buff[c_buff_idx]);

			/* skip null entries */
			if(NULL!=event->frame.this_fn)
			{
				char func_name[1024];

				_gcc_trace_func_name(
						event->frame.this_fn,
						func_name, 
						1024);

				fprintf(
						f, 
						"%s,%d,%s,%p,%p,%ld,%ld,"
						"%ld,%ld\n",
						event->is_enter?"ENTER":"EXIT",
						event->stack_depth,
						func_name,
						event->frame.this_fn,
						event->frame.call_site,
						event->frame.time,
						event->frame.thread,
						event->frame.used_bytes,
						event->delta_mem
				       );
			}
		}

		fclose(f);
	}
}

