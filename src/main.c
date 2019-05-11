/* Contador de números primos
 *
 * Este programa recebera uma serie de caracteres representando números em sua
 * entrada. Ao receber um caractere fim de linha ('\n'), deve imprimir na tela a
 * quantidade de números primos que recebeu e, após, encerrar.
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>


#define WRITE 1
#define READ  0
#define PROCESSES 3

int prime(unsigned long n);

int main(void) {

  int processes[2]; /* Pipe declaration */
  int *b; /* Shared memory to save final result */

  /* Protection flags for allocating memory */
  int protection = PROT_READ | PROT_WRITE;
  int visibility = MAP_SHARED | MAP_ANON;

  pid_t pid[PROCESSES];

  b = (int*) mmap(NULL, sizeof(int), protection, visibility, 0, 0);
  if ((long int)b==-1) {
    fprintf(stderr, "Failed to allocate shared memory!\n");
    return 1;
  }
  (*b)=0;

  if(pipe(processes)==-1) {
    fprintf(stderr, "Pipe failed!\n");
    return 1;
  }

  for(int i=0 ; i<PROCESSES ; i++) {
    pid[i] = fork();

    if(pid[i]<0) {
      fprintf(stderr,"Fork failed!\n");
      return 1;
    } else if(pid[i]==0) {
      /* Child process routine */
      close(processes[WRITE]);
      unsigned long u;
      while(read(processes[READ], &u, sizeof(unsigned long))>0){
        if(prime(u)) (*b)++;
      }
      return 0;
    }
  }  
  /* Main process routine */
  close(processes[READ]);
  unsigned long u;
  while(scanf(" %lu", &u)!= -1) {
    write(processes[WRITE], &u, sizeof(unsigned long));
  }
  close(processes[WRITE]);
  for(int i=0 ; i<PROCESSES ; i++) {
    waitpid(pid[i], NULL, 0);
  }
  printf("%d\n", (*b));
  return 0;
}

int prime(unsigned long n) {
  unsigned d, isPrime;

  d = 2;
  isPrime = 1;		/* Assume that n is prime at first */

  if (n <= 1)
    isPrime = 0;
  while (isPrime == 1 && d <= n / 2) {
    if (n % d  == 0)
      isPrime = 0;
    d = d + 1;
  }

  return isPrime;
}
