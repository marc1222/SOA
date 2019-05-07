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



void bitoa(int a, char *b)
{
  int i, i1;

	for(i = 0; i < 50; i++) b[i] = 0;
  if (a==0) { b[0]='0'; b[1]=0; return ;}

	i = 0;
	int aux = a;
	//printk("(bitoa: ");
	char c[10];
	while(aux>0){
		c[i]= '0';
		aux=aux/10;
		i++;
		
	}
	//for(i1 = 0; i1 <i; i1++)printk(c);
	//printk(" ");
	i--;
	while(i>=0){
    		b[i]=(a%10)+'0';
		a=a/10;
		i--;
	}
	//printk(") ");
}

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
  update_stats_a();
  update_stats_b();
	return -38; /*ENOSYS*/
}

int sys_getpid()
{
  update_stats_a();
  update_stats_b();
	return current()->PID;
}

int ret_from_fork() {
  update_stats_b();
  return 0;
}
int sys_fork()
{
  struct task_struct *pareptr;
  struct task_struct *fillptr;
  int pag;
  int fr[NUM_PAG_DATA];
  page_table_entry *fillPT;
  page_table_entry *parePT;
  union task_union *fillunion;

  update_stats_a();
//no hay pcb error
  if(list_empty(&freequeue)) {
  update_stats_b();
    return -ENOMEM;	
  }
//cojemos pcb del padre y cojemos uno libre para el hijo y lo borramos de free queue
  pareptr = current();
  fillptr = list_head_to_task_struct(freequeue.next);
  list_del(freequeue.next);
  copy_data(pareptr, fillptr, sizeof(union task_union)); // herencia del task union
  allocate_DIR(fillptr);
  fillPT = get_PT(fillptr);
  parePT = get_PT(pareptr);
 // herencia de la taula de pagines
  for (pag = 0; pag < NUM_PAG_KERNEL + NUM_PAG_CODE; ++pag) {
    fillPT[pag] = parePT[pag];
  }
//emepzamos a hacer el alloc frame de las paginas
//si hay error, liberamos todos los frames reservados hasta el error (marxa enerere)
  for (pag = 0; pag < NUM_PAG_DATA; ++pag) {
    fr[pag] = alloc_frame();
      if (fr[pag] == -1) {
	update_stats_b();
        for (pag = pag - 1; pag >= 0; --pag) {
          free_frame(fr[pag]);
        }
        return -ENOMEM;
     }
// es comparteixen les pagines del fill amb les del pare
     set_ss_pag(fillPT, pag+PAG_LOG_INIT_DATA, fr[pag]); 
     set_ss_pag(parePT, pag + NUM_PAG_DATA + PAG_LOG_INIT_DATA, fr[pag]);
  }
//copiem les dades de les pagines
  copy_data(PAG_LOG_INIT_DATA*PAGE_SIZE,(PAG_LOG_INIT_DATA+NUM_PAG_DATA)*PAGE_SIZE, NUM_PAG_DATA*PAGE_SIZE);

//desvinculem les entrades de la PT del pare dels frames fisics del fill
   for (pag = PAG_LOG_INIT_DATA+NUM_PAG_DATA; pag < PAG_LOG_INIT_DATA+2*NUM_PAG_DATA; ++pag) {
    del_ss_pag(parePT, pag);
  }
//flush TLB
  set_cr3(get_DIR(pareptr));
//asignem nou unic pid
 fillptr->PID=next_PID;
 next_PID++;

 init_stats(fillptr);
 fillptr->estat = ST_READY;

 fillunion = (union task_union*) fillptr;
 fillptr->kernel_esp = &(fillunion->stack[KERNEL_STACK_SIZE - 18]);
 *(fillptr->kernel_esp) = &ret_from_fork;
 fillptr->kernel_esp--;
 *(fillptr->kernel_esp) = 0x0;
 list_add_tail(&fillptr->list, &readyqueue);
 update_stats_b();
 return fillptr->PID;
}

void sys_exit()
{  

  update_stats_a();
  
  current()->estat = ST_FREE;

  //free_user_pages(current());
	for (int pag=0; pag<NUM_PAG_DATA; pag++)	{
			free_frame(get_frame(get_PT(current()),PAG_LOG_INIT_DATA+pag));
			del_ss_pag(get_PT(current()),PAG_LOG_INIT_DATA+pag);
	}
  list_add_tail(&current()->list, &freequeue);
  sched_next_rr();
}

#define MAX_BUFFER_SIZE 256
int sys_write( int fd, char* buff, int size ) {
  	update_stats_a();
	char buff_aux[MAX_BUFFER_SIZE];
	int res = check_fd(fd,ESCRIPTURA);
	if (res != 0) {
		return res;
  		update_stats_b();	
	}
	if (buff == NULL) {
		return -EINVAL;
		update_stats_b();
	}
	if (size < 0) {
		return -EINVAL;
		update_stats_b();	
	}
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
	update_stats_b();
	return (size-left);
	
}
int sys_gettime()
{
	update_stats_a();
	update_stats_b();
	return zeos_ticks;
}

int sys_get_stats(int pid, struct stats *st) {
  struct task_struct *taskptr;
  int trobat;
  int i;

  update_stats_a();
	
  trobat = 0;
  if (pid < 0) {
    update_stats_b();
    return -EINVAL;
  }
  for (i = 0; !trobat &&  i < NR_TASKS; ++i) {
    if (pid == task[i].task.PID) {
      taskptr = &(task[i].task);
      trobat = 1;
    }	
  }
  if (!trobat || taskptr->estat == ST_FREE)
    return -ESRCH;
  if(access_ok(VERIFY_WRITE, st, sizeof(struct stats))) {

	char buff[2048];
	/*printk("(sys.c current())PID ");
	bitoa(current()->PID,buff);
	printk(buff);
	
	printk("\ncurrrent():");
	bitoa(current()->stat.user_ticks,buff);
	printk(buff);
	printk(" ");

	bitoa(current()->stat.system_ticks,buff);
	printk(buff);
	printk(" ");

	bitoa(current()->stat.blocked_ticks,buff);
	printk(buff);
	printk(" ");

	bitoa(current()->stat.ready_ticks,buff);
	printk(buff);
	printk(" ");

	bitoa(current()->stat.elapsed_total_ticks,buff);
	printk(buff);
	printk(" ");

	bitoa(current()->stat.remaining_ticks,buff);
	printk(buff);
	printk(" \n");*/

    copy_to_user(&(taskptr->stat), st, sizeof(struct stats));
	
	
	/*
	printk("st:");
	itoa(st->user_ticks,buff);
	printk(buff);
	printk(" ");

	itoa(st->system_ticks,buff);
	printk(buff);
	printk(" ");

	itoa(st->blocked_ticks,buff);
	printk(buff);
	printk(" ");

	itoa(st->ready_ticks,buff);
	printk(buff);
	printk(" ");

	itoa(st->elapsed_total_ticks,buff);
	printk(buff);
	printk(" ");

	itoa(st->remaining_ticks,buff);
	printk(buff);
	printk(" \n");
	*/

	
	

	
    update_stats_b();


    return 0;
  }
  else {
    update_stats_b();
    return -EFAULT;
  }
}

void sys_uselessMatrix(int *M,int n){
	int a, b, c;
	char buff[2048];
	int matrix[n][n][n];
	for(a = 0; a < n; a++)
		for(b = 0; b <n; b++)
			for(c = 0; c<n; c++){
				if((a%2==1) && (b%2==0) &&(b%2==0)){
					matrix[a][c][b]= 4;
				}
				else if((a%2==0) && (b%3==2) &&(b%2==0)){
					matrix[a][a][c]= 8;
				}
				else if((a%2==0) && (b%4==2) &&(b%2==0)){
					matrix[c][c][c]= 1;
				}	
				else {matrix[a][b][b] = 9;}

			}
    	copy_to_user(&matrix, M, sizeof(int)*n*n*n);
	printk("matrix done");

}
