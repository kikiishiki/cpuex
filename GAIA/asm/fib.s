fib_10:
    sub     rsp, rsp, r0, 2
    mov     r2, 1
    ble     r1, r2, L1
    sub     r2, r1, r0, 1
    mov     [rbp - 1], r1
    mov     r1, r2
    call    fib_10
    mov     r2, [rbp - 1]
    sub     r2, r2, r0, 2
    mov     [rbp - 2], r1
    mov     r1, r2
    call    fib_10
    mov     r2, [rbp - 2]
    add     r1, r2, r1, 0
    ret
L1:
    ret
.global main
main:
    mov     r1, 30
    call    fib_10
#    call    print_int
    halt
