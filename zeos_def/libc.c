/*
 * libc.c 
 */

#include <types.h>
#include <error.h>
int errno;

void nitoa(int a, char *b)
{
  int i, i1;
  char c;
	for(i = 0; i < 50; i++) b[i] = 0;
  if (a==0) { b[0]='0'; b[1]=0; return ;}

	i = 0;
	int aux = a;
	while(aux>0){
		aux=aux/10;
		i++;
	}
	//i--;
	while(i>=0){
    		b[i]=(a%10)+'0';
		a=a/10;
		i--;
	}
}


void itoa(int a, char *b)
{
  
}

int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}


void perror()
{
 	//write(1, "ERROR MSG: ", 11);
	//write(1, errmsg[errno-1], strlen(errmsg[errno-1]));
	//write(1,"\n",1);
}

