.globl __start

.rodata
    division_by_zero: .string "division by zero"

.text
__start:
    # Read first operand
    li a0, 5
    ecall
    mv s0, a0
    # Read operation
    li a0, 5
    ecall
    mv s1, a0
    # Read second operand
    li a0, 5
    ecall
    mv s2, a0

###################################
#  TODO: Develop your calculator  #
#                                 #
###################################

li t0, 0
beq s1, t0, addition

li t0, 1 
beq s1, t0, subtract

li t0, 2
beq s1, t0, multiply

li t0, 3
beq s1, t0, divide

li t0, 4
beq s1, t0, min

li t0, 5
beq s1, t0, power

li t0, 6
beq s1, t0, factorial

output:
    # Output the result
    li a0, 1
    mv a1, s3
    ecall

exit:
    # Exit program(necessary)
    li a0, 10
    ecall

division_by_zero_except:
    li a0, 4
    la a1, division_by_zero
    ecall
    jal zero, exit
    
addition:
    add s3, s0, s2
    jal zero, output
    
subtract:
    sub s3, s0, s2
    jal zero, output

multiply:
    mul s3, s0, s2
    jal zero, output
    
divide:
    beq s2, x0, division_by_zero_except
    div s3, s0, s2
    jal zero, output
    
min:
    add s3, s0, x0
    blt s0, s2, output
    add s3, s2, x0
    jal zero, output
    
power:
    li t0, 0
    li s3, 1
    jal zero, power_help
    
power_help:
    beq t0, s2, output
    addi t0, t0, 1
    mul s3, s3, s0
    jal zero, power_help

factorial:
    add t0, s0, x0
    li s3, 1
    jal zero, factorial_help
    
factorial_help:
    beqz t0, output
    mul s3, s3, t0
    addi t0, t0, -1
    jal zero, factorial_help
    