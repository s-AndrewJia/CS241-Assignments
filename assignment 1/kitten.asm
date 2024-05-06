lis $1
.word 0xFFFF0004
lis $2
.word 0xFFFF000c
lis $3
.word 0
lis $4
.word 0xffffffff
lis $5
.word 1

loop:
lw $6, 0($1)
beq $6, $4, end
add $3, $3, $5
sw $6, 0($2)
beq $0, $0, loop

end:
jr $31