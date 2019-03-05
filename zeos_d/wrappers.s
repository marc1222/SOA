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
 pushl %esi
 pushl %ebx

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

write_end:

 popl %ebx
 popl %esi
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
 popl %ecx
 popl %edx

 movl %ebp, %esp
 popl %ebp
 ret
