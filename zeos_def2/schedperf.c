#include <schedperf.h>


struct stats * get_task_stats(struct task_struct *t) 
{
	return &(t->stat);
}
struct list_head * get_task_list(struct task_struct *t)
{
	return &(t->list);
}
