;
; Lab10.asm
;
; Created: 9/09/2021 8:35:54 AM
; Author : Hugo Burton
;


; Replace with your application code
start:
ldi r20, 0x55
ldi r21, 0x17

mov r16, r20
and r16, r21

mov r5, r16
or r5, r21

mov r17, r20
eor r17, r21

mov r6, r20
add r6, r21

mov r7, r20
sub r7, r21

mov r8, r20
com r8

mov r9, r20
neg r9

; task 2

; copy 3 LSB of r7 to r8
ldi r22, 0b00000111
mov r8, r7
and r7, r22

ldi r23, 0xF0
and r9, r23

ldi r24, 0x0F
and r10, r24

ldi r25, 0xF0
or r11, r25