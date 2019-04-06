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
#define BUFF_SIZE 8 


int check_fd(int fd, int permissions)
{
  if (fd!=1) return -EBADF; /*EBADF*/
  if (permissions!=ESCRIPTURA) return -EACCES;

  return 0;
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
  //update_stats_a();
  update_stats_b();
  return 0;
}

int sys_clone (void (*function)(void), void *stack) 
{
  struct task_struct *pareptr;
  struct task_struct *fillptr;
  union task_union *fillunion;
  int pos;
  
  update_stats_a();
  if(list_empty(&freequeue)) {
    update_stats_b();
    return -ENOMEM;	
  }

  if (!access_ok(VERIFY_READ, function, 42)) { // quin size posem?
    update_stats_b();
    return -EFAULT;
  }

  if (!access_ok(VERIFY_WRITE, stack, 42)) { // quin size posem?
    update_stats_b();
    return -EFAULT;
  }

  pareptr = current();
  fillptr = list_head_to_task_struct(freequeue.next);
  list_del(freequeue.next);
  copy_data(pareptr, fillptr, sizeof(union task_union)); // herencia del task union (i del directori)
  allocate_DIR(pareptr); // nomes incrementa les refs del dir del pare
  fillptr->PID=next_PID;
  next_PID++;
  init_stats(fillptr);
  fillptr->estat = ST_READY;
  fillunion = (union task_union*) fillptr;
  fillptr->kernel_ebp = &(fillunion->stack[KERNEL_STACK_SIZE - 18]);
  //*(fillptr->kernel_ebp) = 0xDEAD;
  list_add_tail(&fillptr->list, &readyqueue);
  fillunion->stack[KERNEL_STACK_SIZE - 5] = function;
  fillunion->stack[KERNEL_STACK_SIZE - 2] = stack;
  update_stats_b();
  return fillptr->PID;
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
  if(list_empty(&freequeue)) {
    update_stats_b();
    return -ENOMEM;	
  }
  pareptr = current();
  fillptr = list_head_to_task_struct(freequeue.next);
  list_del(freequeue.next);
  copy_data(pareptr, fillptr, sizeof(union task_union)); // herencia del task union
  allocate_DIR(fillptr);
  fillPT = get_PT(fillptr);
  parePT = get_PT(pareptr);

  for (pag = 0; pag < NUM_PAG_KERNEL + NUM_PAG_CODE; ++pag) {
    fillPT[pag] = parePT[pag]; // herencia de la taula de pagines
  }

  for (pag = PAG_LOG_INIT_DATA; pag < NUM_PAG_DATA+PAG_LOG_INIT_DATA; ++pag) {
    fr[pag-PAG_LOG_INIT_DATA] = alloc_frame();
      if (fr[pag-PAG_LOG_INIT_DATA] == -1) { // fem la marxa enrere si hi ha error, anulem els nous frames
        update_stats_b();
        for (pag = pag - 1; pag >= PAG_LOG_INIT_DATA; --pag) {
          free_frame(fr[pag-PAG_LOG_INIT_DATA]);
        }
        return -ENOMEM;
      }
        set_ss_pag(fillPT, pag, fr[pag-PAG_LOG_INIT_DATA]); // el fill comparteix les seves pagines amb el pare
        set_ss_pag(parePT, pag + NUM_PAG_DATA, fr[pag-PAG_LOG_INIT_DATA]);
  }

  // copia de les dades del pare
  copy_data(PAG_LOG_INIT_DATA*PAGE_SIZE, (PAG_LOG_INIT_DATA+NUM_PAG_DATA)*PAGE_SIZE, NUM_PAG_DATA*PAGE_SIZE);

  for (pag = PAG_LOG_INIT_DATA+NUM_PAG_DATA; pag < PAG_LOG_INIT_DATA+2*NUM_PAG_DATA; ++pag) {
    del_ss_pag(parePT, pag); // el fill ja no comparteix pagines
  }
  set_cr3(get_DIR(pareptr));
  fillptr->PID=next_PID;
  next_PID++;
  init_stats(fillptr);
  fillptr->estat = ST_READY;
  /*fillptr->kernel_ebp = (void*) pareptr->kernel_ebp - (void*)pareptr + (void*)fillptr;
  *(fillptr->kernel_ebp) = &ret_from_fork;
  fillptr->kernel_ebp--;
  *(fillptr->kernel_ebp) = 0xDEAD;*/
  fillunion = (union task_union*) fillptr;
  fillptr->kernel_ebp = &(fillunion->stack[KERNEL_STACK_SIZE - 18]);
  *(fillptr->kernel_ebp) = &ret_from_fork;
  fillptr->kernel_ebp--; // ara fillptr = &fillunion->stack[KERNEL_STACK_SIZE - 19]
  *(fillptr->kernel_ebp) = 0xDEAD;
  list_add_tail(&fillptr->list, &readyqueue);
  update_stats_b();
  return fillptr->PID;
}
    
void sys_exit()
{  
  int pos;
  int i;

  update_stats_a();
  
  for (i = 0; i < NR_SEMS; ++i) {
    sys_sem_destroy(i);
  }

  current()->estat = ST_FREE;
  
  pos = ((int)current()->dir_pages_baseAddr-(int)dir_pages)/(sizeof(page_table_entry) * TOTAL_PAGES);
  --dir_refs[pos];
  if (dir_refs[pos] == 0) {
    free_user_pages(current());
  }
  list_add_tail(&current()->list, &freequeue);
  sched_next_rr();
  //printk("NO ME EJECUTO NUNCA");
}

int sys_write(int fd, char * buffer, int size)
{
  update_stats_a();
  char buff[BUFF_SIZE];
  int r = 0;
  int i = 0;
  
  if ((r = check_fd(fd, ESCRIPTURA)) != 0) {
    update_stats_b();
    return r;
  }
  if (buffer == NULL) {
    update_stats_b();
    return -EFAULT;
  }
  if (size < 0) {
    update_stats_b();
    return -EINVAL;
  }
  if (access_ok(VERIFY_READ, buffer, size) == 0) {
    update_stats_b();
    return -EFAULT;
  }
  while (size - i - BUFF_SIZE > 0) {
    copy_from_user(&buffer[i], buff, BUFF_SIZE); // en teoria no pot retornar mai -1
    r += sys_write_console(buff, BUFF_SIZE);
    i += BUFF_SIZE;
  }
  if (size - i > 0) {
    copy_from_user(&buffer[i], buff, size - i);
    r += sys_write_console(buff, size - i);
  }
  update_stats_b();
  return r;	
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
    copy_to_user(&(taskptr->stat), st, sizeof(struct stats));
    update_stats_b();
    return 0;
  }
  else {
    update_stats_b();
    return -EFAULT;
  }
}
  
int sys_sem_init(int n_sem, unsigned int value) {
  if (n_sem < 0 || n_sem >= NR_SEMS) {
     //printk("einval");
     return -EINVAL;
  }
  if (sems[n_sem].ocupat) {
    //printk("ebusyskipped");
    return -EBUSY;
  }
  sems[n_sem].ocupat = 1;
  sems[n_sem].counter = (int) value;
  sems[n_sem].owner = current()->PID;
  ++sems[n_sem].times_used;
  INIT_LIST_HEAD(&(sems[n_sem].queue));
  return 0;
}

int sys_sem_wait(int n_sem) {
  int times_used_before;

  if (n_sem < 0 || n_sem >= NR_SEMS || !sems[n_sem].ocupat) { ///
	  return -EINVAL;
  }
  sems[n_sem].counter--;
  if (sems[n_sem].counter < 0) {
    current()->estat = ST_BLOCKED;
    times_used_before = sems[n_sem].times_used;
    list_add_tail(&(current()->list), &(sems[n_sem].queue));
    sched_next_rr();
    // return -1 si al tornar el sem esta lliure o el sem_id es diferent del que havia abans
    if (!sems[n_sem].ocupat || times_used_before != sems[n_sem].times_used) {
      return -1;
    }
  }
  return 0;
}

int sys_sem_signal(int n_sem) {
  if (n_sem < 0 || n_sem >= NR_SEMS || !sems[n_sem].ocupat) {
	  return -EINVAL;
  }
  sems[n_sem].counter++;
  if (sems[n_sem].counter <= 0) {
    struct task_struct *taskptr = list_head_to_task_struct(sems[n_sem].queue.next);
    list_del(sems[n_sem].queue.next);
    taskptr->estat = ST_READY;
    list_add_tail(&(taskptr->list), &readyqueue);
  }
  return 0;
}

int sys_sem_destroy(int n_sem) {

  if (n_sem < 0 || n_sem >= NR_SEMS || !sems[n_sem].ocupat) {
    return -EINVAL;
  }

  if (sems[n_sem].owner != current()->PID) {
    return -1;
  }

  sems[n_sem].ocupat = 0;

  while (!list_empty(&(sems[n_sem].queue))) {
    struct task_struct *taskptr = list_head_to_task_struct(sems[n_sem].queue.next);
    list_del(sems[n_sem].queue.next);
    taskptr->estat = ST_READY;
    list_add_tail(&(taskptr->list), &readyqueue);
  }
  
  return 0;
}
  
