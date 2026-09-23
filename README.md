# Programming Assignment 1: xv6 System Calls

**Name:** Maria Isabel Pedroza  
**Repository:** isabelpedroza18-sys/xv6-riscv[cite: 1]  
**Branch:** pa1-syscalls[cite: 1]  

---

## Task 6: Report

### What I Learned
* **How CPU time is tracked:** I learned that an operating system measures how long a process runs by using timer interrupts[cite: 1]. Every time the hardware timer fires (`which_dev == 2`), the kernel runs code in `kernel/trap.c`[cite: 1]. By incrementing a counter (`cputime++`) inside both `usertrap()` and `kerneltrap()` before yielding, the kernel records how many clock ticks that specific process spent on the CPU[cite: 1].
* **How system calls connect user space to kernel space:** Adding `wait2()` taught me the full path of a system call[cite: 1]:
  1. Adding the user-level prototype in `user/user.h` and the entry in `user/usys.pl` so the shell can call it[cite: 1].
  2. Giving it a syscall number in `kernel/syscall.h` and mapping that number to a function pointer in `kernel/syscall.c`[cite: 1].
  3. Declaring the kernel-side prototype in `kernel/defs.h`[cite: 1].
  4. Writing `sys_wait2()` in `kernel/sysproc.c` to grab pointer arguments from user registers with `argaddr()`[cite: 1].
  5. Writing `kwait2()` in `kernel/proc.c` to safely copy kernel data into user memory using `copyout()`[cite: 1].
* **Process synchronization:** I learned how parent and child processes coordinate when a child finishes[cite: 1]. The parent process has to sleep while waiting for children, check for zombie processes, acquire locks so data isn't corrupted, and clean up the process using `freeproc()`[cite: 1].

### Difficulties Encountered & How I Solved Them
* **Syscall Number Conflict:** The assignment instructions suggested using number 22 for `SYS_wait2`, but my repository already had `#define SYS_sync 22`[cite: 1]. I resolved this by checking the table in `kernel/syscall.h` and setting `SYS_wait2` to 23 so it wouldn't collide with existing calls[cite: 1].
* **Function Signature Differences in `copyout`:** When writing `kwait2()`, I got a compiler error because my kernel's `copyout()` required 5 arguments (`p->pagetable, p->sz, addr, src, len`) instead of the standard 4[cite: 1]. I checked the existing `kwait()` function in `kernel/proc.c` and updated `copyout()` to match the 5-parameter version[cite: 1].
* **Syntax and Indentation Error in `kerneltrap()`:** While adding `myproc()->cputime++` inside `kernel/trap.c`, I accidentally mismatched the braces around the `if` check[cite: 1]. This caused GCC to throw a `-Werror=misleading-indentation` error and fail the build. Adding explicit curly braces `{ ... }` around both statements inside the `if` block fixed the issue[cite: 1].

---

## Extra Credit: Limitations of CPU Time Accounting

1. **Tick Granularity:** xv6 timer interrupts occur at fixed intervals (~100 times per second, or about 10 ms per tick). If a process starts, does a quick calculation, and exits completely between two timer ticks, its `cputime` will show up as 0 ticks even though it used CPU cycles.
2. **Sampling Inaccuracy:** The kernel only checks which process is running at the exact instant the timer interrupt fires. If Process A runs for 9 milliseconds and yields right before the tick, it gets credited with 0 ticks. If Process B runs for just 1 millisecond and gets interrupted by the tick, it gets charged a full tick. This makes the accounting an approximation rather than an exact measurement.
3. **Kernel and Scheduling Overhead:** Time spent switching contexts in assembly (`swtch.S`), running the scheduler loop, or handling other hardware interrupts isn't accurately attributed to processes, which can cause slight differences between wall-clock elapsed time and measured CPU time.
