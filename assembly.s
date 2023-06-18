.text
pushq % rbp
movq % rsp , % rbp
subq $32 , % rsp
movl $1 , -4(% rbp)

# coloca v1 no eax
movl -4(% rbp) , % eax
leave
ret
