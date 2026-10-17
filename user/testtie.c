#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int sync_pipe[2];
  pipe(sync_pipe);

  int fixed_priority = 25;

  printf("Starting tie-breaking priority verification (priority %d)...\n", fixed_priority);

  for (int i = 1; i <= 3; i++) {
    int pid = fork();
    if (pid == 0) {
      setpriority(fixed_priority);

      // Block until released by parent
      char token;
      read(sync_pipe[0], &token, 1);
      close(sync_pipe[0]);
      close(sync_pipe[1]);

      volatile uint64 counter = 0;
      while (counter < 400000000ULL) {
        counter++;
      }

      printf("Child %d (pid %d, pri %d) finished.\n", i, getpid(), fixed_priority);
      exit(0);
    }
  }

  // Allow children to set priority and block on the pipe
  pause(10);

  // Release all 3 tied children simultaneously
  write(sync_pipe[1], "GO!", 3);
  close(sync_pipe[0]);
  close(sync_pipe[1]);

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  printf("All tied tests completed.\n");
  exit(0);
}
