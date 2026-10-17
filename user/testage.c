#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  printf("Starting CPU-bound children to test age...\n");

  for(int i = 0; i < 2; i++){
    int pid = fork();
    if(pid == 0){
      volatile uint64 count = 0;
      while(count < 500000000) {
        count++;
      }
      exit(0);
    }
  }

  pause(10);

  printf("Running ps to check runnable age:\n");
  char *args[] = { "ps", 0 };
  exec("ps", args);

  exit(0);
}
