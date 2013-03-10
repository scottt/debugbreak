#ifndef DEBUG_BREAK_H
#define DEBUG_BREAK_H

#include <signal.h>
#include <unistd.h>
#include <sys/syscall.h>

#ifdef __cplusplus
extern "C" {
#endif

enum {
	DEBUG_BREAK_PREFER_BUILTIN_TRAP_TO_SIGTRAP = 1,
};

#if defined(__i386__) || defined(__x86_64__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
__attribute__((gnu_inline, always_inline))
static void __inline__ trap_instruction(void)
{
	__asm__ volatile("int $0x03");
}
#elif defined(__thumb__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
#warning "debug_break: __thumb__: untested"
/* FIXME: __THUMB_INTERWORK__ could mix thumb and ARM code in one process, detect thumb mode at runtime? */
__attribute__((gnu_inline, always_inline))
static void __inline__ trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in the GDB source.
	 * Instruction used here is called 'eabi_linux_thumb_le_breakpoint' there.
	 */
	__asm__ volatile(".inst 0xde01");
}
#elif defined(__arm__) && !defined(__thumb__)
enum { HAVE_TRAP_INSTRUCTION = 1, };
__attribute__((gnu_inline, always_inline))
static void __inline__ trap_instruction(void)
{
	/* See 'arm-linux-tdep.c' in the GDB source.
	 * Instruction used here is called 'eabi_linux_arm_le_breakpoint' there.
	 *
	 * Known problem:
	 * After a breakpoint hit, can't stepi, step, or continue in GDB.
	 * 'step' stuck on the same instruction.
	 *
	 * Workaround: jump over the 4 byte instruction
	 * (gdb) tbreak *($pc + 4)
	 * (gdb) jump   *($pc + 4)
	 *
	 * debugbreak-gdb.py adds a 'debugbreak-stepi' command in GDB for this.
	 */
	__asm__ volatile(".inst 0xe7f001f0");
}
#else
enum { HAVE_TRAP_INSTRUCTION = 0, };
#endif

__attribute__((gnu_inline, always_inline))
static void __inline__ debug_break(void)
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

#ifdef __cplusplus
}
#endif

#endif
