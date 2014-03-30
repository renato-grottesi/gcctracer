#define MAX_CALL_STACK_DEEP 128

typedef struct stack_frame
{
	unsigned long int time;
	unsigned long int thread;
	unsigned long int used_memory_kb;
	void* this_fn;
	void* call_site;
} stack_frame;

typedef struct call_stack
{
	int num_frames;
	stack_frame frames[MAX_CALL_STACK_DEEP];
} call_stack;

void __cyg_profile_func_enter(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void __cyg_profile_func_exit(void *this_fn, void *call_site) __attribute__((no_instrument_function));
void _gcc_trace_get_call_stack(call_stack* stack);
void _gcc_trace_print_call_stack(call_stack* stack);

