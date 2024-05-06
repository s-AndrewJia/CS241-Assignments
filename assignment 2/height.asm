lis $3
.word 0
lis $5
.word 0

height:
sw $1, -4($30)
sw $2, -8($30)
sw $4, -12($30)
sw $6, -16($30)
sw $7, -20($30)
sw $8, -24($30)
sw $10, -28($30)
sw $11, -32($30)
sw $12, -36($30)
sw $13, -40($30)
sw $15, -44($30)
sw $20, -48($30)
sw $31, -52($30)

lis $13
.word 52
sub $30, $30, $13

lis $4
.word 4
lis $10
.word 1
lis $11
.word -1
lis $12
.word 2
lis $20
.word height

beq $5, $11, retzero

add $15, $5, $0

add $5, $15, $10
multu $5, $4
mflo $5
add $5, $5, $1
lw $5, 0($5)
jalr $20
add $6, $3, $10

add $5, $15, $12
multu $5, $4
mflo $5
add $5, $5, $1
lw $5, 0($5)
jalr $20
add $7, $3, $10

sltu $8, $7, $6
beq $8, $10, left
add $3, $7, $0
beq $0, $0, end

left:
add $3, $6, $0

end:
add $30, $30, $13
lw $1, -4($30)
lw $2, -8($30)
lw $4, -12($30)
lw $6, -16($30)
lw $7, -20($30)
lw $8, -24($30)
lw $10, -28($30)
lw $11, -32($30)
lw $12, -36($30)
lw $13, -40($30)
lw $15, -44($30)
lw $20, -48($30)
lw $31, -52($30)

jr $31


retzero:
add $3, $0, $0
beq $0, $0, end