.text
pushq % rbp
movq % rsp , % rbp
subq $32 , % rsp
movl $1 , -4(% rbp)
movl $2 , -8(% rbp)

# coloca v1 no v3
movl -4(% rbp) , % eax
movl % eax , -12(% rbp)

# soma v2 ao v3
addl -16(% rbp) , %eax
movl % eax , -12(% rbp)

#movl -12(% rbp), %eax

# fim
leave
ret
