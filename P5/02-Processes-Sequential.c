#include <stdio.h>       /* printf, sprintf */
#include <stdlib.h>
#include <string.h>
#include <unistd.h>      /* fork, _exit */
#include <sys/wait.h>    /* wait */


int main(int argc, char *argv[])
{
    for (int i=0; i<10; i++) {
    	if(fork() == 0) {
    		usleep(10000+rand()%10000);
    		printf("I'm process %d\n",i);
    		_exit(0);
    	}
      wait(NULL);
    }
    //while (wait(NULL) > 0);
    return 0;
}

