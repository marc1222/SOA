#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

    
    	int ret = write(1,"Write working\n",20);
	char *buff = "";
	itoa(ret,buff);
	write(1,buff,2);
	//write(1,"\n",1);
	//write(1,"\n",1);
  while(1) { }
}
