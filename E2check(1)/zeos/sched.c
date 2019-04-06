/*
 * sched.c - initializes struct for task 0 anda task 1
 */

#include <sched.h>
#include <mm.h>
#include <io.h>
#include <utils.h>

/**
 * Container for the Task array and 2 additional pages (the first and the last one)
 * to protect against out of bound accesses.
 */
union task_union protected_tasks[NR_TASKS+2]
  __attribute__((__section__(".data.task")));

union task_union *task = &protected_tasks[1]; /* == union task_union task[NR_TASKS] */

struct task_struct *idle_task;


struct task_struct *list_head_to_task_struct(struct list_head *l)
{
  return list_entry( l, struct task_struct, list);
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
  ++dir_refs[pos];
  
  return 1;
}

void cpu_idle(void)
{
  __asm__ __volatile__("sti": : :"memory");

  while(1)
  {
  ;
  }
}

void inner_task_switch(union task_union*t)
{
  tss.esp0 = &(t->stack[KERNEL_STACK_SIZE]);
  writeMSR(0x0175, &(t->stack[KERNEL_STACK_SIZE]));
  if (current()->dir_pages_baseAddr != t->task.dir_pages_baseAddr) {
    set_cr3(get_DIR(&(t->task)));
  }
  current()->kernel_ebp = get_ebp_tswitch();
  set_esp_ret(t->task.kernel_ebp);
  
}

void init_idle (void)
{
  union task_union *unionptr;
  struct list_head *primer = freequeue.next;
  idle_task = list_head_to_task_struct(freequeue.next);
  list_del(freequeue.next);
  idle_task->PID = 0;
  idle_task->estat = ST_READY;
  idle_task->quantum = 0;
  init_stats(idle_task);
  allocate_DIR(idle_task);
  ((union task_union *) idle_task)->stack[KERNEL_STACK_SIZE - 1] = &cpu_idle;
  ((union task_union *) idle_task)->stack[KERNEL_STACK_SIZE - 2] = 0x12345678;
  idle_task->kernel_ebp = &(((union task_union *) idle_task)->stack[KERNEL_STACK_SIZE - 2]);
}

void init_task1(void)
{
  struct task_struct *initptr;

  initptr = list_head_to_task_struct(freequeue.next);
  list_del(freequeue.next);
  initptr->PID = 1;
  ticks_left = 150;
  initptr->quantum = 150;
  initptr->estat = ST_RUN;
  init_stats(initptr);
  allocate_DIR(initptr);
  set_user_pages(initptr);
  tss.esp0 = &(((union task_union *) initptr)->stack[KERNEL_STACK_SIZE]);
  writeMSR(0x0175, &(((union task_union *) initptr)->stack[KERNEL_STACK_SIZE]));
  set_cr3(get_DIR(initptr));
}


void init_sched(){
  int i;
  
  INIT_LIST_HEAD(&freequeue);
  list_add(&task[0].task.list, &freequeue);
  for (i = 1; i < NR_TASKS; ++i) {
    list_add(&task[i].task.list, &task[i - 1].task.list);
    task[i].task.estat = ST_FREE;
  }
  INIT_LIST_HEAD(&readyqueue);
  for (i = 0; i < NR_TASKS; ++i) {
    dir_refs[i] = 0;
  }
  
  for (i = 0; i < NR_SEMS; ++i) {
    sems[i].ocupat = 0;
    sems[i].times_used = 0;
  }
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

void update_sched_data_rr () {
  if (ticks_left != 0 && current()->stat.remaining_ticks != 0) {
    --ticks_left;
    --current()->stat.remaining_ticks;
  }
}

int needs_sched_rr () {
  //return (ticks_left == 0 && !list_empty(&readyqueue));
  return (ticks_left == 0);
}

void update_process_state_rr (struct task_struct *t, struct list_head *dst_queue) {
  if (dst_queue == NULL) {
    //list_del(readyqueue.next);
  }
  else {
    if (t != idle_task) list_add_tail(&(t->list), dst_queue);
  }
}

void sched_next_rr () {
  union task_union *nextptr;

  if (list_empty(&readyqueue)) {
    nextptr = (union task_union *) idle_task;
    ticks_left = 0;
    idle_task->estat = ST_RUN;
    task_switch((union task_union*)idle_task);
  }
  else {
    nextptr = (union task_union *) list_head_to_task_struct(readyqueue.next);
    list_del(readyqueue.next);
    ticks_left = nextptr->task.quantum;
    nextptr->task.stat.remaining_ticks = ticks_left;
    update_stats_d(&nextptr->task);
  }
  nextptr->task.estat = ST_RUN;
  task_switch(nextptr);
  
}

void schedule () {
  update_sched_data_rr();
  if (needs_sched_rr()) {
    update_process_state_rr(current(), &readyqueue);
    update_stats_c();
    current()->estat = ST_READY;
    sched_next_rr();
  }
  //else update_stats_b();
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
  //current()->stat.total_trans++;
}

void update_stats_d(struct task_struct *t) {
  int current_ticks = get_ticks();
  t->stat.ready_ticks += current_ticks - t->stat.elapsed_total_ticks;
  t->stat.elapsed_total_ticks = current_ticks;
  t->stat.total_trans++;
}


