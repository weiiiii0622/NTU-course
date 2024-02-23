.globl	__start

.text
__start:
    ### save ra、s0 ###                                   
    addi    sp, sp, -16
    sw      ra, 12(sp)                      
    sw      s0, 8(sp) 
    call    read_int            # [a0] = n
    jal     ra, func            # T(n)
    call    print_int
    j       exit
    
    

### n or T(n) at [a0], result at [t0], constant at [t1],[t2] ###

func:
    addi    sp, sp, -16
    sw      ra, 12(sp)           # store ra
    sw      a0, 8(sp)            # store n

    addi    t1, x0, 1            # [t2] = 1
    addi    t2, x0, 2            # [t2] = 2
    bge     a0, t2, func_loop    # n >= 2
    sgtz    a0, a0               # n > 0 ? a0 = 1 : a0 = 0
    addi    sp, sp, 16
    jalr    zero, 0(ra)    

func_loop:
    addi    a0, a0, -1           # n = n - 1
    jal     ra, func             # T(n-1)
    slli    t0, a0, 1            # [t0] = 2 * T(n-1)
    sw      t0, 4(sp)            # store left sub-tree value
    lw      a0, 8(sp)            # restore n
    addi    a0, a0, -2           # n = n - 2
    jal     ra, func             # T(n-2)
    lw      t0, 4(sp)            # restore left sub-tree value
    lw      ra, 12(sp)           # restore return addr.
    add     a0, t0, a0           # [a0] = [t0] + T(n-2)
    addi    sp, sp, 16
    jalr    zero, 0(ra)
    

read_int:
    li	    a0, 5
    ecall
    jr	    ra
    
print_int:
    mv     a1, a0
    li	   a0, 1
    ecall
    jr     ra

exit:
    ### exit handling ###
    li      a0, 0
    lw      ra, 12(sp)                      
    lw      s0, 8(sp)                       
    addi    sp, sp, 16
    li      a0, 10
    ecall