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

ENTRY(getpid)
	pushl %ecx
	pushl %edx
	movl $20, %eax
	pushl $getpid_ret
	pushl %ebp
	movl %esp, %ebp
	sysenter
getpid_ret:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge getpid_end
err_getpid:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
getpid_end:
	ret

ENTRY(fork)
	pushl %ecx
	pushl %edx
	movl $2, %eax
	pushl $fork_ret
	pushl %ebp
	movl %esp, %ebp
	sysenter
fork_ret:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge fork_end
err_fork:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
fork_end:
	ret

ENTRY(exit)
	pushl %ecx
	pushl %edx
	movl $1, %eax
	pushl $exit_ret
	pushl %ebp
	movl %esp, %ebp
	sysenter
exit_ret:
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
	pushl $get_stats_ret
	pushl %ebp
	movl %esp, %ebp
	sysenter
get_stats_ret:
	movl %ebp, %esp
	popl %ebp
	addl $4, %esp
	popl %ebx
	popl %edx
	popl %ecx
	cmpl $0, %eax
	jge get_stats_end
err_get_stats:
	negl %eax
	movl %eax, errno
	movl $-1, %eax
get_stats_end:
	ret

