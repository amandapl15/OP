//Antonio Pintado Lineros u172771
//Amanda Pintado Lineros u137702
#include <stdio.h>
#include <string.h>
#include <unistd.h>    // Unix-like system calls read and write
#include <fcntl.h>     // Unix-like system calls to open and close
#include <pthread.h> 
 
#include "myutils.h"
typedef struct args_struct{
   int i,j;
 } args;
#define R 4  // Constant indicating the image divisions RxR
#define N 1000
 
pthread_mutex_t lock;
pthread_mutex_t lockRegions[R*R];
 
monitor_lock monitorRegions[R*R];
 
enum { width=1024, height=1024 };
//enum { width=8192, height=8192 };
unsigned char pixels[width * height * 3];
 
int tga_write_header(int fd, int width, int height) {
 static unsigned char tga[18];
 int nbytes;
 tga[2] = 2;
 tga[12] = 255 & width;
 tga[13] = 255 & (width >> 8);
 tga[14] = 255 & height;
 tga[15] = 255 & (height >> 8);
 tga[16] = 24;
 tga[17] = 32;
 nbytes = write(fd, tga, sizeof(tga));
 return nbytes == sizeof(tga);
}
 
 
void write_tga(char* fname, unsigned char *pixels, int width, int height)
{
 int fd = open(fname,  O_CREAT | O_RDWR, 0640);
 tga_write_header(fd, width, height);
 printf("Created file %s: Writing pixels size %d bytes\n", fname, 3*width*height);
 write(fd, pixels,3*width*height);
 close(fd);
}
 
 
 
void tga_read_header(int fd, int* width, int* height) {
 static unsigned char tga[18];
 read(fd, tga, 12);
 read(fd, width, 2);
 read(fd, height, 2);
 read(fd, &tga[16], 2);
}
 
 
int compute_iter(int i, int j, int width, int height) {
 int itermax = 255/2; 
 int iter;
 double x,xx,y,cx,cy;
 cx = (((float)i)/((float)width)-0.5)/1.3*3.0-0.7;
 cy = (((float)j)/((float)height)-0.5)/1.3*3.0;
 x = 0.0; y = 0.0;
 for (iter=1;iter<itermax && x*x+y*y<itermax;iter++)  {
   xx = x*x-y*y+cx;
   y = 2.0*x*y+cy;
   x = xx;
 }
 return iter;
}
 
void* generate_mandelbrot_region(void* arg) { //funcion de region critica para el ejercicio 3, modificada para que se pueda llamar mediante threads (void *)
 int* args = (int*) arg;
 int ti = args[0];
 int tj = args[1];
 int k,l;
 int n = width / R;
 for (k=0;k<n;k++) {
   int t_index = ti*n*width + tj*n + k*width;
   int i = t_index / width;
   int j = t_index % width;
   unsigned char *p_region = &pixels[3*t_index];
   for (l=0; l<n; l++) {
     *p_region++ = 255 * ((float)(j+l) / height);
     *p_region++ = 255 * ((float)i / width);
     *p_region++ = 2 * compute_iter(i, (j+l), width, height);
   }
 }
 free(args);
 return NULL;
}
 
void generate_mandelbrot(unsigned char *p, int width, int height) {
 int i, j;
 for (i = 0; i < height; i++) {
   for (j = 0; j < width; j++) {
     *p++ = 255 * ((float)j / height);
     *p++ = 255 * ((float)i / width);
     *p++ = 2*compute_iter(i,j,width,height);
   }
 }
}
 
 
void interchange(int si, int sj, int ti, int tj, unsigned char *p, int width, int height) {
 int k;
 int n = width / R;
 unsigned char* square = malloc(n*n*3);
 memset(square, 0, n*n*3);
 
 for (k=0;k<n;k++) {
   int t_index = ti*n*3*width + tj*3*n + k*3*width;
   memcpy(&square[k*3*n], &p[t_index], n*3);
 }
 for (k=0;k<n;k++) {
   int s_index = si*n*3*width + sj*3*n + k*3*width;
   int t_index = ti*n*3*width + tj*3*n + k*3*width;
   memcpy(&p[t_index], &p[s_index], n*3);
 }
 for (k=0;k<n;k++) {
   int s_index = si*n*3*width + sj*3*n + k*3*width;
   memcpy(&p[s_index], &square[k*3*n], n*3);
 }
 
 free(square);
}
 
void* interchange4(void* arg) {
 //Si en el enunciado se pide 100 intercambios para cada thread, usar un for de 0 a 1000 llamando a interchange y generando 4 numeros nuevos aleatoriamente, este caso es para 1 para cada thread. (No entendemos el enunciado)
 int si,sj,ti,tj,lock1,lock2;
 si = rand()%R;
 sj = rand()%R;
 ti = rand()%R;
 tj = rand()%R;
 if(si*R+sj == ti*R+tj){ //Si se quiere hacer un intercambio de un source igual al target no realizamos la operacion (ya que no se haria nada y consumiria tiempo)
   return NULL;
 }
 if(si*R+sj < ti*R+tj){  //se ejecuta primero el indice menor, y creamos el indice lock1 y lock2
   lock1 = si*R+sj;
   lock2 = ti*R+tj;
 }else if (si*R+sj > ti*R+tj){
   lock2 = si*R+sj;
   lock1 = ti*R+tj;
 }
 //pthread_mutex_lock(&lockRegions[lock1]); //Para el ej 4.3
 //pthread_mutex_lock(&lockRegions[lock2]);
 mon_lock(&monitorRegions[lock1]); //para el ej 4.4
 mon_lock(&monitorRegions[lock2]);
//Se hace el intercambio llamando a la funcion original
 interchange(si, sj, ti, tj, pixels, width, height);  
 //pthread_mutex_unlock(&lockRegions[lock2]);
 //pthread_mutex_unlock(&lockRegions[lock1]);
 mon_unlock(&monitorRegions[lock2]);
 mon_unlock(&monitorRegions[lock1]);
 return NULL;
}
 
int main(void) {
 int si, sj, ti, tj;
//threads para el ejercicio 3
 pthread_t thids[R*R]; 
//lock global para el ej 4.2
 pthread_mutex_init(&lock, NULL);
//inicializacion para el ejercicio 4.3 inicializacion de los locks de regiones 
 for(int i=0;i<R*R;++i) { 
   pthread_mutex_init(&lockRegions[i], NULL);
 }
//para el ejercicio 4.4 inicializacion de los monitores para cada region, como en el anterior
 for(int i=0;i<R*R;++i) { 
   mon_lock_init(&monitorRegions[i]);
 }
 
 startTimer(0);
 int idThread = 0;
 args* arg;
 for(int i=0;i<R;++i) {
   for(int j=0;j<R;++j) {
//Pasamos como argumentos los indices i y j, que seran utiles para generar cada una de las regiones de la imagen final
     arg = malloc(sizeof(args)); 
     arg->i=i;
     arg->j=j;
//Se ejecutan los threads con la funcion generate modificada
     pthread_create(&thids[idThread], NULL, generate_mandelbrot_region, (void *) arg); 
     idThread++;
   }
 }
//Esperamos los threads del ejercicio 3
 for(int i=0;i<R*R;i++) { 
   pthread_join(thids[i], NULL);
 }
 printf("Time spent generate_mandelbrot: %ldms\n", endTimer(0));
 write_tga("image.tga", pixels, width, height);
 
 int i;
 startTimer(0);
//threads para el ej 4
 pthread_t thids2[N]; 
 for(i=0;i<N;i++) {
//Declarados globalmente (no hace falta pasarlos como argumento)
   /*int* newArgs = malloc(2*sizeof(int)); 
   newArgs[0] = width;
   newArgs[1] = height;*/
   pthread_create(&thids2[i], NULL, interchange4, NULL);
 }
//Esperamos a los threads del ejercicio 4
 for(int i=0;i<N;i++) { 
   pthread_join(thids2[i], NULL);
 }
 printf("Time spent during interchange: %ldms\n", endTimer(0));
 write_tga("image_scrambled.tga", pixels, width, height);
 
 return 0;
}