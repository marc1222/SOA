#include <libc.h>

char buff[24];

int pid;

int __attribute__ ((__section__(".text.main")))
  main(void)
{
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */

	//char buff[20];
	//int gettime();
	//itoa((int)gettime(),buff);
	//write(1,buff,strlen(buff));
	
	//runjp();
	//fork();
//	write(1,"hooala",7);
// int i = 0;
	//struct stats *st;
	//get_stats(1,st);

	while(1) {
//	fork();
//if (i == 100000000) {//write(1,"hooala",7);
	
//	unsigned long val = st->user_ticks;
//	itoa((int)gettime(),buff);
//	write(1,buff,strlen(buff));
//	st->user_ticks = 2;
//	get_stats(1,st);
//	val = st->user_ticks;
//	itoa((int)gettime(),buff);
//	write(1,buff,strlen(buff));
//	}
//i++;	
	}
}
