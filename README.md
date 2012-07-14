"debugbreak.h" allows you to put breakpoints in your sourcde code with:
```C
#include <stdio.h>
#include "debugbreak.h"

int main()
{
	debug_break();
	printf("hello world\n");
	return 0;
}
```
Much like the DebugBreak() intrinsic provided by Windows and QNX headers.

Note that gcc's [__builtin_trap()](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-g_t_005f_005fbuiltin_005ftrap-3278) is unsatisfactory because "continue" in gdb
doesn't work even if you do:
```
	(gdb) handle SIGILL stop nopass
	(gdb) file TEST-PROGRAM
	(gdb) run
	# breakpoint hit
	(gdb) continue
```
while it works with debug_break(). __builtin_trap() apparently also marks the instruction after it as unreachable and may cause the compiler optimizers to remove them so is not suitable for placing break points in source code.

NOTES
================================

gcc's [__builtin_trap()](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-g_t_005f_005fbuiltin_005ftrap-3278) generates an "ud2" instruction on i386 / x86-64:
```
$ gdbdis test/trap main
   0x0000000000400390 <+0>:	0f 0b	ud2    
$ gdbdis test/break main
   0x00000000004003d0 <+0>:	50	push   %rax
   0x00000000004003d1 <+1>:	cc	int3   
   0x00000000004003d2 <+2>:	bf a0 05 40 00	mov    $0x4005a0,%edi
   0x00000000004003d7 <+7>:	e8 d4 ff ff ff	callq  0x4003b0 <puts@plt>
   0x00000000004003dc <+12>:	31 c0	xor    %eax,%eax
   0x00000000004003de <+14>:	5a	pop    %rdx
   0x00000000004003df <+15>:	c3	retq   
```
Clang / LLVM also has __builtin_trap() but further provides a  [__builtin_debugger()](http://lists.cs.uiuc.edu/pipermail/llvm-commits/Week-of-Mon-20120507/142621.html) that generates "int $0x03" instead of "ud2" on i386 / x86-64.
