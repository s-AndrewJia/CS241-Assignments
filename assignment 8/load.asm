.import readWord
.import printHex

lis $4
.word 4

lis $11
.word 1

lis $20
.word readWord
lis $21
.word printHex

sw $31, -4($30)
sub $30, $30, $4

jalr $20
add $7, $3, $0      ; $7 is address alpha
add $8, $7, $0

jalr $20
jalr $20
add $10, $3, $0

jalr $20
add $5, $3, $0      ; length in bytes of header + MIPS program

lis $6
.word 0xc

lis $16
.word 0xc

loop:
beq $5, $6, rel
jalr $20
sw $3, 0($8)
add $1, $3, $0
jalr $21
add $6, $6, $4
add $8, $8, $4
beq $0, $0, loop


rel:
jalr $20
bne $3, $11, run
jalr $20
add $3, $3, $7
sub $3, $3, $16
lw $9, 0($3)
add $9, $9, $7
sub $9, $9, $16
sw $9, 0($3)
add $6, $6, $4
beq $0, $0, rel




run:
add $8, $7, $0

sw $4, -4($30)
sub $30, $30, $4
sw $20, -4($30)
sub $30, $30, $4
sw $21, -4($30)
sub $30, $30, $4
sw $7, -4($30)
sub $30, $30, $4
sw $8, -4($30)
sub $30, $30, $4
sw $5, -4($30)
sub $30, $30, $4
sw $6, -4($30)
sub $30, $30, $4
sw $9, -4($30)
sub $30, $30, $4
sw $10, -4($30)
sub $30, $30, $4
sw $11, -4($30)
sub $30, $30, $4
sw $16, -4($30)
sub $30, $30, $4

jalr $8

lis $30
.word 0x01000000
lis $31
.word 48
sub $30, $30, $31
lis $31
.word 4

add $30, $30, $31
lw $16, -4($30)
add $30, $30, $31
lw $11, -4($30)
add $30, $30, $31
lw $10, -4($30)
add $30, $30, $31
lw $9, -4($30)
add $30, $30, $31
lw $6, -4($30)
add $30, $30, $31
lw $5, -4($30)
add $30, $30, $31
lw $8, -4($30)
add $30, $30, $31
lw $7, -4($30)
add $30, $30, $31
lw $21, -4($30)
add $30, $30, $31
lw $20, -4($30)
add $30, $30, $31
lw $4, -4($30)

setup2:
lis $6
.word 0xc
add $8, $7, $0

loop2:
beq $5, $6, end
lw $1, 0($8)
jalr $21
add $6, $6, $4
add $8, $8, $4
beq $0, $0, loop2

end:
add $30, $30, $4
lw $31, -4($30)
jr $31