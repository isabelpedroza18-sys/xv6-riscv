#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/pstat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time <command> [args...]\n");
    exit(1);
  }

  struct rusage ru;
  int start_time = uptime();
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "time: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    exec(argv[1], &argv[1]);
    fprintf(2, "time: exec %s failed\n", argv[1]);
    exit(1);
  } else {
    int status;
    wait2(&status, &ru);
    int end_time = uptime();

    int elapsed = end_time - start_time;
    int cpu = ru.cputime;
    int pct = 0;
    if (elapsed > 0) {
      pct = (cpu * 100) / elapsed;
    }

    printf("elapsed time: %d ticks, cpu time: %d ticks, %d%% CPU\n",
           elapsed, cpu, pct);
  }

  exit(0);
}
