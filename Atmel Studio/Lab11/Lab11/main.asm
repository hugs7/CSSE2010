;
; Lab11.asm
;
; Created: 14/09/2021 6:53:00 PM
; Author : Hugo Burton
;


; Replace with your application code
.include "m324Adef.inc"
jmp RESET

RESET:
	ldi r16, 0
	out DDRA, r16

	ldi r16, 0xFF
	out DDRC, r16


LOOP:
	in r16, PINA
	neg r16
	out PORTC, r16
	jmp LOOP