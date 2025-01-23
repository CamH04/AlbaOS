.global apm_bios_call
.code32

apm_bios_call:
    push %ebp
    mov %esp, %ebp
    push %ebx
    push %ecx
    push %edx

    mov 8(%ebp), %ax
    mov 12(%ebp), %bx
    mov 16(%ebp), %cx
    mov 20(%ebp), %dx

    int $0x15

    lahf               # load flags into AH
    movzwl %ax, %eax

    pop %edx
    pop %ecx
    pop %ebx
    pop %ebp
    ret
