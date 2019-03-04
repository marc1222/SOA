#include<asm.h>

ENTRY(write_wrapper)
	pushl %ebp
	movl %esp, %ebp

	// Save edx and ecx
	pushl %edx
	pushl %ecx
	pushl %ebx

	movl $4, %eax
	movl 8(%ebp), %ebx
	movl 12(%ebp), %ecx
	movl 16(%ebp), %edx

	pushl write_ret

	pushl %ebp
	movl %esp, %ebp
	
	sysenter


write_ret: 
	movl %ebp, %esp
	popl %ebp

	cmpl $0, %eax
	jge write_end
	neg %eax
	movl %eax, errno
	
write_end:

	popl %esi
	popl %ebx
	popl %ecx
	popl %edx

	movl %ebp, %esp
	popl %ebp
	ret
	

		 
