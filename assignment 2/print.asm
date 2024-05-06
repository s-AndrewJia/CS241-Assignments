sw $31, -4($30)
lis $31
.word 4
sub $30, $30, $31
lis $10
.word print
lis $1
.word 57
jalr $10
lis $1
.word -241
jalr $10
lis $1
.word 404
jalr $10
lis $31
.word 4
add $30, $30, $31
lw $31, -4($30)
jr $31

print:
sw $1, -4($30)
sw $2, -8($30)
sw $3, -12($30)
sw $4, -16($30)
sw $5, -20($30)
sw $10, -24($30)
sw $11, -28($30)
sw $15, -32($30)
sw $16, -36($30)
sw $21, -40($30)
sw $30, -44($30)
sw $31, -48($30)

lis $11
.word 48
sub $30, $30, $11

lis $4
.word 4
lis $5
.word 0

lis $10
.word 10
lis $15
.word 48
lis $16
.word 45

lis $21
.word 0xFFFF000c



slt $2, $1, $0
beq $2, $0, calc
sub $1, $0, $1
sw $16, 0($21)

calc:
divu $1, $10
mflo $1
mfhi $3
add $3, $3, $15
sw $3, -4($30)
sub $30, $30, $4
add $5, $5, $4
bne $1, $0, calc

disp:
add $30, $30, $4
lw $3, -4($30)
sw $3, 0($21)
sub $5, $5, $4
bne $5, $0, disp

sw $10, 0($21)

add $30, $30, $11
lw $1, -4($30)
lw $2, -8($30)
lw $3, -12($30)
lw $4, -16($30)
lw $5, -20($30)
lw $10, -24($30)
lw $11, -28($30)
lw $15, -32($30)
lw $16, -36($30)
lw $21, -40($30)
lw $30, -44($30)
lw $31, -48($30)

jr $31