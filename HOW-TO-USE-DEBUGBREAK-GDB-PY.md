# How to use `debugbreak-gdb.py`

Just add `-x debugbreak-gdb.py` to your usual GDB invocation or add `source debugbreak-gdb.py` to your `$HOME/.gdbinit`.

Here's a sample session:

```
$ cd debugbreak
$ make
$ gdb -q -x debugbreak-gdb.py test/break
Reading symbols from test/break...done.

(gdb) set disassemble-next-line 1
(gdb) run
Starting program: /home/fedora/debugbreak/test/break 

Program received signal SIGTRAP, Trace/breakpoint trap.
main () at test/break.c:6
6		debug_break();
(gdb) stepi
0x000000001000048c in trap_instruction () at ./debugbreak.h:131
131		trap_instruction();
   0x0000000010000480 <main+0>:	7c 08 02 a6	mflr    r0
   0x0000000010000484 <main+4>:	f8 01 00 10	std     r0,16(r1)
   0x0000000010000488 <main+8>:	f8 21 ff 91	stdu    r1,-112(r1)
=> 0x000000001000048c <main+12>:	7d 82 10 08	twge    r2,r2

(gdb) debugbreak-step

(gdb) debugbreak-continue

```
