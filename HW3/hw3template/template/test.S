main:
        addi    sp,sp,-48
        lui     a0,%hi(_ZSt3cin)
        sw      s0,40(sp)
        addi    a1,sp,8
        addi    s0,sp,16
        addi    a0,a0,%lo(_ZSt3cin)
        sw      ra,44(sp)
        sw      s1,36(sp)
        sw      s0,8(sp)
        sw      zero,12(sp)
        sb      zero,16(sp)
        call    std::basic_istream<char, std::char_traits<char> >& std::operator>><char, std::char_traits<char>, std::allocator<char> >(std::basic_istream<char, std::char_traits<char> >&, std::__cxx11::basic_string<char, std::char_traits<char>, std::allocator<char> >&)
        lw      t3,12(sp)
        blez    t3,.L25
        lw      a6,8(sp)
        sub     a7,zero,t3
        li      t5,-1
        mv      t4,a6
        li      a4,0
        li      t0,0
        li      t1,-1
.L20:
        mv      a0,t5
        addi    t6,t0,1
        beq     t5,t1,.L3
.L38:
        ble     t3,t6,.L4
        lbu     t2,1(t4)
        lbu     a3,-1(t4)
        mv      a1,t6
        mv      a5,t5
        beq     a3,t2,.L5
        j       .L37
.L7:
        mv      a3,a7
        beq     a5,a7,.L6
        add     a2,a6,a5
        add     a3,a6,a1
        lbu     a2,0(a2)
        lbu     a3,0(a3)
        bne     a2,a3,.L27
.L5:
        addi    a5,a5,-1
        addi    a1,a1,1
        bne     a5,t1,.L7
        li      a3,-1
.L6:
        sub     a1,a1,a3
        addi    a1,a1,-1
        bge     a1,a4,.L22
        mv      a1,a4
.L22:
        lbu     a5,0(t4)
        bne     a5,t2,.L29
        addi    a5,t0,2
        ble     t3,a5,.L10
        lbu     a3,2(t4)
        lbu     a4,-1(t4)
        bne     a3,a4,.L10
        addi    a0,t0,-2
        addi    a5,t0,3
        bltz    a0,.L13
.L12:
        beq     t3,a5,.L10
        add     a3,a6,a0
        add     a4,a6,a5
        lbu     a3,0(a3)
        lbu     a4,0(a4)
        bne     a3,a4,.L10
        addi    a0,a0,-1
        addi    a5,a5,1
        bne     a0,t1,.L12
.L13:
        li      a0,-1
.L10:
        sub     a5,a5,a0
        addi    a4,a5,-1
        addi    t5,t5,1
        addi    t4,t4,1
        addi    a7,a7,2
        mv      t0,t6
        bge     a4,a1,.L20
        mv      a4,a1
        mv      a0,t5
        addi    t6,t0,1
        bne     t5,t1,.L38
.L3:
        mv      a1,a4
        blez    a4,.L39
.L8:
        ble     t3,t6,.L2
        lbu     a4,1(t4)
        lbu     a5,0(t4)
        bne     a4,a5,.L29
        addi    a5,t0,2
        j       .L10
.L27:
        mv      a3,a5
        j       .L6
.L29:
        mv      a5,t6
        mv      a0,t0
        j       .L10
.L25:
        li      a1,0
.L2:
        lui     a0,%hi(_ZSt4cout)
        addi    a0,a0,%lo(_ZSt4cout)
        call    std::basic_ostream<char, std::char_traits<char> >::operator<<(int)
        call    std::basic_ostream<char, std::char_traits<char> >& std::endl<char, std::char_traits<char> >(std::basic_ostream<char, std::char_traits<char> >&)
        lw      a0,8(sp)
        beq     a0,s0,.L31
        call    operator delete(void*)
.L31:
        lw      ra,44(sp)
        lw      s0,40(sp)
        lw      s1,36(sp)
        li      a0,0
        addi    sp,sp,48
        jr      ra
.L4:
        mv      a1,a4
        bgtz    a4,.L2
        li      a1,1
        j       .L2
.L37:
        mv      a3,t5
        j       .L6
.L39:
        li      a1,1
        j       .L8
        lw      a5,8(sp)
        mv      s1,a0
        beq     a5,s0,.L18
        mv      a0,a5
        call    operator delete(void*)
.L18:
        mv      a0,s1
        call    _Unwind_Resume
_GLOBAL__sub_I_main:
        addi    sp,sp,-16
        sw      s0,8(sp)
        lui     s0,%hi(std::__ioinit)
        addi    a0,s0,%lo(std::__ioinit)
        sw      ra,12(sp)
        call    std::ios_base::Init::Init() [complete object constructor]
        addi    a1,s0,%lo(std::__ioinit)
        lw      s0,8(sp)
        lw      ra,12(sp)
        lui     a2,%hi(__dso_handle)
        lui     a0,%hi(_ZNSt8ios_base4InitD1Ev)
        addi    a2,a2,%lo(__dso_handle)
        addi    a0,a0,%lo(_ZNSt8ios_base4InitD1Ev)
        addi    sp,sp,16
        tail    __cxa_atexit
std::__ioinit:
        .zero   1
DW.ref.__gxx_personality_v0:
        .word   __gxx_personality_v0