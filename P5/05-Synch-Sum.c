#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // for sleep function : waits for seconds
#include <time.h>    // for usleep : waits for microseconds
#include <sys/time.h>
struct timeval start;
void startTimer();
long endTimer();


#define NTHREADS 10
#define NSUMS 4

int sums[NSUMS];
pthread_mutex_t lock[4]; //Creamos 4 locks, uno para cada posicion del array
//pthread_mutex_t lock;

void* fsum(void * arg) {
  for(int i=0;i<500;i++) {
    pthread_mutex_lock(&lock[i%NSUMS]);//hacemos un lock en funcion de la posicion que queramos operar de nuestro array de ints, sera el de la posicion i%NSUMS
    //pthread_mutex_lock(&lock);
 	  sums[i%NSUMS] = sums[i%NSUMS] + 1;
    usleep(100+rand()%100);
    pthread_mutex_unlock(&lock[i%NSUMS]);//unlock
    //pthread_mutex_unlock(&lock);
  }
  return NULL;
}

int main(int argc, char *argv[])
{
  pthread_t tids[NTHREADS];
  //inicializamos los 4 locks
  for(int i=0; i<NTHREADS; i++) { 
    pthread_mutex_init(&lock[i], NULL);
  }

  startTimer();

  for(int i=0; i<NSUMS; i++) sums[i] = 0;


  for(int i=0; i<NTHREADS; i++) {
    printf("Creating thread %d\n", i);
    pthread_create(&tids[i], NULL, fsum, NULL);
  }

  for(int i=0; i<NTHREADS; i++) {
   pthread_join(tids[i], NULL);
  }

  printf("Sums are: ");
  for(int i=0; i<NSUMS; i++)  printf("%d ", sums[i]);
  printf("    computed in %ld millis\n", endTimer());
  
}


void startTimer() {
  gettimeofday(&start, NULL);
}

long endTimer() {
  long mtime, seconds, useconds;    
  struct timeval end;
  gettimeofday(&end, NULL);
  seconds  = end.tv_sec  - start.tv_sec;
  useconds = end.tv_usec - start.tv_usec;
  mtime = ((seconds) * 1000 + useconds / 1000.0) + 0.5;

  return mtime;
}