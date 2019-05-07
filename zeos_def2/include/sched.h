/*
 * sched.h - Estructures i macros pel tractament de processos
 */

#ifndef __SCHED_H__
#define __SCHED_H__

#include <list.h>
#include <types.h>
#include <mm_address.h>
#include <stats.h>
#include <interrupt.h>

#define NR_TASKS      10
#define KERNEL_STACK_SIZE	1024
int ticks_left;
enum state_t { ST_RUN, ST_READY, ST_BLOCKED, ST_FREE };

struct task_struct {
  int PID;			/* Process ID. This MUST be the first field of the struct. */
  page_table_entry * dir_pages_baseAddr;

	/*new*/
	struct list_head list;
	unsigned long *kernel_esp;
 	int quantum;
	enum state_t estat;
	struct stats stat;

};

union task_union { //NAMED TASK
  struct task_struct task;
  unsigned long stack[KERNEL_STACK_SIZE];    /* pila de sistema, per procés */
};

extern union task_union task[NR_TASKS]; /* Vector de tasques */
extern struct list_head freequeue;
extern struct list_head readyqueue;

extern struct task_struct *idle_task;

#define KERNEL_ESP(t)       	(DWord) &(t)->stack[KERNEL_STACK_SIZE]

#define INITIAL_ESP       	KERNEL_ESP(&task[1])

/* Inicialitza les dades del proces inicial */
void init_task1(void);

void init_idle(void);

void init_sched(void);

struct task_struct * current();

void task_switch(union task_union*t);

struct task_struct *list_head_to_task_struct(struct list_head *l);

int allocate_DIR(struct task_struct *t);

page_table_entry * get_PT (struct task_struct *t) ;

page_table_entry * get_DIR (struct task_struct *t) ;


void init_stats(struct task_struct *t);
/* Headers for the scheduling policy */
//void sched_next_rr();
//void update_process_state_rr(struct task_struct *t, struct list_head *dest);
//int needs_sched_rr();
//void update_sched_data_rr();

void update_stats_a();
void update_stats_b();
void update_stats_c();
void update_stats_d(struct task_struct *t);

void schedule();

void init_rdyq();
void init_fq();
void set_esp(unsigned long *kernel_esp);
unsigned long get_ebp();

#endif  /* __SCHED_H__ */
