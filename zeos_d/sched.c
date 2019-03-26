/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>




struct list_head freequeue;
struct list_head readyqueue;

struct task_struct * idle_task;

union task_union task[NR_TASKS]
  __attribute__((__section__(".data.task")));


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return (struct task_struct *)((unsigned int)l&0xfffff000);
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
	struct list_head *idle_lh = list_first(&freequeue);
	list_del(idle_lh);
	struct task_struct *idle_ts = list_head_to_task_struct(idle_lh);
	
	union task_union *idle_tu = (union task_union*) idle_ts; 

	idle_ts->PID = 0;
	allocate_DIR(idle_ts);


	idle_tu->stack[KERNEL_STACK_SIZE-2] = 0;
	idle_tu->stack[KERNEL_STACK_SIZE-1] = (unsigned long)&cpu_idle;
	idle_tu->p_task.kernel_esp = (unsigned long)&idle_tu->stack[KERNEL_STACK_SIZE-2];

	idle_task = idle_ts;
}

void init_task1(void)
{
	struct list_head *init_lh = list_first(&freequeue);
	list_del(init_lh);
	struct task_struct *init_ts = list_head_to_task_struct(init_lh);	
	init_ts->PID = 1;

	allocate_DIR(init_ts);
	set_user_pages(init_ts);

	union task_union *init_tu = (union task_union *)init_ts;
 	tss.esp0 = &init_tu->stack[KERNEL_STACK_SIZE];
	writeMSR(0x175,tss.esp0);

	set_cr3(init_ts->dir_pages_baseAddr);	
}


void init_sched(){
	init_fq();
	init_rdyq();
}

void init_fq() {
//orden 0->1->2->3->....->n-freequeue (la cola cuelga de freequeue->prev)
//EL PREV DE FREEQUEUE APUNTA A N, FREEQUEUE NEXT APUNTA A SI MISMO
	INIT_LIST_HEAD(&freequeue);
	int i;
	for(i = 0; i < NR_TASKS; i++) {
		list_add_tail(&task[i].p_task.list,&freequeue);
	}
}
void init_rdyq() {
	INIT_LIST_HEAD(&readyqueue); //vacia
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

void inner_task_witch(union task_union *new) {

	tss.esp0 = (unsigned long)&new->stack[KERNEL_STACK_SIZE];
	writeMSR(0x175, tss.esp0);

	set_cr3(new->p_task.dir_pages_baseAddr);

	unsigned long *ebp =  &(current()->kernel_esp);
	unsigned long *new_esp = new->p_task.kernel_esp;
	 __asm__ __volatile__(
  		"mov %%ebp,(%0);"
  		"mov %1,%%esp;" 
  		"pop %%ebp;"
  		"ret;"
		: //no output
  		: "r" (ebp), "r" (new_esp)
	);
} 

