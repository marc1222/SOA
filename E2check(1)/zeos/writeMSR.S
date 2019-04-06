#include <asm.h>

ENTRY(writeMSR)
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %ecx
	movl 12(%ebp), %eax
	movl 16(%ebp), %edx
	wrmsr
	movl %ebp, %esp
	popl %ebp
	ret

ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp
	pushl %esi
	pushl %edi
	pushl %ebx
	movl 8(%ebp), %esi // passem el parametre
	pushl %esi
	call inner_task_switch
	addl $4, %esp // esborrem el parametre
	popl %ebx
	popl %edi
	popl %esi
	movl %ebp, %esp
	popl %ebp
	ret

ENTRY(get_ebp_tswitch)
	movl %ebp, %eax
	ret

ENTRY(set_esp_ret)
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %esp
	popl %ebp
	ret
