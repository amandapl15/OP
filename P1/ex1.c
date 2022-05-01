#include <stdlib.h>
#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <unistd.h>	//Unix-like sys-calls read and write
#include <fcntl.h>	//Unix-like sys-calls open and close
#include "myutils.c"

int main(int argc, char *argv[]) {
  //printf("Hello World\n");
  int input;
  sscanf(argv[1], "%d", &input);
  printf("Input recieved: %d\n", input);
  return 0;
}