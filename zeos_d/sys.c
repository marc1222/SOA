/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#include <errno.h>

#include <system.h>


#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
}
int strlen(char *a)
{
  int i;
  
  i=0;
  
  while (a[i]!=0) i++;
  
  return i;
}

int sys_ni_syscall()
{
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
	return current()->PID;
}

int sys_fork()
{
  int PID=-1;

  // creates the child process
  
  return PID;
}

void sys_exit()
{  
}
#define MAX_BUFFER_SIZE 256
int sys_write( int fd, char* buff, int size ) {
	char buff_aux[MAX_BUFFER_SIZE];
	int res = check_fd(fd,ESCRIPTURA);
	if (res != 0) 
		return res;
	if (buff == NULL)
		return -EINVAL;
	if (size < 0)
		return -EINVAL;

	//if (strlen(buff) < size) size = strlen(buff);
	int left = size; 
	while (left > MAX_BUFFER_SIZE) {
		copy_from_user(buff, buff_aux, MAX_BUFFER_SIZE);
		res = sys_write_console(buff_aux, MAX_BUFFER_SIZE);
		left -= res;
		buff += res;

	}
	if (left > 0) {
		copy_from_user(buff, buff_aux,left);
		res = sys_write_console(buff_aux, left);
		left -= res;
	}
	return (size-left);
	
}
int sys_gettime()
{
	return zeos_ticks;
}
