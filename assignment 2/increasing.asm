lis $4
.word 4
lis $5
.word 1
lis $6
.word 1

add $10, $1, $0
add $11, $2, $0

lis $20
.word compare
sw $31, -4($30)
sub $30, $30, $4



lw $1, 0($10)

loop:
beq $5, $11, end
mult $4, $5
mflo $2
add $2, $2, $10
lw $2, 0($2)

jalr $20
beq $3, $0, end

add $1, $2, $0
add $5, $5, $6
beq $0, $0, loop

end:
add $30, $30, $4
lw $31, -4($30)
jr $31