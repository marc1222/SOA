# 1 "writeMSR.S"
# 1 "<built-in>"
# 1 "<command-line>"
# 31 "<command-line>"
# 1 "/usr/include/stdc-predef.h" 1 3 4
# 32 "<command-line>" 2
# 1 "writeMSR.S"
# 1 "include/asm.h" 1
# 2 "writeMSR.S" 2

.globl writeMSR; .type writeMSR, @function; .align 0; writeMSR:
 pushl %ebp
 movl %esp, %ebp
 movl 8(%ebp), %ecx
 movl 12(%ebp), %eax
 movl 16(%ebp), %edx
 wrmsr
 movl %ebp, %esp
 popl %ebp
 ret

.globl task_switch; .type task_switch, @function; .align 0; task_switch:
 pushl %ebp
 movl %esp, %ebp
 pushl %esi
 pushl %edi
 pushl %ebx
 movl 8(%ebp), %esi
 pushl %esi
 call inner_task_switch
 addl $4, %esp
 popl %ebx
 popl %edi
 popl %esi
 movl %ebp, %esp
 popl %ebp
 ret

.globl get_ebp_tswitch; .type get_ebp_tswitch, @function; .align 0; get_ebp_tswitch:
 movl %ebp, %eax
 ret

.globl set_esp_ret; .type set_esp_ret, @function; .align 0; set_esp_ret:
 pushl %ebp
 movl %esp, %ebp
 movl 8(%ebp), %esp
 popl %ebp
 ret
