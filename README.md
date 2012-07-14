"debugbreak.h" allows you to put breakpoints in your sourcde code with:
<pre><code>
#include <stdio.h>
#include "debugbreak.h"

int main()
{
	debug_break();
	printf("hello world\n");
	return 0;
}
</code></pre>
Much like the DebugBreak() intrinsic provided by Windows and QNX headers.

Note that gcc's __builtin_trap() is unsatisfactory because "continue" in gdb
doesn't work even if you do:
<pre><code>
	(gdb) handle SIGILL stop nopass
	(gdb) file TEST-PROGRAM
	(gdb) run
	# breakpoint hit
	(gdb) continue
</code></pre>
while it works with debug_break().

NOTES:
gcc's __builtin_trap() generates an "ud2" instruction on i386 / x86-64:.
Clang / LLVM hasa  __builtin_debugger() that generates "int $0x03" instead of "ud2" on i386 / x86-64.
http://lists.cs.uiuc.edu/pipermail/llvm-commits/Week-of-Mon-20120507/142621.html
