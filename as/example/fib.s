.section .text

_start:


    mov %rax, 60
    xor %rdi, %rdi
    syscall

fib:
    pushq %rbp
    mov %rbp, %rsp
    pushq %rbx
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    subq %rsp, 12

    mov (%rsp,)

    addq %rsp, 12

    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %rbx
    popq %rbp
    ret
