.text
pushq % rbp
movq % rsp , % rbp
subq $32 , % rsp
movl $1 , -4(% rbp)
movl $2 , -8(% rbp)

movl $1, %r10d
movl -8(% rbp) , %r11d
addl %r11d , %r10d
movl %r11d , -12(% rbp)

movl -12(% rbp), %eax

# fim
leave
ret
