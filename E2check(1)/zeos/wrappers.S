#include <asm.h>

ENTRY(write_slow)
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx
	movl $4, %eax
	int $0x80
	cmpl $0, %eax
	jge fin_write_slow
err_write_slow:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_write_slow:
	movl %ebp, %esp
	popl %ebp
	ret

ENTRY(gettime_slow)
	pushl %ebp
	movl %esp, %ebp
	movl $10, %eax
	//movl $666, %eax
	int $0x80
	cmpl $0, %eax
	jge fin_gettime_slow
err_gettime_slow:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_gettime_slow:
	movl %ebp, %esp
	popl %ebp
	ret

ENTRY(write)
	pushl %ecx
	pushl %edx
	pushl %ebx // es un registre "segur"
	movl 16(%esp), %ebx
	movl 20(%esp), %ecx
	movl 24(%esp), %edx
	movl $4, %eax
	pushl $tornada_write
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_write:
	movl %ebp, %esp
	popl %ebp
	popl %edx // descartem la adreca
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_write
err_write:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_write:
	ret

ENTRY(gettime)
	pushl %ecx
	pushl %edx
	movl $10, %eax
	pushl $tornada_gettime
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_gettime:
	movl %ebp, %esp
	popl %ebp
	popl %edx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_gettime
err_gettime:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_gettime:
	ret

ENTRY(getpid)
	pushl %ecx
	pushl %edx
	movl $20, %eax
	pushl $tornada_getpid
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_getpid:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_getpid
err_getpid:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_getpid:
	ret

ENTRY(fork)
	pushl %ecx
	pushl %edx
	movl $2, %eax
	pushl $tornada_fork
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_fork:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_fork
err_fork:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_fork:
	ret
	
ENTRY(exit)
	pushl %ecx
	pushl %edx
	movl $1, %eax
	pushl $tornada_exit
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_exit:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	ret

ENTRY(get_stats)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl 20(%esp), %ecx
	movl $35, %eax
	pushl $tornada_get_stats
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_get_stats:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_get_stats
err_get_stats:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_get_stats:
	ret

ENTRY(clone)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl 20(%esp), %ecx
	movl $19, %eax
	pushl $tornada_clone
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_clone:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_clone
err_clone:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_clone:
	ret

ENTRY(sem_init)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl 20(%esp), %ecx
	movl $21, %eax
	pushl $tornada_sem_init
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_sem_init:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_sem_init
err_sem_init:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_sem_init:
	ret

ENTRY(sem_wait)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl $22, %eax
	pushl $tornada_sem_wait
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_sem_wait:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_sem_wait
err_sem_wait:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_sem_wait:
	ret

ENTRY(sem_signal)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl $23, %eax
	pushl $tornada_sem_signal
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_sem_signal:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_sem_signal
err_sem_signal:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_sem_signal:
	ret
	
ENTRY(sem_destroy)
	pushl %ecx
	pushl %edx
	pushl %ebx
	movl 16(%esp), %ebx
	movl $24, %eax
	pushl $tornada_sem_destroy
	pushl %ebp
	movl %esp, %ebp
	sysenter
tornada_sem_destroy:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fin_sem_destroy
err_sem_destroy:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fin_sem_destroy:
	ret
