#include <stdio.h>
#include <stdlib.h>

#include <gcctrace.h>

static int indent = 0;

void __cyg_profile_func_enter (void *this_fn, void *call_site)
{
	int i = 0;
	for(i=0; i<indent; i++) printf("\t");
	printf("calling %p from %p\n", this_fn, call_site);
	indent++;
}

void __cyg_profile_func_exit (void *this_fn, void *call_site)
{
	int i = 0;
	indent--;
	for(i=0; i<indent; i++) printf("\t");
	printf("returning %p to %p\n", this_fn, call_site);
}
