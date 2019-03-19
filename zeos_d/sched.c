/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return (struct task_struct *)(l & (struct list_head *)0xfffff000);
}


extern struct list_head blocked;


/* get_DIR - Returns the Page Directory address for task 't' */
page_table_entry * get_DIR (struct task_struct *t) 
{
	return t->dir_pages_baseAddr;
}

/* get_PT - Returns the Page Table address for task 't' */
page_table_entry * get_PT (struct task_struct *t) 
{
	return (page_table_entry *)(((unsigned int)(t->dir_pages_baseAddr->bits.pbase_addr))<<12);
}


int allocate_DIR(struct task_struct *t) 
{
	int pos;

	pos = ((int)t-(int)task)/sizeof(union task_union);

	t->dir_pages_baseAddr = (page_table_entry*) &dir_pages[pos]; 

	return 1;
}

void cpu_idle(void)
{
	__asm__ __volatile__("sti": : :"memory");

	while(1);
}

void init_idle (void)
{

	idle_task = list_head_to_task_struct(freequeue->prev);
	list_del(freequeue->prev);
	idle_task->PID = 0;
	allocate_DIR(idle_task);
	//new
	idle_task->kernel_esp = (int *)(&idle_task[KERNEL_STACK_SIZE-2]);
	int *aux = (int *)idle_task;
	aux[KERNEL_STACK_SIZE-1] = cpu_idle;
	aux[KERNEL_STACK_SIZE-2] = 0;
}

void init_task1(void)
{
	struct task_struct * init = list_head_to_task_struct(freequeue->prev);
	list_del(freequeue->prev);
	init->PID = 1;
	allocate_DIR(init);

	set_user_pages(init);

        tss.esp0 = (DWord)init;
	writeMSR(0x175,(DWord)init);

	set_cr3(init->dir_pages_baseAddr);	
}


void init_sched(){

	INIT_LIST_HEAD(freequeue);
	int i;
//orden 0->1->2->3->....->n-freequeue (la cola cuelga de freequeue->prev)
//EL PREV DE FREEQUEUE APUNTA A N, FREEQUEUE NEXT APUNTA A SI MISMO
	for(i = 0; i < NR_TASKS; i++) {
		list_add_tail(&(task[i]->p_task->list), freequeue);
	}

	INIT_LIST_HEAD(readyqueue); //vacia

	

}

struct task_struct* current()
{
  int ret_value;
  
  __asm__ __volatile__(
  	"movl %%esp, %0"
	: "=g" (ret_value)
  );
  return (struct task_struct*)(ret_value&0xfffff000);
}

