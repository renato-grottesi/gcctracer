#include <stdlib.h>

typedef struct stack_frame
{
	unsigned long int time;
	unsigned long int thread;
	unsigned long int used_bytes;
	void* this_fn;
	void* call_site;
} stack_frame;

typedef struct call_stack
{
	unsigned int num_frames;
	stack_frame *frames;
} call_stack;

void __cyg_profile_func_enter(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) 
	__attribute__((no_instrument_function));

void* malloc(size_t sz) 
	__attribute__((no_instrument_function));
void free(void *p) 
	__attribute__((no_instrument_function));
void *calloc(size_t nmemb, size_t size) 
	__attribute__((no_instrument_function));
void *realloc(void *ptr, size_t size) 
	__attribute__((no_instrument_function));

void _gcc_trace_get_call_stack(call_stack* stack);
void _gcc_trace_print_call_stack(call_stack* stack);

/* TODO: Add functions to dump the circular buffer */
