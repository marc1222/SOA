# 1 "switch.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "switch.S"
# 1 "include/asm.h" 1
# 2 "switch.S" 2

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
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

.globl get_ebp; .type get_ebp, @function; .align 0; get_ebp:
 movl %ebp, %eax
 ret

.globl set_esp; .type set_esp, @function; .align 0; set_esp:
 pushl %ebp
 movl %esp, %ebp
 movl 8(%ebp), %esp
 popl %ebp
 ret
