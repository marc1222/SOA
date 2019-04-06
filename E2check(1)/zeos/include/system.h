/*
 * system.h - Capçalera del mòdul principal del sistema operatiu
 */

#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include <types.h>

int zeos_ticks;
int next_PID;
extern TSS         tss;
extern Descriptor* gdt;

#endif  /* __SYSTEM_H__ */
