[debugbreak.h](https://github.com/scottt/debugbreak/blob/master/debugbreak.h) allows you to put breakpoints in your C/C++ code with a call to **debug_break()**, e.g.:
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
this works like the **DebugBreak()** intrinsic provided by [Windows](http://msdn.microsoft.com/en-us/library/ea9yy3ey.aspx) and [QNX](http://www.qnx.com/developers/docs/6.3.0SP3/neutrino/lib_ref/d/debugbreak.html).

Note that gcc's [__builtin_trap()](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-g_t_005f_005fbuiltin_005ftrap-3278) is unsatisfactory for this purpose because once you hit a breakpoint caused by it you can't **continue** in gdb. Even if you setup gdb with:
```
	(gdb) handle SIGILL stop nopass
```
**continue** still doesn't work reliably with **__builtin_trap()** while it does work with **debug_break()**. **__builtin_trap()** also apparently marks the instructions after it as unreachable and thus may cause the compiler to optimize them away.

Notes
================================

gcc's [__builtin_trap()](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-g_t_005f_005fbuiltin_005ftrap-3278) generates an **ud2** instruction on i386 / x86-64:

[test/trap.c](https://github.com/scottt/debugbreak/blob/master/test/trap.c):
```C
#include <stdio.h>

int main()
{
	__builtin_trap();
	printf("hello world\n");
	return 0;
}
```

```
$ gdbdis test/trap main
   0x0000000000400390 <+0>:     0f 0b	ud2    
```
**debug_break()** generates an **int3** instruction on i386 / x86-64:

[test/break.c](https://github.com/scottt/debugbreak/blob/master/test/break.c):
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

```
$ gdbdis test/break main
   0x00000000004003d0 <+0>:     50	push   %rax
   0x00000000004003d1 <+1>:     cc	int3   
   0x00000000004003d2 <+2>:     bf a0 05 40 00	mov    $0x4005a0,%edi
   0x00000000004003d7 <+7>:     e8 d4 ff ff ff	callq  0x4003b0 <puts@plt>
   0x00000000004003dc <+12>:    31 c0	xor    %eax,%eax
   0x00000000004003de <+14>:    5a	pop    %rdx
   0x00000000004003df <+15>:    c3	retq   
```
Clang / LLVM also has a **__builtin_trap()** that generates **ud2** but further provides a  [__builtin_debugger()](http://lists.cs.uiuc.edu/pipermail/llvm-commits/Week-of-Mon-20120507/142621.html) that generates **int3** on i386 / x86-64.

