/*
 * interrupt.c -
 */
#include <types.h>
#include <interrupt.h>
#include <segment.h>
#include <hardware.h>
#include <io.h>
#include <system.h>
#include <zeos_interrupt.h>
#include <libc.h>
#include <schedperf.h>


void system_call_handler();

Gate idt[IDT_ENTRIES];
Register    idtR;

char char_map[] =
{
  '\0','\0','1','2','3','4','5','6',
  '7','8','9','0','\'','¡','\0','\0',
  'q','w','e','r','t','y','u','i',
  'o','p','`','+','\0','\0','a','s',
  'd','f','g','h','j','k','l','ñ',
  '\0','º','\0','ç','z','x','c','v',
  'b','n','m',',','.','-','\0','*',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0','\0','\0','\0','\0','\0','7',
  '8','9','-','4','5','6','+','1',
  '2','3','0','\0','\0','\0','<','\0',
  '\0','\0','\0','\0','\0','\0','\0','\0',
  '\0','\0'
};

void setInterruptHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE INTERRUPTION GATE FLAGS:                          R1: pg. 5-11  */
  /* ***************************                                         */
  /* flags = x xx 0x110 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}

void setTrapHandler(int vector, void (*handler)(), int maxAccessibleFromPL)
{
  /***********************************************************************/
  /* THE TRAP GATE FLAGS:                                  R1: pg. 5-11  */
  /* ********************                                                */
  /* flags = x xx 0x111 000 ?????                                        */
  /*         |  |  |                                                     */
  /*         |  |   \ D = Size of gate: 1 = 32 bits; 0 = 16 bits         */
  /*         |   \ DPL = Num. higher PL from which it is accessible      */
  /*          \ P = Segment Present bit                                  */
  /***********************************************************************/
  Word flags = (Word)(maxAccessibleFromPL << 13);

  //flags |= 0x8F00;    /* P = 1, D = 1, Type = 1111 (Trap Gate) */
  /* Changed to 0x8e00 to convert it to an 'interrupt gate' and so
     the system calls will be thread-safe. */
  flags |= 0x8E00;    /* P = 1, D = 1, Type = 1110 (Interrupt Gate) */

  idt[vector].lowOffset       = lowWord((DWord)handler);
  idt[vector].segmentSelector = __KERNEL_CS;
  idt[vector].flags           = flags;
  idt[vector].highOffset      = highWord((DWord)handler);
}


void setIdt()
{
  /* Program interrups/exception service routines */
  idtR.base  = (DWord)idt;
  idtR.limit = IDT_ENTRIES * sizeof(Gate) - 1;
  
  set_handlers();
/* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */
  setInterruptHandler(33, keyboard_handler, 0);
  setInterruptHandler(32, clock_handler, 0);
  setTrapHandler(0x80, system_call_handler, 3);
  /* ADD INITIALIZATION CODE FOR INTERRUPT VECTOR */

  set_idt_reg(&idtR);
}
void setMSR() {
	writeMSR(0x174,__KERNEL_CS);
	writeMSR(0x175,INITIAL_ESP);
	writeMSR(0x176,(int)syscall_handler_sysenter);
}
void keyboard_routine() {
	char ch = inb (0x60);
	if(ch & 0x80) {//break
		
	}
	else {
		//COMMENT: since i changed sys_get_stats to get_task_stats the ret can no longer be used to check for errors (if ret!=-1 then print)
		char aux = char_map[ch & 0x7F];
		if(aux != '\0') printc_xy(0, 0, aux);		
		else printc_xy(0,0,'C');
		int ret;		
		struct stats* aux2;
		char buff[256];
		char buff2[256];
		if (aux == 'r') {
			ret = sys_set_sched_policy(0);
			if (ret != -1) {
				while (ret < 100000) {
					ret++;
				}
				printk("---RR---");
				aux2 = get_task_stats(current());
				printk("PID: ");
				itoa(current()->PID,buff2);
				printk(buff2);
				printk("...");
				itoa(aux2->user_ticks,buff);
				printk("user_ticks");
				printk(buff);
				printk("...");
				itoa(aux2->system_ticks,buff);
				printk("system ticks");
				printk(buff);
				printk("...");
				itoa(aux2->blocked_ticks,buff);
				printk("blocked ticks");
				printk(buff);
				printk("...");
				itoa(aux2->ready_ticks,buff);
				printk("ready ticks");
				printk(buff);
				printk("...");
				itoa(aux2->elapsed_total_ticks,buff);
				printk("elapsed total ticks");
				printk(buff);
				printk("...");
				itoa(aux2->remaining_ticks,buff);
				printk("remaining ticks");
				printk(buff);
				printk("...");
				itoa(aux2->total_trans,buff);
				printk("total trans");
				printk(buff);
				printk("...");

								
			}
		}
		else if(aux == 'f') {
			ret = sys_set_sched_policy(1);
			if (ret != -1) {

				while (ret < 100000) {
					ret++;
				}

				printk("---FCFS---");	
				aux2 = get_task_stats(current());
				printk("PID: ");
				itoa(current()->PID,buff2);
				printk(buff2);
				printk("...");

				itoa(aux2->user_ticks,buff);
				printk("user_ticks");
				printk(buff);
				printk("...");
				itoa(aux2->system_ticks,buff);
				printk("system ticks");
				printk(buff);
				printk("...");
				itoa(aux2->blocked_ticks,buff);
				printk("blocked ticks");
				printk(buff);
				printk("...");
				itoa(aux2->ready_ticks,buff);
				printk("ready ticks");
				printk(buff);
				printk("...");
				itoa(aux2->elapsed_total_ticks,buff);
				printk("elapsed total ticks");
				printk(buff);
				printk("...");
				itoa(aux2->remaining_ticks,buff);
				printk("remaining ticks");
				printk(buff);
				printk("...");
				itoa(aux2->total_trans,buff);
				printk("total trans");
				printk(buff);
				printk("...");
			}		
		}
		else if (aux == 'p') {
			aux2 = get_task_stats(idle_task);
			printk("PID: ");
			itoa(idle_task->PID,buff2);
			printk(buff2);
			printk("...");
			itoa(aux2->user_ticks,buff);
			printk("user_ticks");
			printk(buff);
			printk("...");
			itoa(aux2->system_ticks,buff);
			printk("system ticks");
			printk(buff);
			printk("...");
			itoa(aux2->blocked_ticks,buff);
			printk("blocked ticks");
			printk(buff);
			printk("...");
			itoa(aux2->ready_ticks,buff);
			printk("ready ticks");
			printk(buff);
			printk("...");
			itoa(aux2->elapsed_total_ticks,buff);
			printk("elapsed total ticks");
			printk(buff);
			printk("...");
			itoa(aux2->remaining_ticks,buff);
			printk("remaining ticks");
			printk(buff);
			printk("...");
			itoa(aux2->total_trans,buff);
			printk("total trans");
			printk(buff);
			printk("...");
		}
			
	}
	update_stats_b();
}

void clock_routine() {
	update_stats_a();
	zeos_ticks++;
	zeos_show_clock();
	schedule();
	update_stats_b();
	zeos_update_read_console_emul();
}

