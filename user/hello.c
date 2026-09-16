#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  // TODO: if an argument was given (argc = 2), greet by name.
  // TODO: otherwise, print the default greeting.

if (argc == 2){

printf("Hello, %p!", argv);

}
else{

printf("Hello, World!);

}

  exit(0);

}


