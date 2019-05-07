# 1 "wrappers.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "wrappers.S"
# 1 "include/asm.h" 1
# 2 "wrappers.S" 2

.globl write; .type write, @function; .align 0; write:
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



.globl gettime; .type gettime, @function; .align 0; gettime:
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

.globl getpid; .type getpid, @function; .align 0; getpid:
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

.globl fork; .type fork, @function; .align 0; fork:
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

.globl exit; .type exit, @function; .align 0; exit:
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

.globl get_stats; .type get_stats, @function; .align 0; get_stats:
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

.globl uselessMatrix; .type uselessMatrix, @function; .align 0; uselessMatrix:
 pushl %ecx
 pushl %edx
 pushl %ebx
 movl 16(%esp), %ebx
 movl 20(%esp), %ecx
 movl $38, %eax
 pushl $uselessMatrix_ret
 pushl %ebp
 movl %esp, %ebp
 sysenter
uselessMatrix_ret:
 movl %ebp, %esp
 popl %ebp
 addl $4, %esp
 popl %ebx
 popl %edx
 popl %ecx
 ret
