.set STACK_SIZE, 0x800
.set FORMAT_BUFFER_SIZE, 0x100

.set FORMAT_STRING_0_LENGTH, .string_length format_string_0
.set FORMAT_STRING_1_LENGTH, .string_length format_string_1
.set FORMAT_STRING_2_LENGTH, .string_length format_string_2

.section .text

# compute fib of n, write result to stdout with format "fib(<n>) = <result>\n"
_start:
    # setup stack
    mov %rsp, stack_top

    # a = fib(10)
    mov %rdi, 0x0A
    call fib

    mov %r12, %rax  # _12 = a

    # write(1, format_string_0, FORMAT_STRING_0_LENGTH)
    mov %rdi, 0x01
    mov %rsi, format_string_0
    mov %rdx, FORMAT_STRING_0_LENGTH
    call write

    # a = itoa(10, format_buffer, FORMAT_BUFFER_SIZE)
    mov %rdi, 0x0A
    mov %rsi, format_buffer
    mov %rdx, FORMAT_BUFFER_SIZE
    call itoa

    # write(1, format_buffer, a)
    mov %rdi, 0x01
    mov %rsi, format_buffer
    mov %rdx, %rax
    call write

    # write(1, format_string_1, FORMAT_STRING_1_LENGTH)
    mov %rdi, 0x01
    mov %rsi, format_string_1
    mov %rdx, FORMAT_STRING_1_LENGTH
    call write

    # a = itoa(_12, format_buffer, FORMAT_BUFFER_SIZE)
    mov %rdi, %r12
    mov %rsi, format_buffer
    mov %rdx, FORMAT_BUFFER_SIZE
    call itoa

    # write(1, format_buffer, a)
    mov %rdi, 0x01
    mov %rsi, format_buffer
    mov %rdx, %rax
    call write

    # write(1, format_string_2, FORMAT_STRING_2_LENGTH)
    mov %rdi, 0x01
    mov %rsi, format_string_2
    mov %rdx, FORMAT_STRING_2_LENGTH
    call write

    # exit(0)
    xor %rdi, %rdi
    call exit

# int fib(int n)
fib:
    # save base pointer
    push %rbp

    # set base pointer
    mov %rbp, %rsp

    # save registers
    push %rbx

    mov %rax, 0x00  # a = 0
    mov %rbx, 0x01  # b = 1

.loop:
    cmp %rdi, 0x00
    jle .done   # if n <= 0, goto .done

    mov %rcx, %rax  # c = a
    add %rcx, %rbx  # c += b

    mov %rax, %rbx  # a = b
    mov %rbx, %rcx  # b = c

    dec %rdi  # n--

    jmp .loop

.done:
    # restore registers
    pop %rbx

    # restore base pointer
    pop %rbp

    # return
    ret

# int itoa(int value, char *buffer, int count)
itoa:
    # save base pointer
    push %rbp

    # set base pointer
    mov %rbp, %rsp

    # save registers
    push %rbx

    mov %r8, %rdx   # _8 = count

    mov %rcx, %rdx  # c = count
    dec %rcx        # c--

    mov %rax, %rdi  # a = value

    movb (%rsi, 0x00, %rcx, 0x01), 0x00 # buffer[c] = 0
    dec %rcx                            # c--

.loop:
    cmp %rcx, 0x00
    jlt .done   # if c < 0, goto .done

    xor %rdx, %rdx  # d = 0
    mov %rbx, 0x0A  # b = 10
    div %rbx        # a = a / b, d = a % b

    add %dl, '0'    # d += '0'

    mov (%rsi, 0x00, %rcx, 0x01), %dl   # buffer[c] = d
    dec %rcx                            # c--

    cmp %rax, 0x00
    jne .loop   # if a != 0, goto .loop

.done:
    mov %rax, %r8   # a = _8
    sub %rax, %rcx  # a -= c
    dec %rax        # a--

    mov %r8, %rsi   # _8 = buffer

    mov %r9, %rsi   # _9 = buffer
    add %r9, %rcx   # _9 += c
    inc %r9         # _9++

    xor %rcx, %rcx  # c = 0

.shift_loop:
    cmp %rcx, %rax
    jge .shift_done # if c >= a, goto .shift_done

    mov %bl, (%r9)  # b = *_9
    mov (%r8), %bl  # *_8 = b

    inc %r8     # _8++
    inc %r9     # _9++
    inc %rcx    # c++

.shift_done:
    # restore registers
    pop %rbx

    # restore base pointer
    pop %rbp

    # return
    ret

# void write(int fd, const void *buffer, int count)
write:
    mov %rax, 0x01  # a = 1
    syscall         # write
    ret

# void exit(int value)
exit:
    mov %rax, 0x3C  # a = 60
    syscall         # exit

.section .bss

stack_end:
    .skip STACK_SIZE
stack_top:

format_buffer:
    .skip FORMAT_BUFFER_SIZE

.section .rodata

format_string_0:
    .string "fib("
format_string_1:
    .string ") = "
format_string_2:
    .string "\n"
