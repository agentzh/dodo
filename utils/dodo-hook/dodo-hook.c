#include <sys/types.h>
#include <sys/time.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define TRACE_PID_FILE "trace.out.%d"

void __cyg_profile_func_enter(void*, void*) __attribute__ ((no_instrument_function));
void __cyg_profile_func_exit(void*, void*) __attribute__ ((no_instrument_function));
void initializer() __attribute__ ((no_instrument_function, constructor));
void finalizer() __attribute__ ((no_instrument_function, destructor));
static void* get_ctx_arg0_by_hook_this(void **this);

FILE *fp;
int seq;
int call_depth;	/* for now we focus on single-thread app, so a global var is well enough to record call depth */

/* Module initializer */
void initializer()
{
	char tmp[255];
	snprintf(tmp, sizeof(tmp), TRACE_PID_FILE, getpid());

	fp = fopen(tmp, "w");
	if(!fp) {
		fprintf(stderr, "Failed to open file '%s': %s\n", tmp, strerror(errno));
		exit(1);
	}

	seq = 0;
	call_depth = 0;
	fprintf(stderr, "*** dodo-hook initialized, output to file '%s'\n", tmp);
}

/* Hook function get called immediately after entering a function */
void __cyg_profile_func_enter(void *this, void *callsite)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	call_depth++;
	fprintf(fp, "%d %d.%06d %d %p I &arg0:%p\n",
			seq,
			(int)tv.tv_sec,
			(int)tv.tv_usec,
			call_depth,
			this,
			get_ctx_arg0_by_hook_this(&this)
			);
	seq++;
}

/* Hook function called just before exiting a function */
void __cyg_profile_func_exit(void *this, void *callsite)
{
	struct timeval tv;
	gettimeofday(&tv, NULL);
	fprintf(fp, "%d %d.%06d %d %p O &arg0:%p\n",
			seq,
			(int)tv.tv_sec,
			(int)tv.tv_usec,
			call_depth,
			this,
			get_ctx_arg0_by_hook_this(&this)
			);
	seq++;
	call_depth--;
}

/* Module finalizer */
void finalizer()
{
	if(fp) {
		fclose(fp);
	}
	fprintf(stderr, "*** dodo-hook finalized...\n");
}

/**
 * Get pointer to the 1st param of ctx func based on gcc hook's 'this' param
 * addr.  The rest params of ctx func can be accessed by iterating the pointer
 * forward.
 *
 * NOTE: This function assumes standard stack frame layout (i.e. EBP is
 * preserved in a chaining form). If stack frame is omitted (e.g.
 * -fomit-frame-pointer is used to optimize code size), the return value would
 *  be unpredictable!
 * */
static void* get_ctx_arg0_by_hook_this(void **this)
/* {{{ */
{
	/**
	 * (void*)(*(char**)((char*)&this - ws - ws) + ws + ws)
	 *		or
	 * (void*)(*(void**)((void**)&this - 1 - 1) + 1 + 1)
	 * */
#define WORD_SIZE sizeof(void*)
	char* p = (char*)this;	/* 'this' param addr */
	p -= WORD_SIZE;			/* ret addr of gcc hook */
	p -= WORD_SIZE;			/* preserved orig EBP of current stack frame */
	p = *(char**)p;			/* preserved orig EBP of upper stack frame */
	p += WORD_SIZE;			/* ret addr of ctx func */
	p += WORD_SIZE;			/* addr of ctx func's 1st arg */
	return (void*)p;
}
/* }}} */

/**
 * vim:ft=c ts=4 sw=4 fdm=marker
 * */

