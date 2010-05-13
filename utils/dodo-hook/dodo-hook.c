#include <stdio.h>
#include <stdlib.h>

#define TRACE_FILE "/tmp/trace.out"

void __cyg_profile_func_enter(void*, void*) __attribute__ ((no_instrument_function));
void __cyg_profile_func_exit(void*, void*) __attribute__ ((no_instrument_function));
void initializer() __attribute__ ((no_instrument_function, constructor));
void finalizer() __attribute__ ((no_instrument_function, destructor));

FILE *fp;

/* Module initializer */
void initializer()
{
	fp = fopen(TRACE_FILE, "w");
	if(!fp) {
		perror("Failed to open file '" TRACE_FILE "'");
		exit(1);
	}

	fprintf(stderr, "*** dodo-hook initialized...\n");
}

/* Hook function get called immediately after entering a function */
void __cyg_profile_func_enter(void *this, void *callsite)
{
}

/* Hook function called just before exiting a function */
void __cyg_profile_func_exit(void *this, void *callsite)
{
}

/* Module finalizer */
void finalizer()
{
	if(fp) {
		fclose(fp);
	}
	fprintf(stderr, "*** dodo-hook finalized...\n");
}

