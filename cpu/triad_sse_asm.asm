
global triad_sse_asm_repeat
;RDI x, RSI y, RDX z, RCX n, R8 repeat
pi: dd 3.14159
;align 16
section .text
    triad_sse_asm_repeat:
    shl             rcx, 2  
    add             rdi, rcx
    add             rsi, rcx
    add             rdx, rcx
    movss           xmm2, [rel pi]
    shufps          xmm2, xmm2, 0
    ;neg                rcx 
align 16
.L1:
    mov             rax, rcx
    neg             rax
align 16
.L2:
    movaps          xmm1, [rdi+rax]
    mulps           xmm1, xmm2
    addps           xmm1, [rsi+rax]
    movaps          [rdx+rax], xmm1
    add             rax, 16
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    ret

global triad_sse_asm_repeat2
;RDI x, RSI y, RDX z, RCX n, R8 repeat
;pi: dd 3.14159
;align 16
section .text
    triad_sse_asm_repeat2:
    shl             rcx, 2  
    movss           xmm2, [rel pi]
    shufps          xmm2, xmm2, 0
align 16
.L1:
    xor             rax, rax
align 16
.L2:
    movaps          xmm1, [rdi+rax]
    mulps           xmm1, xmm2
    addps           xmm1, [rsi+rax]
    movaps          [rdx+rax], xmm1
    add             eax, 16
    cmp             eax, ecx
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    ret



global triad_sse_asm_repeat_unroll16
section .text
    triad_sse_asm_repeat_unroll16:
    shl             rcx, 2
    add             rcx, rdi
    movss           xmm2, [rel pi]
    shufps          xmm2, xmm2, 0
.L1:
    xor             rax, rax
    mov             r9, rdi
    mov             r10, rsi
    mov             r11, rdx
.L2:
    %assign unroll 8
    %assign i 0 
    %rep    unroll
        movaps          xmm1, [r9 + 16*i]
        mulps           xmm1, xmm2,
        addps           xmm1, [r10 + 16*i]
        movaps          [r11 + 16*i], xmm1
    %assign i i+1 
    %endrep
    add             r9, 16*unroll
    add             r10, 16*unroll
    add             r11, 16*unroll
    cmp             r9, rcx
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    ret
