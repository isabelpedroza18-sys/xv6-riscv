#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define NUM_JOBS 3

int
main(int argc, char *argv[])
{
  int sync_pipe[2];
  pipe(sync_pipe);

  // Response time communication: each child reports its start tick
  int resp_pipes[NUM_JOBS][2];
  for (int i = 0; i < NUM_JOBS; i++) {
    pipe(resp_pipes[i]);
  }

  int priorities[NUM_JOBS] = {0, 10, 20};
  int pids[NUM_JOBS];
  int response_times[NUM_JOBS];
  int turnaround_times[NUM_JOBS];

  printf("Starting scheduler benchmark with %d processes...\n", NUM_JOBS);

  for (int i = 0; i < NUM_JOBS; i++) {
    pids[i] = fork();
    if (pids[i] == 0) {
      setpriority(priorities[i]);

      // Wait at the barrier until the parent releases all jobs
      char token;
      read(sync_pipe[0], &token, 1);
      close(sync_pipe[0]);
      close(sync_pipe[1]);

      // Record first scheduled tick (Response Time)
      int start_tick = uptime();
      write(resp_pipes[i][1], &start_tick, sizeof(int));
      close(resp_pipes[i][0]);
      close(resp_pipes[i][1]);

      // Fixed computational workload
      volatile uint64 count = 0;
      while (count < 600000000ULL) {
        count++;
      }

      exit(0);
    }
  }

  // Let children set priorities and block on the pipe
  pause(10);

  int release_time = uptime();

  // Release all children at the exact same tick
  write(sync_pipe[1], "GO!", 3);
  close(sync_pipe[0]);
  close(sync_pipe[1]);

  // Read first-execution tick from each child
  for (int i = 0; i < NUM_JOBS; i++) {
    int first_run = 0;
    read(resp_pipes[i][0], &first_run, sizeof(int));
    response_times[i] = first_run - release_time;
    close(resp_pipes[i][0]);
    close(resp_pipes[i][1]);
  }

  // Measure completion time (Turnaround Time)
  for (int i = 0; i < NUM_JOBS; i++) {
    wait(0);
    int finish_time = uptime();
    turnaround_times[i] = finish_time - release_time;
  }

  printf("\n--- Benchmark Results ---\n");
  int total_resp = 0;
  int total_ta = 0;
  for (int i = 0; i < NUM_JOBS; i++) {
    printf("Job %d (Priority %d): Response Time = %d ticks, Turnaround Time = %d ticks\n",
           i, priorities[i], response_times[i], turnaround_times[i]);
    total_resp += response_times[i];
    total_ta += turnaround_times[i];
  }

  printf("Average Response Time: %d ticks\n", total_resp / NUM_JOBS);
  printf("Average Turnaround Time: %d ticks\n", total_ta / NUM_JOBS);

  exit(0);
}
