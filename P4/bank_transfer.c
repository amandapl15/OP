//Antonio Pintado Lineros u172771
//Amanda Pintado Lineros u137702
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <semaphore.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
 
#include "myutils.h"
 
int SOLUTION = 0; // Default implementation monitors, but reading from argv
char* str_id[2] = {"FILE_LOCK", "NAMED_SEMAPHORE"}; //En este caso
sem_t* named_mutex;
 
typedef struct struct_account {
  int id;
  int account_type;
  double balance;
} account;
 
#define N 10               // Number of Accounts
#define NProc 10           // Number of Chill Process
 
void do_something() {
  int n = 20;
  int sum = 0;
  for(int i=0;i<n;i++)
  for(int j=0;j<n;j++)
  for(int k=0;k<n;k++) sum++;
}
 
bool check(account* acc, double amount) {
  return acc->balance - amount >= 0;
}
 
bool withdraw(account* acc, double amount) {
  if(check(acc,amount))  {
    acc->balance -= amount;
    do_something();
    return true;
  }
  return false;
}
 
void deposit(account* acc, double amount) {
  acc->balance += amount;
  do_something() ;
}
 
 
bool transfer(int from, int to, double amount, int fd) {
  if(SOLUTION==0) {
   if(from > to) { //en caso de file locks, bloqueamos las cuentas que queramos transeferir, para que no haya deadlock, siempre bloqueamos primero la cuenta con numero mas pequeño
     while(file_lock(fd, to*sizeof(account), sizeof(account)) < 0);
 	 while(file_lock(fd, from*sizeof(account), sizeof(account)) < 0);
   }
   else {
     while(file_lock(fd, from*sizeof(account), sizeof(account)) < 0);
 	 while(file_lock(fd, to*sizeof(account), sizeof(account)) < 0);
   }
  } else if(SOLUTION==1) {
   sem_wait(named_mutex); //wait del named semaphore
  }
 
  // ******************************************
 account acc_to;
 account acc_from;
 lseek(fd, from*sizeof(account), SEEK_SET);
 read(fd, &acc_from, sizeof(account)); //con lseek y un read obtenemos la cuenta from, a la pondremos el dinero, ubicandola en acc_from
 lseek(fd, to*sizeof(account), SEEK_SET);
 read(fd, &acc_to, sizeof(account));//con lseek y un read obtenemos la cuenta to, a la cual realizamos la transferencia
  bool bDone = withdraw(&acc_from, amount); //en los dos siguiente pasos hacemos igual que en bank_threaded, es decir colocamos dinero en una cuenta y lo sacamos de otra, para finalmente una vez ya tenemos estas dos cuentas modificadas las escribimos en el fichero binario con lseeks y writes
 if(bDone) {
   deposit(&acc_to, amount);
   lseek(fd, to*sizeof(account), SEEK_SET);
   write(fd, &acc_to, sizeof(account));
   lseek(fd, from*sizeof(account), SEEK_SET);
   write(fd, &acc_from, sizeof(account));
 }
 // ******************************************

  if(SOLUTION==0) {
    if(from > to) {
     file_unlock(fd, from*sizeof(account), sizeof(account)); //cerramos los file_lock
 	    file_unlock(fd, to*sizeof(account), sizeof(account));
     }else{
     file_unlock(fd, to*sizeof(account), sizeof(account));
 	 file_unlock(fd, from*sizeof(account), sizeof(account));
   }
  } else if(SOLUTION==1) {
   sem_post(named_mutex); //hacemos un post (equivalente a signal) al named_semaphore
  }
  return bDone;
}
 
 
void tranferProc( int fd ) {
 int from = -1;
  int to = -1;
 //cada proceso hijo hara 1000 transferencias aleatorias, asi hacemos 10000 trasferencias en total, como pedian
  int iter = 1000;
  while(iter--) {
    from = -1;
    to = -1;
    while(from == to) {
      from = rand()%N;
      to = rand()%N;
    }
    transfer(from, to, rand()%50+1, fd);
  }
}
 
 
 
int main(int argc, char *argv[])
{
  if(argc > 1) SOLUTION = atoi(argv[1]);
 
 named_mutex = sem_open("named_mut", O_CREAT, 0600, 1);
 printf("%p\n", named_mutex);
//sem_unlink ("named_mutex"); // unlink previene que si no se ha cerrado en algun momento, no exista para siempre el lock, en este caso no lo usamos para que no hayan errores en la ejecucion (nos sucedia), pero para ello es importante dejar que acabe la ejecucion para conseguir cerrar el named semaphore y que no quede abierto.
 printf("SOLUTION: %s\n",str_id[SOLUTION]);
  
 int ret;
  startTimer(0);
 for(int i=0;i<NProc;i++) {
   ret = fork();
   //los hijos que creamos nos haran las transferencias, en este caso tendremos 10, y cada uno hará 1000 (es decir 10000 transferencias)
   if(ret == 0){
     int fbank = open("bank.dat",  O_RDWR, 0640); //Abrimos el fichero para cada proceso hijo, este paso es importante ya que puede tener un valor diferente en funcion del proceso que se este ejecutando
     if(fbank == -1) return 0;
     tranferProc(fbank);
     close(fbank);
     exit(0);
   }
 }
 //el proceso padre espera a que terminen los hijos
 printf("Father : waiting for childs...\n");
 for(int i = 0;i < NProc; i++) { 
   wait(NULL);
 }
 sem_close(named_mutex);
 printf("Done: %ld millis\n",endTimer(0));
 
  return 0;
}
 

