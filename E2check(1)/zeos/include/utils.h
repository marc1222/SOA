#ifndef UTILS_H
#define UTILS_H

void copy_data(void *start, void *dest, int size);
int copy_from_user(void *start, void *dest, int size);
int copy_to_user(void *start, void *dest, int size);

#define VERIFY_READ	0
#define VERIFY_WRITE	1
int access_ok(int type, const void *addr, unsigned long size);

unsigned long get_ticks(void);

#define min(a,b)	(a<b?a:b)

unsigned long get_ticks(void);

// funcions en assembler pel codi de sistema
unsigned long* get_ebp_tswitch();
void set_esp_ret(unsigned long *kernel_ebp);
void writeMSR(int num, long int data);



#endif
