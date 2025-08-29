    .globl   myadd
    .globl   mycount
    .globl   _Z9mycounty
    .p2align 2
    .type    myadd,%function
    .type    mycount,%function
    .type    _Z9mycounty,%function
myadd:                     // Function "myadd" entry point.
    .fnstart
    add      r0, r0, r1   // Function arguments are in R0 and R1. Add together and put the result in R0.
    bx       lr           // Return by branching to the address in the link register.
    .fnend

mycount:
    .fnstart
    mov     r1, #0
    sub     r2, r0, #1
    ands    r0, r2
    addpl   r1, #1
    subne   pc, #20
    mov     r0, r1
    bx      lr
    .fnend

_Z9mycounty:
    .fnstart
    mov     r0, #444
    bx      lr
    .fnend

#r = y ^ ((x ^ y) & -(x < y)); // min(x, y)
