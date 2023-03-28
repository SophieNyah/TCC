.data:
__literal_0: .asciiz "soma: "

.text:



main:
subi $sp, $sp, 20
move $s7, $sp
li $s6, 1
li $s5, 0
li $s4, 5
lw $t3, 0($s7)
li $t4, 7
li $t3, 0
addi $t3, $s7, 0
sw $t4, ($t3)
lw $t7, 4($s7)
li $t3, 5
li $t7, 4
addi $t7, $s7, 4
sw $t3, ($t7)
lw $t6, 8($s7)
li $t7, 33
li $t6, 8
addi $t6, $s7, 8
sw $t7, ($t6)
lw $t5, 12($s7)
li $t6, 10
li $t5, 12
addi $t5, $s7, 12
sw $t6, ($t5)
lw $t4, 16($s7)
li $t5, 2
li $t4, 16
addi $t4, $s7, 16
sw $t5, ($t4)
for_init_1:
li $s5, 0
for_check_1:
slt $t6, $s5, $s4
beq $t6, $zero, for_end_1
j for_body_1
for_update_1:
addi $s5, $s5, 1
j for_check_1
for_body_1:
sll $t4, $s5, 2
add $t4, $s7, $t4
lw $t4, ($t4)
mult $s6, $t4
mflo $t7
move $s6, $t7
j for_update_1
for_end_1:
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
