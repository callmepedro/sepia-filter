global sepia_four

section .rodata
align 16
c1: dd 0.272, 0.349, 0.393, 0.272
align 16
c2: dd 0.543, 0.686, 0.769, 0.543
align 16
c3: dd 0.131, 0.168, 0.189, 0.131
align 16
maxval: dd 255.0, 255.0, 255.0, 255.0


%macro set_batch 1
    movdqu xmm0, [rdi]
    movdqu xmm1, [rdi+16]
    movdqu xmm2, [rdi+32]

    shufps xmm0, xmm0, %1
    shufps xmm1, xmm1, %1
    shufps xmm2, xmm2, %1
%endmacro

%macro set_factor 1
    movdqu xmm3, [c1]
    movdqu xmm4, [c2]
    movdqu xmm5, [c3]

    shufps xmm3, xmm3, %1
    shufps xmm4, xmm4, %1
    shufps xmm5, xmm5, %1
%endmacro

%macro calc_and_save 1
    mulps xmm0, xmm3
    mulps xmm1, xmm4
    mulps xmm2, xmm5
    addps xmm0, xmm1
    addps xmm0, xmm2

    minps xmm0, [maxval]
    cvtps2dq xmm0, xmm0

    pextrb [rsi + %1*4], xmm0, 0
    pextrb [rsi+1 + %1*4], xmm0, 4
    pextrb [rsi+2 + %1*4], xmm0, 8
    pextrb [rsi+3 + %1*4], xmm0, 12
%endmacro

%define batch_mask1 0xA5
%define batch_mask2 0x40
%define batch_mask3 0xFE
%define factor_mask1 0x24
%define factor_mask2 0x49
%define factor_mask3 0x92

section .text
sepia_four:
    set_batch batch_mask1
    set_factor factor_mask1
    calc_and_save 0

    set_batch batch_mask2
    set_factor factor_mask2
    calc_and_save 1

    set_batch batch_mask3
    set_factor factor_mask3
    calc_and_save 2

    ret