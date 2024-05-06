lis $20
.word 0xFFFF0004
lis $21
.word 0xFFFF000c

add $1, $0, $0

lis $10
.word 10
lis $11
.word 48
lis $12
.word 45

loop:
lw $3, 0($20)
beq $3, $10, cneg

bne $3, $12, nums
sltu $2, $3, $11
beq $0, $0, loop

nums:
multu $1, $10
mflo $1
sub $4, $3, $11
add $1, $1, $4
beq $0, $0, loop

cneg:
beq $2, $0, disp
sub $1, $0, $1

disp:
sw $1, 0($21)


jr $31