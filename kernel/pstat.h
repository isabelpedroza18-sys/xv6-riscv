#ifndef _PSTAT_H_
#define _PSTAT_H_

#include "param.h"
#include "types.h"

// Define the process states so both kernel and ps.c can see them
enum procstate { UNUSED, USED, SLEEPING, RUNNABLE, RUNNING, ZOMBIE };

struct pstat {
   int pid;     // Process ID
    enum procstate state;  // Process state
    uint64 size;     // Size of process memory (bytes)
    int ppid;        // Parent process ID
    char name[16];   // Parent command name
    int priority;    // Process priority
    int readytime;   // Ticks spent in RUNNABLE state
};
struct rusage {
  uint cputime;
};

#endif
