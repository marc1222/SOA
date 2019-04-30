/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>
#include <schedperf.h>



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
	//struct list_head *idle_lh = list_first(&freequeue);
	struct task_struct *idle_ts = list_head_to_task_struct(freequeue.next);	
	list_del(freequeue.next);
	
	union task_union *idle_tu = (union task_union*) idle_ts; 

	idle_ts->PID = 0;
	idle_ts->estat = ST_READY;
	idle_ts->quantum = 0;
	init_stats(idle_ts);
	allocate_DIR(idle_ts);
	//set_user_pages(idle_ts);

	idle_tu->stack[KERNEL_STACK_SIZE-2] = 0;
	idle_tu->stack[KERNEL_STACK_SIZE-1] = &cpu_idle;
	idle_tu->task.kernel_esp = &(idle_tu->stack[KERNEL_STACK_SIZE-2]);

	idle_task = idle_ts;
}

void init_task1(void)
{
	struct list_head *init_lh = list_first(&freequeue);
	struct task_struct *init_ts = list_head_to_task_struct(init_lh);	
list_del(init_lh);
		
	init_ts->PID = 1;
	ticks_left = 150;
	init_ts->quantum = 150;
	init_ts->estat = ST_RUN;
	init_stats(init_ts);

	allocate_DIR(init_ts);
	set_user_pages(init_ts);

	union task_union *init_tu = (union task_union *)init_ts;
 	tss.esp0 = &(init_tu->stack[KERNEL_STACK_SIZE]);
	writeMSR(0x175,&(init_tu->stack[KERNEL_STACK_SIZE]));
	//set_cr3(get_DIR(init_ts));
	set_cr3(get_DIR(init_ts));	
}


void init_sched(){
	init_fq();
	init_rdyq();
	init_sched_policy();
}

void init_fq() {
	INIT_LIST_HEAD(&freequeue);
	list_add(&task[0].task.list, &freequeue);
	int i;
	for(i = 1; i < NR_TASKS; i++) {
		list_add(&task[i].task.list, &task[i - 1].task.list);
		task[i].task.estat = ST_FREE;
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

void inner_task_switch(union task_union *new) {

	tss.esp0 = &(new->stack[KERNEL_STACK_SIZE]);
	writeMSR(0x175, tss.esp0);

	if (current()->dir_pages_baseAddr != new->task.dir_pages_baseAddr) {
	//printk("otro...");
	set_cr3(new->task.dir_pages_baseAddr);
	}
	//else printk("mismo...");
 	current()->kernel_esp = get_ebp();
	set_esp(new->task.kernel_esp);
} 
void update_sched_data_rr() {
  if (ticks_left != 0 && current()->stat.remaining_ticks != 0) {
    --ticks_left;
    --current()->stat.remaining_ticks;
  }
}
int needs_sched_rr () {
  return (ticks_left == 0);
}
void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue) {
  if (dst_queue == NULL) {

  }
  else {
    if (t != idle_task) list_add_tail(&(t->list), dst_queue);
  }
}
void sched_next_rr() {
  union task_union *next;
//si la cola de rdy esta vacia, metemos idle
  if (list_empty(&readyqueue)) {
    next = (union task_union *) idle_task;
    ticks_left = 0;
    idle_task->estat = ST_RUN;
    task_switch((union task_union*)idle_task);
  } else { //hay algun proceso en cola de rdy
    next = (union task_union *) list_head_to_task_struct(readyqueue.next);
    list_del(readyqueue.next);
    ticks_left = next->task.quantum;
    next->task.stat.remaining_ticks = ticks_left;
    update_stats_d(&next->task);
  }
  next->task.estat = ST_RUN;
  task_switch(next);
  
}

void schedule () {
  update_sched_data();
  if (needs_sched()) {
    update_process_state(current(), &readyqueue);
    update_stats_c();
    current()->estat = ST_READY;
    sched_next();
  }
}

int get_quantum (struct task_struct *t) {
  return t->quantum;
}

void set_quantum (struct task_struct *t, int new_quantum) {
  t->quantum = new_quantum;
}
void init_stats(struct task_struct *t) {
  t->stat.user_ticks = 0;
  t->stat.system_ticks = 0;
  t->stat.blocked_ticks = 0;
  t->stat.ready_ticks = 0;
  t->stat.elapsed_total_ticks = get_ticks();
  t->stat.total_trans = 0;
  t->stat.remaining_ticks = t->quantum;
}
void update_stats_a() {
  int current_ticks = get_ticks();
  current()->stat.user_ticks += current_ticks - current()->stat.elapsed_total_ticks;
  current()->stat.elapsed_total_ticks = current_ticks;
}

void update_stats_b() {
  int current_ticks = get_ticks();
  current()->stat.system_ticks += current_ticks - current()->stat.elapsed_total_ticks;
  current()->stat.elapsed_total_ticks = current_ticks;
}

void update_stats_c() {
  int current_ticks = get_ticks();
  current()->stat.system_ticks += current_ticks - current()->stat.elapsed_total_ticks;
  current()->stat.elapsed_total_ticks = current_ticks;
}

void update_stats_d(struct task_struct *t) {
  int current_ticks = get_ticks();
  t->stat.ready_ticks += current_ticks - t->stat.elapsed_total_ticks;
  t->stat.elapsed_total_ticks = current_ticks;
  t->stat.total_trans++;
}
struct stats * get_task_stats(struct task_struct *t) 
{
	return &(t->stat);
}
struct list_head * get_task_list(struct task_struct *t)
{
	return &(t->list);
}

void block_process(struct list_head *block_queue) {
	struct task_struct *t = current();
	struct stats *st = get_task_stats(t);
	update_process_state(t, block_queue);
	st->system_ticks = get_ticks()-st->elapsed_total_ticks;
	st->elapsed_total_ticks = get_ticks();
	sched_next();
}
void unblock_process(struct task_struct *blocked){
	struct stats *st = get_task_stats(blocked);
	struct list_head *l = get_task_list(blocked);
	update_process_state(blocked, &readyqueue);
	st->blocked_ticks += (get_ticks()-st->elapsed_total_ticks);
	st->elapsed_total_ticks = get_ticks();
	if (needs_sched()) {
		update_process_state(current(), &readyqueue);
		sched_next();
	}
}



