#include <stdio.h>
#include <signal.h>

#include <unistd.h>
#include <sys/syscall.h>

enum {
	DEBUG_BREAK_PREFER_BUILTIN_TRAP_TO_SIGTRAP = 1,
};

#if defined(__i386__) || defined(__x86_64__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
__attribute__((always_inline))
static void trap_instruction(void)
{
	asm("int $0x03");
}
#elif defined(__thumb__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
#warning "debug_break: __thumb__: untested"
/* FIXME: __THUMB_INTERWORK__ could mix thumb and ARM code in one process, detect thumb mode at runtime? */
__attribute__((always_inline))
static void trap_instruction(void)
{
	asm(".inst\t0xdeff");
}
#elif defined(__arm__) && !defined(__thumb__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
#warning "debug_break: __arm__: untested"
static void trap_instruction(void)
{
	asm(".inst\t0xe7ffffff");
}
#else
#error "debug_break: else"
enum { HAVE_TRAP_INSTRUCTION = 0, };
#endif

__attribute__((always_inline))
static void debug_break(void)
{
	if (HAVE_TRAP_INSTRUCTION) {
		trap_instruction();
	} else if (DEBUG_BREAK_PREFER_BUILTIN_TRAP_TO_SIGTRAP) {
		 /* raises SIGILL on Linux x86{,-64}, to continue in gdb:
		  * (gdb) handle SIGILL stop nopass
		  * */
		__builtin_trap();
	} else {
		raise(SIGTRAP);
	}
}

int fib(int n)
{
	int r;
	if (n == 0 || n == 1)
		return 1;
	r = fib(n-1) + fib(n-2);
	if (r == 89) {
		debug_break();
	}
	return r;
}

int main()
{
	printf("%d\n", fib(15));
	return 0;
}
