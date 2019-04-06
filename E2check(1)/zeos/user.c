#include <libc.h>

/*int pid;
char buffer[42];
char buffer2[42];
volatile int b = 1;*/


/*void prueba()
{
  b = 0;
  write(1, "ey", 2);
  sem_wait(0);
  write(1, "hola", 4);
  exit();
}*/

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
  //runjp_rank(0, 32);
  //while (1) {
  /*volatile int s = 0;
  sem_init(0, 0);
  clone(&prueba, &buffer[42]);
  while (b) {}
  sem_destroy(0);
  write(1, "killd", 5);*/
  
  runjp();
  while(1) {
  }
}
