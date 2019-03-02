/*
 * sys.c - Syscalls implementation
 */
#include <devices.h>

#include <utils.h>

#include <io.h>

#include <mm.h>

#include <mm_address.h>

#include <sched.h>

#define LECTURA 0
#define ESCRIPTURA 1

int check_fd(int fd, int permissions)
{
  if (fd!=1) return -9; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -13; /*EACCES*/
  return 0;
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


int sys_write(int fd, char * buffer, int size){
	int r = check_fd(fd, ESCRIPTURA);
	if(r < 0) return r;
	if (buffer == NULL || size <= 0) return -EINVAL; 

	char bufferTwo[size];
	r = copy_from_user(void *start, void *dest, int size);
	if(r < 0) return r;
	r = sys_write_console (bufferTwo,size);
	return r;


}


int sys_gettime(){
	return zeos_get_ticks();


}


