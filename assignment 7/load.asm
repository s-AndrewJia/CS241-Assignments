.import readWord
.import printHex

lis $4
.word 4

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

jalr $20
add $5, $3, $0      ; length in bytes of header + MIPS program

lis $6
.word 0xc

loop:
beq $5, $6, setup2
jalr $20
sw $3, 0($8)
add $1, $3, $0
jalr $21
add $6, $6, $4
add $8, $8, $4
beq $0, $0, loop

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