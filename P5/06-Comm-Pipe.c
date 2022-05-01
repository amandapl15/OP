#include <stdio.h>       /* printf, sprintf */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */
#include <sys/types.h>


int main(int argc, char *argv[])
{
  int fd[2];
  pipe(fd);
    for (int i=0; i<10; i++) {
    	if(fork() == 0) {
        //Cerramos la posicion 1 que es para escritura, ya que el hijo no va a usarlo
        close(fd[1]);
        int id;
        read(fd[0], &id, sizeof(int));
    		usleep(10000+rand()%10000);
    		printf("I'm process %d\n",id);
    		_exit(0);
    	}
    }
    //Cerramos la posicion 0 que es para lectura, el padre solo va ha escribir
    close(fd[0]);
    for (int i=0; i<10; i++) {
      write(fd[1], &i, sizeof(int));
    }
    
    while (wait(NULL) > 0);
    return 0;
}

