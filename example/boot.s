.set ALIGN,    (.shl 1, 0)
.set MEMINFO,  (.shl 1, 1)
.set FLAGS,    (.or ALIGN, MEMINFO)
.set MAGIC,    0x1BADB002
.set CHECKSUM, (.neg (.add MAGIC, FLAGS))

.section .multiboot
.align 4
.dword MAGIC
.dword FLAGS
.dword CHECKSUM

.section .bss
.align 16
stack_bottom:
.skip 16384
stack_top:

.section .text
.extern _start
_start:
    mov %esp, $stack_top
    call kernel_main
    cli
.loop:
    hlt
    jmp .loop
