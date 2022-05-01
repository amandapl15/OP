#include <stdlib.h>
#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <unistd.h>	//Unix-like sys-calls read and write
#include <fcntl.h>	//Unix-like sys-calls open and close
#include "myutils.h"

int main(int argc, char *argv[]) {
  //printf("Hello World\n");
  int * a = malloc(10*sizeof(int));
  printf("%p", a);
  a++;
  printf("\n%p", a);
  char buff[10] = "hOLS";
  read(0, buff, sizeof(char));
}