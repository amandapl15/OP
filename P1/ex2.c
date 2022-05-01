#include <stdlib.h>
#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <unistd.h>	//Unix-like sys-calls read and write
#include <fcntl.h>	//Unix-like sys-calls open and close
#include "myutils.c"

int main(int argc, char *argv[]) {
  //printf("Hello World\n");
  int input;
  sscanf(argv[1], "%d", &input);
  int * random_array = (int*) malloc(input * sizeof(int));

  for(int i=0; i< input; i++){
    int random = rand() %100;
    random_array [i] = random;
  }
  printf("Random fillled array of %d elemnts created: ", input);
  for(int i=0; i< input; i++){
    printf("%d ", random_array [i]);
  }
  printf("\n");
  return 0;
}