//Antonio Pintado Lineros u172771
//Amanda Pintado Lineros u137702
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <semaphore.h>
#include <unistd.h>     // unix-like system calls read and write
#include "myutils.h"

#define bank_sum 1000      //Bank total sum
#define N 10               // Number of Accounts

typedef struct struct_account {
	int id;
	int account_type;
	double balance;
} account;

int main(int argc, char *argv[])
{
  int fbank = open("bank.dat",  O_RDWR, 0640);

  if(fbank == -1) return 0;
  double total_sum = bank_sum;   //aqui se acumula la suma de los balance que iremos recogiendo de las cuentas
  account actual_account;
  sem_t* named_mutex;
  named_mutex = sem_open("named_mut", O_CREAT, 0600, 1);
  sem_unlink ("named_mutex"); // unlink previene que si no se ha cerrado en algun momento, no exista para siempre el lock, esto es util ya que no siempre fallara nuestro programa y no se cerrara en todos los casos
  while(total_sum == bank_sum){ //mientras que la suma total recogida sea 1000 seguimos comprobando sumando los balance totales, hasta que esto no se cumpla
    sem_wait(named_mutex); //lock del named semaphore
    /////////////////////////////////////////////////////////////////////
    total_sum = 0;
    lseek(fbank, 0, SEEK_SET); //ubicamos el pointer de lectura al inicio del fichero .dat
    for(int i=0; i<N; i++){
        while(file_lock(fbank, i*sizeof(account), sizeof(account)) < 0); //se van cerrando las cuentas mientras se van comprobando, esto impide que hayan transferencias en cuentas visitadas anteriormente y el precio total no varie
        read(fbank, &actual_account, sizeof(account));
        total_sum += actual_account.balance;
    }
    printf("Suma total obtenida: %f\n", total_sum);
    for(int i=0; i<N; i++) file_unlock(fbank, i*sizeof(account), sizeof(account)); //unlock de todo el file lock, para dejar libre todas las cuentas para el bank_threaded
    /////////////////////////////////////////////////////////////////////
    sem_post(named_mutex); //unlock del named semaphore
    usleep(500000); //Equivalente a 0.5 segundos
  }
  sem_close(named_mutex);
  printf("ERROR, la suma total no es 1000\n");
}