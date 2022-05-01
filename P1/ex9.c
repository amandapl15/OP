#include <stdlib.h>
#include <stdio.h>      // standard C lib input output basic functions compatible with Windows
#include <unistd.h>	//Unix-like sys-calls read and write
#include <fcntl.h>	//Unix-like sys-calls open and close
#include "myutils.h"

int main(int argc, char *argv[]) {
  //printf("Hello World\n");
  startTimer(0);
   int input;
  sscanf(argv[1], "%d", &input);
  int * random_arrayA = (int*) malloc(input * sizeof(int));
  int * random_arrayB = (int*) malloc(input * sizeof(int));
  int fda = open ("nums.dat", O_CREAT|O_RDWR, 0644);
  int fdb = open ("nums.txt", O_CREAT|O_RDWR, 0644);
  int random;
  char buff[80];

  for(int i=0; i< input; i++){
    random = rand() %100;
    random_arrayA [i] = random;
    sprintf(buff, "%d ", random);
    write(fdb, buff, sizeof(buff));
  }
  write(fda, random_arrayA, sizeof(random_arrayA));
  
  char buffe[100];
  read (fda, buffe, sizeof(buff));
  printf("\nText file num.dat: ");
  for(int i=0; i< 100; i++){
    printf("%c ", buffe[i]);
  }
  char* ch_end;
  read_split(fdb, buff, sizeof(buff), ch_end);

  printf("Time: %ld\n", endTimer(0));
  return 0;
}