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
jalr $20

jalr $20
add $5, $3, $0      ; length in bytes of header + MIPS program

lis $6
.word 0xc

loop:
beq $5, $6, end
jalr $20
add $1, $3, $0
jalr $21
add $6, $6, $4
beq $0, $0, loop

end:
add $30, $30, $4
lw $31, -4($30)
jr $31