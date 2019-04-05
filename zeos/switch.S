#include<asm.h>

ENTRY(task_switch)
	pushl %ebp
	movl %esp, %ebp

	pushl %esi
	pushl %edi
	pushl %ebx	


	pushl 8(%ebp)
	call inner_task_switch

	popl %ebx

	popl %ebx
	popl %edi
	popl %esi

	popl %ebp
	ret

ENTRY(get_ebp)
	movl %ebp, %eax
	ret

ENTRY(set_esp)
	pushl %ebp
	movl %esp, %ebp
	movl 8(%ebp), %esp
	popl %ebp
	ret
