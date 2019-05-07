#include <libc.h>

char buff[2048];

int pid;
int matrixN;

void bitoa(int a, char *b)
{
  int i, i1;
  char c;
	for(i = 0; i < 50; i++) b[i] = 0;
  if (a==0) { b[0]='0'; b[1]=0; return ;}

	i = 0;
	int aux = a;
	//write(1, "(bitoa: ", 8);
	while(aux>0){
		//char c[1];
		//c[0]= aux%10 + '0';
		//write(1, c, 1);
		aux=aux/10;
		i++;
	}
	i--;
	while(i>=0){
    		b[i]=(a%10)+'0';
		a=a/10;
		i--;
	}
	//write(1, ") ", 3);
}


int __attribute__ ((__section__(".text.main")))
  main(void)
{
	set_sched_policy(0);  //RR
    /* Next line, tries to move value 0 to CR3 register. This register is a privileged one, and so it will raise an exception */
     /* __asm__ __volatile__ ("mov %0, %%cr3"::"r" (0) ); */
	//crear procesos
	//set_sched_policy(0);  //RR
	fork();
	fork();
	
	//set policy

	//set_sched_policy(1); //FCFS
	int a, b, c;
	matrixN = 500;
	int n = 2;
	for(a = 0; a < matrixN; a++) {
		for(b = 0; b <matrixN; b++) {
			for(c = 0; c<matrixN; c++) {
				if (c%3 == 0) n += 2;
				else if (c%3 == 1) n-= 2;
				else n+=1;
			}
		}	
	}

	bitoa(n, buff);
	write(1, buff, strlen(buff));
	write(1, "\n", 1);
	struct stats aux2;
	get_stats(getpid(), &aux2);
	write(1, "(user)PID: ", 11);
	bitoa(getpid(),buff);
	write(1, buff, strlen(buff));
	write(1, "\nstats: ", 8);
	
	bitoa(aux2.user_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, " ", 1);

	bitoa(aux2.system_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, " ", 1);

	bitoa(aux2.blocked_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, " ", 1);

	bitoa(aux2.ready_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, " ", 1);

	bitoa(aux2.elapsed_total_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, " ", 1);

	bitoa(aux2.remaining_ticks,buff);
	write(1, buff, strlen(buff));
	write(1, "\n\n", 2);	

	read(0, buff, 200);

	while(1) {
	}

}
