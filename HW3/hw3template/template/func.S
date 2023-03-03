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
    # t2 is the maxlen
    li t2 0
    # use t3 for loop i
    li t3 0
loop:
    bgt t3 t1 loopend
    # t4 is current length
    li t4 1
    # t5 for loop j
    li t5 1
    # consider odd length
loop1:
    blt t3 t5 loop1end
    add s1 t3 t5
    bgt s1 t1 loop1end
    # s2 is the left index
    sub s2 t3 t5
    # s3 is the right index
    add s3 t3 t5
    # load char to s2
    add s2 a0 s2
    lb s2 0(s2)
    # load char to s3
    add s3 a0 s3
    lb s3 0(s3)
    # check if they are equal
    bne s2 s3 loop1end
    # add 2 to t4
    addi t4 t4 2
    # add 1 to t5
    addi t5 t5 1
    # jump to loop1
    j loop1
loop1end:
    # get the max length between t4 and t2
max1:
    bge t2 t4 max1end
    mv t2 t4
max1end:
    # consider even length
    # let t4 be 0
    li t4 0
    # t5 for loop j
    li t5 0
    # consider odd length
loop2:
    blt t3 t5 loop2end
    add s1 t3 t5
    bgt s1 t1 loop2end
    # s2 is the left index
    sub s2 t3 t5
    # s3 is the right index
    add s3 t3 t5
    addi s3 s3 1
    # load char to s2
    add s2 a0 s2
    lb s2 0(s2)
    # load char to s3
    add s3 a0 s3
    lb s3 0(s3)
    # check if they are equal
    bne s2 s3 loop2end
    # add 2 to t4
    addi t4 t4 2
    # add 1 to t5
    addi t5 t5 1
    # jump to loop2
    j loop2
loop2end:
    # get the max length between t4 and t2
max2:
    bge t2 t4 max2end
    mv t2 t4
max2end:
    # add 1 to t3
    addi t3 t3 1
    j loop
loopend:
    # t2 is the maxlen
    mv a0 t2
    ret