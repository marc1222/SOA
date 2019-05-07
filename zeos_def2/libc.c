/*
 * libc.c 
 */

#include <types.h>
#include <error.h>
int errno;


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

