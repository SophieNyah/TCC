.data:
__literal_0: .asciiz "fibonacci: "

.text:



main:
li $s7, 9
li $s6, 0
li $s5, 1
li $s4, 0
li $s3, 0
for_init_1:
li $s3, 0
for_check_1:
slt $t5, $s3, $s7
beq $t5, $zero, for_end_1
j for_body_1
for_update_1:
addi $s3, $s3, 1
j for_check_1
for_body_1:
add $t6, $s6, $s5
move $s4, $t6
move $s6, $s5
move $s5, $s4
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
la $a0, ($s5)
syscall
lw $a0, 0($sp)
addi $sp, $sp, 4
li $v0, 10
li $a0, 0
syscall
