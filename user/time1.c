#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  if (argc < 2) {
    fprintf(2, "Usage: time1 <command> [args...]\n");
    exit(1);
  }

  int start_time = uptime();
  int pid = fork();

  if (pid < 0) {
    fprintf(2, "time1: fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    // Child: execute the target command with its arguments
    exec(argv[1], &argv[1]);
    fprintf(2, "time1: exec %s failed\n", argv[1]);
    exit(1);
  } else {
    // Parent: wait for child process to terminate, then read clock ticks
    wait(0);
    int end_time = uptime();
    printf("elapsed time: %d ticks\n", end_time - start_time);
  }

  exit(0);
}
