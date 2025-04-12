# Bug Report
## Problem:
When the command `ls   ps` is entered, operation is as expected.  
When the command `ls` is entered next, `ls` prints and `ps` prints.
## Steps Solving the Problem
Turning on debug, in our first pass we correctly parse the argument vector as
follows:
```
Processed argv:
arg00: ls
arg01: ps
Parsed argv.
Parent path entered
Child path entered
ls: cannot access 'ps': No such file or directory
Child 10654, exited with 2
```
Notice that our argument vector for exec looks like `{"ls", "ps"}`.  
Everything occurs as expected.  
  
The problem is on our next pass:
```
Processed argv:
arg00: ls
Parsed argv.
Parent path entered
Child path entered
baxter_tyler_HW3_global.h     baxter_tyler_HW3_main.c	baxter_tyler_HW3_shell.h  baxter_tyler_HW3_test.h	commands.txt
baxter_tyler_HW3_int_shell.h  baxter_tyler_HW3_main.o	baxter_tyler_HW3_shell.o  baxter_tyler_HW3_test.o	Makefile
baxter_tyler_HW3_main	     baxter_tyler_HW3_shell.c	baxter_tyler_HW3_test.c   baxter_tyler_HW3_writeup.pdf	README.md
Child 10655, exited with 0
```
Which is correct. However, we now have a hanging command we also `exec`. As
follows:
```
Add argument path entered.
XXX: ps
Processed argv:
arg00: ps
Parsed argv.
Parent path entered
Child path entered
  PID TTY          TIME CMD
10652 pts/44   00:00:00 make
10653 pts/44   00:00:00 baxter_tyler_HW
10656 pts/44   00:00:00 ps
32211 pts/44   00:00:23 zsh
32226 pts/44   00:00:00 zsh
32308 pts/44   00:00:00 zsh
32310 pts/44   00:00:00 zsh
32311 pts/44   00:00:00 gitstatusd-linu
Child 10656, exited with 0
```
This consistently happens, not just with the command combination described, but any
combination of `ls ps` (notice only one space now). We assume there's likely more 
commands that don't cleanly execute either. So what's causing this?  
  
There's a couple areas that are immediately likely culprits:
 * Hanging-on to bad memory.
 * Non-zero exit codes.

The first assumption is non-zero exit codes, as this code was passed though ASAN
and valgrind extensively. That doesn't mean that our way of managing our buffers
isn't incorrect though. Recall: We re-use the buffer, so getting caught on a bad
index could definitely be causing this.  
(**Spoiler:** It was the buffer.)  
  
That said, non-zero exit codes really feels like the problem, since we mainly
tested with successful commands. We write a quick unit test to debug this specific
command sequence: `TEST_TwoCommands()`.  
  
Output from `TEST_TwoCommands()` looks as follows:
```
Processed argv:
arg00: ls
arg01: ps
Processed int_argv1:
int_argv1[0]:	ls
int_argv1[1]:	ps
Processed ext_argv1:
ext_argv1[0]:	ls
ext_argv1[1]:	ps

Processed argv:
arg00: ls
Processed int_argv2:
int_argv2[0]:	ls
Processed ext_argv2:
ext_argv2[0]:	ls
```
And execution occurs as we expect. It's unlikely to be our curruent system in an
isolated box, so it must be how the different systems play together on a whole.  
  
**Note:** We know that we're avoiding the recursive loop for piped commands
entirely (that was a problem point), so we can focus solely on the simple `exec`
routine.  
  
Expected control flow is:
 * `SH_Buf` creates a buffer iterator. In this case we have a single line, so it
   simply returns that single line.
 * We enter our iterator loop with only one execution.
 * We `SH_Exec` our command, which forwards the command to our internal API. 
 * We destroy that argv subset of the iterator.
 * We destroy the iterator itself.

Switching back to debug and off test, we'll confirm that's the behaviour we are
getting.
```main:	SH_Buf iterator:	ls ps
main:	shargv[0]:	ls
main:	shargv[1]:	ps
Parent path entered
Child path entered
ls: cannot access 'ps': No such file or directory
Child 15822, exited with 2
main:	SH_Buf iterator:	Exited
```
That's expected.
```main:	SH_Buf iterator:	ls
�%V
main:	shargv[0]:	ls
Parent path entered
Child path entered
baxter_tyler_HW3_global.h     baxter_tyler_HW3_main.o	baxter_tyler_HW3_test.c       commands.txt
baxter_tyler_HW3_int_shell.h  baxter_tyler_HW3_shell.c	baxter_tyler_HW3_test.h       Makefile
baxter_tyler_HW3_main	     baxter_tyler_HW3_shell.h	baxter_tyler_HW3_test.o       README.md
baxter_tyler_HW3_main.c       baxter_tyler_HW3_shell.o	baxter_tyler_HW3_writeup.pdf  writeup.txt
Child 15959, exited with 0
IDX: 112
IDX: 115
IDX: 10
Add argument path entered.
XXX: ps
Processed argv:
arg00: ps
Parsed argv.
main:	SH_Buf iterator:	ps
�%V
```
That's not expected, and we see clear memory faults.  
  
Back to our iterator unit test, `TEST_SHBuf`. Let's create a new unit test for
this condition (without pipes and multiple newlines), so we can figure out
what's going wrong: `TEST_SHBufBasic()`.  
  
That's not good:
```
TEST_SHBufBasic1: cmd1 string: ls    ps
�
TEST_SHBufBasic2: cmd2 string: ls
SH_BufCreate: Found newline.
TEST_SHBufBasic1: Successful iteration loop.
TEST_SHBufBasic1: Buffer line: ls    ps
SH_BufCreate: Found newline.
TEST_SHBufBasic2: Successful iteration loop.
TEST_SHBufBasic2: Buffer line: ls
\
```
Oh, we left an interesting comment in `SH_Buf`:
```
// WARNING: If we newline-terminate we buffer overflow
// in our test cases; if we NULL-terminate we buffer
// overflow in our main driver code. What we should do
// is newline-terminate then NULL-terminate, but main
// driver code is prioritized for now.
cbuf[aidx - bidx] = '\n';
```
**Note**: A partial solution was do just that. I tested it and it was not the
solution to this problem though. I left that out of any diffs introduced by this
write-up, because we're not trying to solve multiple problems here -- also, we
might break more of the API if we half-hazardly address that now (hence, why I
didn't in my final submission).  
  
Well, that explains the garbage on `printf()`. Recall: `printf()` expects a
NULL-terminated string. That doesn't explain our bad buffer though.  
**Note:** This is correct, right? It's what the internet seems to think, but
it's not in the `printf()` man pages. I'm assuming it's in the `%s` man pages 
(and moving on for now).  
  
Setting aside the garbage from that, we know from this test that with two
separate iterator buffers we don't have a hanging command. That means the issue
has to be from reusing an iterator buffer.  

Let's confirm we're entering our destruction routine properly:
```
SH_BufDestroy:	Entered
SH_BufDestroy:	free:	ls ps
SH_BufDestroy:	free:	
SH_BufDestroy:	Destroyed
```
We should not be entering free a second time. (**Note**: This seems to actually
be fine, but I don't have a good reason why -- we should fail the `NULL` check and
not proceed. Maybe due to how we're creating our iterator buffer, which is a
separate issue to look into.) Also after clearing, we had another strange interaction:
```
main:	SH_Buf iterator:	Exited
SH_BufDestroy:	Entered
SH_BufDestroy:	free:	clear
SH_BufDestroy:	free:	
SH_BufDestroy:	Destroyed
Prompt> ls
...
main:	SH_Buf iterator:	ar
p�U
main:	shargv[0]:	ar
Parent path entered
Child path entered
Usage: ar [emulation options] [-]{dmpqrstx}[abcDfilMNoOPsSTuvV] [--plugin <name>] [member-name] [count] archive-file file...
       ar -M [<mri-script]
```
Definitely some memory issues with the iterator.

## Issue Identified:
Our iterator is failing in the simple case where we do not have a multi-newline command.  
  
*Why did this make it through our test coverage?*  
Because we only tested the complex case and assumed our implementation was correct if it worked for the
complex case. The iterator was implemented late into the implementation, so
previous assumptions were assumed to be correct, although they very cleary are
not.  
  
At this point, I also think I know what it is. Since we re-use our buffer with
each line cycle, and our buffer iterator traverses looking for newlines as the
delimiter, we end up parsing previous commands into our next command. To
illustrate:
```
First command:	ls ps\n
Buffer:			ls ps\n
Iterator:		ls ps\n

Next command:	ls\n
Buffer:			ls\nps\n
Iterator:		ls, ps
```

There's three immediate solutions that come to mind:
 * `malloc(3)` a new buffer (or `calloc(3)` for a guarantee),
 * `memset(3)` and zero-out the old buffer,
 * pass a size around (e.g., `bytes`).

The best solution is to pass a size around. We already have it from
`SH_Prompt()` (wrapper for `read(2)`), and we'd be saving memory allocations (an
extremely expensive operation).  
  
The solution we're going to choose though, to confirm this is in fact the bug
correctly identified, and also not change the API and break the entire shell
project for the purposes of this write-up, is to `memset(3)` the buffer.  
  
Yes, this works (we'll take debug off to simplify output):
```
./baxter_tyler_HW3_main "Prompt> "
Prompt> ls    ps 
ls: cannot access 'ps': No such file or directory
Child 26243, exited with 2
Prompt> ls
baxter_tyler_HW3_global.h     baxter_tyler_HW3_main.o	baxter_tyler_HW3_test.c       commands.txt
baxter_tyler_HW3_int_shell.h  baxter_tyler_HW3_shell.c	baxter_tyler_HW3_test.h       Makefile
baxter_tyler_HW3_main	     baxter_tyler_HW3_shell.h	baxter_tyler_HW3_test.o       README.md
baxter_tyler_HW3_main.c       baxter_tyler_HW3_shell.o	baxter_tyler_HW3_writeup.pdf  writeup.txt
Child 26244, exited with 0
Prompt>
```

## Recap
 * The problem was that we were reusing our buffer. This is not fundamentally a
   problem, but what was happening was we were only splitting our iterator via
   newlines and we were leaving newlines in the buffer.
 * After entering a larger command, every shorter command would have whatever
   length of commands from the larger command added to it.
 * This is because we're still splitting by newlines, and there's more newlines
   in the buffer.
 * Our solution was to `memset(3)` and zero-out the buffer on each loop.
 * The better solution is change the API and pass the amount of bytes we read to
   our iterator. That way, we'll never overflow any commands left in the buffer
   past our current command.
