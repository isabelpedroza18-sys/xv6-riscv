#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  int sync_pipe[2];
  pipe(sync_pipe);

  int priorities[3] = {10, 20, 30};

  printf("Starting synchronized priority verification...\n");

  for (int i = 0; i < 3; i++) {
    int pid = fork();
    if (pid == 0) {
      setpriority(priorities[i]);

      // Block until the parent writes the start token
      char start_signal;
      read(sync_pipe[0], &start_signal, 1);
      close(sync_pipe[0]);
      close(sync_pipe[1]);

      volatile uint64 counter = 0;
      while (counter < 500000000ULL) {
        counter++;
      }

      printf("Child with priority %d finished.\n", priorities[i]);
      exit(0);
    }
  }

  // Ensure all 3 children are sleeping on the pipe with priorities set
  pause(10);

  // Release all 3 children simultaneously
  write(sync_pipe[1], "GO!", 3);
  close(sync_pipe[0]);
  close(sync_pipe[1]);

  for (int i = 0; i < 3; i++) {
    wait(0);
  }

  printf("All priority tests completed.\n");
  exit(0);
}
