lis $10
.word 0xFFFF0004
lis $11
.word 0xFFFF000c
lis $12
.word 0xffffffff

lis $1
.word 1
lis $4
.word 4

add $3, $0, $0

loop:
lw $2, 0($10)
beq $2, $12, out
add $3, $3, $1
sw $2, -4($30)
sub $30, $30, $4
beq $0, $0, loop

out:
add $5, $3, $0

disp:
add $30, $30, $4
lw $6, -4($30)
sw $6, 0($11)
sub $5, $5, $1
bne $5, $0, disp

jr $31