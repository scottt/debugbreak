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

Notes
================================

The requirements for the **debug_break()** function are:
* Act as an compiler code motion barrier
* Doesn't cause the compiler to think the code following it can't be reached
* Trigger a software breakpoint hit event when executed (i.e. causes **SIGTRAP** on Linux)
* After a breakpoint hit, support using **continue**, **next**, **step**, **stepi** etc commands in GDB to continue execution.

Ideally, both gcc and Clang would provide a **__builtin_debugger()** builtin function for all supported architectures that satisfies the above.
Unfortunately, we're not there yet.
gcc's [__builtin_trap()](http://gcc.gnu.org/onlinedocs/gcc/Other-Builtins.html#index-g_t_005f_005fbuiltin_005ftrap-3278) causes the optimizer to think the code follwing it  can't be reached.
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
On i386 / x86-64, **__builtin_trap()** generates an **ud2** instruction which triggers **SIGILL** instead of **SIGTRAP**.
This makes it necessary to change GDB's default behavior on SIGILL to not terminate the inferior:
```
	(gdb) handle SIGILL stop nopass
```
Even after this, stepping doesn't work on some GDB versions.

**debug_break()** generates an **int3** instruction on i386 / x86-64.

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
Stepping works well after a breakpoint hit.
Clang / LLVM also has a **__builtin_trap()** that generates **ud2** but further provides [__builtin_debugger()](http://lists.cs.uiuc.edu/pipermail/llvm-commits/Week-of-Mon-20120507/142621.html) that generates **int3** on i386 / x86-64.

On ARMv7, **debug_break()** generates the undefined instruction **.inst 0xe7f001f0** in ARM mode
(as opposed to Thumb mode) which triggers SIGTRAP on recent Linux kernels.
Unfortunately, stepping afterwards doesn't work and requires a workaround like:
```
(gdb) tbreak *($pc + 4)
(gdb) jump   *($pc + 4)
```
to jump over the four byte instruction.
The included [debugbreak-gdb.py](https://github.com/scottt/debugbreak/blob/master/debugbreak.h) script adds a **debugbreak-stepi** GDB command that would automate the above.
```
$ arm-none-linux-gnueabi-gdb -x debugbreak-gdb.py test/break-c++
<...>
(gdb) run
Program received signal SIGTRAP, Trace/breakpoint trap.
main () at test/break-c++.cc:6
6		debug_break();
debugbreak-stepi  define            delete            detach            
(gdb) debugbreak-stepi 
Temporary breakpoint 1 at 0x8504: file test/break-c++.cc, line 7.

Temporary breakpoint 1, main () at test/break-c++.cc:7
7		std::cout << "hello, world\n";
```
```
