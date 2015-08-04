
global triad_avx_asm_repeat
;RDI x, RSI y, RDX z, RCX n, R8 repeat
pi: dd 3.14159
align 16
section .text
    triad_avx_asm_repeat:
    shl             rcx, 2  
    add             rdi, rcx
    add             rsi, rcx
    add             rdx, rcx
    vbroadcastss    ymm2, [rel pi]
    ;neg                rcx 

align 16
.L1:
    mov             rax, rcx
    neg             rax
align 16
.L2:
    vmulps          ymm1, ymm2, [rdi+rax]
    vaddps          ymm1, ymm1, [rsi+rax]
    vmovaps         [rdx+rax], ymm1
    add             rax, 32
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    vzeroupper
    ret

global triad_avx_asm_repeat2
;RDI x, RSI y, RDX z, RCX n, R8 repeat
;pi: dd 3.14159
align 16
section .text
    triad_avx_asm_repeat2:
    shl             rcx, 2  
    vbroadcastss    ymm2, [rel pi]

align 16
.L1:
    xor             rax, rax
align 16
.L2:
    vmulps          ymm1, ymm2, [rdi+rax]
    vaddps          ymm1, ymm1, [rsi+rax]
    vmovaps         [rdx+rax], ymm1
    add             eax, 32
    cmp             eax, ecx
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    vzeroupper
    ret

global triad_avx_asm_repeat_unroll16
align 16
section .text
    triad_avx_asm_repeat_unroll16:
    shl             rcx, 2
    add             rcx, rdi
    vbroadcastss    ymm2, [rel pi]  
align 16
.L1:
    xor             rax, rax
    mov             r9, rdi
    mov             r10, rsi
    mov             r11, rdx
align 16
.L2:
    %assign unroll 16
    %assign i 0 
    %rep    unroll
        vmulps          ymm1, ymm2, [r9 + 32*i]
        vaddps          ymm1, ymm1, [r10 + 32*i]
        vmovaps         [r11 + 32*i], ymm1
    %assign i i+1 
    %endrep
    add             r9, 32*unroll
    add             r10, 32*unroll
    add             r11, 32*unroll
    cmp             r9, rcx
    jne             .L2
    sub             r8d, 1
    jnz             .L1
    vzeroupper
    ret
