stirling:
sw $1, -4($30)
sw $2, -8($30)
sw $5, -12($30)
sw $10, -16($30)
sw $11, -20($30)
sw $20, -24($30)
sw $31, -28($30)

lis $11
.word 28
sub $30, $30, $11

lis $10
.word 1

lis $20
.word stirling



beq $1, $0, retzero
beq $2, $0, retzero

sub $1, $1, $10
jalr $20
mult $3, $1
mflo $5
sub $2, $2, $10
jalr $20
add $3, $5, $3

end:
add $30, $30, $11
lw $1, -4($30)
lw $2, -8($30)
lw $5, -12($30)
lw $10, -16($30)
lw $11, -20($30)
lw $20, -24($30)
lw $31, -28($30)

jr $31

retzero:
beq $1, $2, retone
add $3, $0, $0
beq $0, $0, end

retone:
add $3, $10, $0
beq $0, $0, end