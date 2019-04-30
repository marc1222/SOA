#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	//crear procesos
	fork();
	fork();

	//set policy
	set_sched_policy(0);  //RR
	//set_sched_policy(1); //FCFS
	int a=0,b=0,c=0;
	while(1) {
		//begin JP
		a += 3;
		b++;
		c = c+1;
	}
}
