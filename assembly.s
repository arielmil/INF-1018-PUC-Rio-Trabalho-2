.text
.globl main

main:

    /* testando todos os comandos necessários para operações aritiméticas e algumas adicionais */
    pushq %rbp
    movq %rsp, %rbp

    subq $32, %rsp

    movl %edi, -4(%rbp)

    movl -4(%rbp), %r10d
    movl $1, %r11d
    addl %r10d, %r11d
    movl %r11d, -4(%rbp)

    movl -4(%rsp), %r10d
    cmpl $0, %r10d
    jle end

resto:

    movl $0, %eax
    leave
    ret


end:
    movl $1, %eax
    leave
    ret

movl -12(% rbp), %eax

# fim
leave
ret
