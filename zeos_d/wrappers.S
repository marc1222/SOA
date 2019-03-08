#include<asm.h>

ENTRY(write)
	pushl %ebp
	movl %esp, %ebp

	pushl %edx
	pushl %ecx
	pushl %ebx
	pushl %esi

	movl $4, %eax
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx

	leal write_ret, %esi
	pushl %esi

	pushl %ebp
	movl %esp, %ebp
	
	sysenter

write_ret: 

	popl %ebp

	cmpl $0, %eax
	jge write_end
	neg %eax
	movl %eax, errno
	movl $-1, %eax

	
write_end:
	popl %esi

	popl %esi
	popl %ebx
	popl %ecx
	popl %edx

	movl %ebp, %esp
	popl %ebp
	ret
	

		 
ENTRY(gettime)
	pushl %ebp
	movl %esp, %ebp

	pushl %edx
	pushl %ecx
	pushl %esi

	movl $10, %eax

	leal gettime_ret, %esi
	pushl %esi

	pushl %ebp
	movl %esp, %ebp
	
	sysenter

gettime_ret:

	popl %ebp

	popl %esi

	popl %esi
	popl %ecx
	popl %edx

	movl %ebp, %esp
	popl %ebp
	ret






