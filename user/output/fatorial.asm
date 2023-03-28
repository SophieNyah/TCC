.data:
__literal_0: .asciiz "fatorial: "

.text:



main:
li $s7, 5
li $s6, 0
subi $sp, $sp, 8  #store stack
sw $s7, 0($sp)  #store stack
sw $s6, 4($sp)  #store stack
subi $sp, $sp, 4
sw $a0, 0($sp)
move $a0, $s7
jal fat
lw $a0, 0($sp)
addi $sp, $sp, 4
lw $s6, 4($sp)  #retrieve stack
lw $s7, 0($sp)  #retrieve stack
addi $sp, $sp, 8  #retrieve stack
move $s6, $v0
subi $sp, $sp, 4
sw $a0, 0($sp)
li $v0, 4
la $a0, __literal_0
syscall
lw $a0, 0($sp)
addi $sp, $sp, 4
subi $sp, $sp, 4
sw $a0, 0($sp)
li $v0, 1
la $a0, ($s6)
syscall
lw $a0, 0($sp)
addi $sp, $sp, 4
li $v0, 10
li $a0, 0
syscall


fat:
subi $sp, $sp, 4
sw $ra, 0($sp)
subi $t5, $a0, 1
move $s5, $t5
seq $t6, $a0, 1
beq $t6, $zero, if_end_1
li $v0, 1
# clear stack
lw $ra, 0($sp)
addi $sp, $sp, 4
jr $ra
if_end_1:
subi $sp, $sp, 12  #store stack
sw $s7, 0($sp)  #store stack
sw $s5, 4($sp)  #store stack
sw $s6, 8($sp)  #store stack
subi $sp, $sp, 4
sw $a0, 0($sp)
move $a0, $s5
jal fat
lw $a0, 0($sp)
addi $sp, $sp, 4
lw $s6, 8($sp)  #retrieve stack
lw $s5, 4($sp)  #retrieve stack
lw $s7, 0($sp)  #retrieve stack
addi $sp, $sp, 12  #retrieve stack
mult $a0, $v0
mflo $t6
move $v0, $t6
# clear stack
lw $ra, 0($sp)
addi $sp, $sp, 4
jr $ra
