#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#include <gcctrace.h>

__thread int indent = 0;
__thread unsigned long int TLS_tid = 0;

static inline int _gcc_trace_get_tid()
{
	if(!TLS_tid)
	{
		TLS_tid = (unsigned long int)pthread_self();
	}

	return TLS_tid;
}

static inline void _gcc_trace_format_string(char* str, const char* word, void *this_fn, void *call_site)
{
	int i = 0;
	char tabs[1024];

	str[0] = '\0';

	for(i=0; i<indent && i<1023; i++) tabs[i]='\t';
	tabs[i] = '\0';
	snprintf(str, 4096, "%s[%d] %s %p from %p\n", tabs, _gcc_trace_get_tid(), word, this_fn, call_site);
}

void __cyg_profile_func_enter(void *this_fn, void *call_site)
{
	char str[4096];
	_gcc_trace_format_string(str, "entering", this_fn, call_site);
	fprintf(stderr, str);
	indent++;
}

void __cyg_profile_func_exit(void *this_fn, void *call_site)
{
	char str[4096];
	indent--;
	_gcc_trace_format_string(str, "returning", this_fn, call_site);
	fprintf(stderr, str);
}
