# DO NOT REMOVE THIS LINE WHEN SUBMITTING
.globl LPS

LPS:
    # YOUR CODE HERE
    # a var that store the string length
    li t0 0
    # find the string length
strlen:
    # load char to t1
    add t1 t0 a0
    # dereference t1
    lb t1 0(t1)
    # check if it is zero
    beq t1 zero strlenend
    # not zero, add 1 to t0
    addi t0 t0 1
    # jump to strlen
    j strlen
strlenend:
    # t1 is the length of string n
    mv t1 t0
    # t2 is the res
    li t2 0
    # use t3 for loop i
    li t3 0
loop:
    bgt t3 t1 loopend
    # t4 is left index
    addi t4 t3 -1
    # t5 is right index
    addi t5 t3 1
    # consider odd length
loop1:
    blt t4 zero loop1end
    bgt t5 t1 loop1end
    # compare the left and right char
    # a2 is the left char
    add a2 a0 t4
    lb a2 0(a2)
    # a3 is the right char
    add a3 a0 t5
    lb a3 0(a3)
    # check if they are equal
    bne a2 a3 loop1end
    # l--
    addi t4 t4 -1
    # r++
    addi t5 t5 1
    # jump to loop1
    jal x0 loop1
loop1end:
    # get the max length between res and r-l-1
max1:
    # t4 is r-l-1
    sub t4 t5 t4
    addi t4 t4 -1
    # t2 is res
    bge t2 t4 max1end
    mv t2 t4
max1end:
    # let l = i
    mv t4 t3
    # let r = i+1
    addi t5 t3 1
loop2:
    blt t4 x0 loop2end
    bgt t5 t1 loop2end
    # compare the left and right char
    # a2 is the left char
    add a2 a0 t4
    lb a2 0(a2)
    # a3 is the right char
    add a3 a0 t5
    lb a3 0(a3)
    # check if they are equal
    bne a2 a3 loop2end
    # l--
    addi t4 t4 -1
    # r++
    addi t5 t5 1
    # jump to loop2
    jal x0 loop2
loop2end:
    # get the max length between t4 and t2
max2:
    # t4 is r-l-1
    sub t4 t5 t4
    addi t4 t4 -1
    # t2 is res
    bge t2 t4 max2end
    mv t2 t4
max2end:
    # let l = i
    mv t4 t3
    # let r = i+1
    addi t5 t3 1
    # i++
    addi t3 t3 1
    # jump to loop
    jal x0 loop
loopend:
    # t2 is the maxlen
    mv a0 t2
    ret