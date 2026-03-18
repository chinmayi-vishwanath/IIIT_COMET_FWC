.include "m328pdef.inc"

.org 0x0000
rjmp RESET

RESET:
    ; PD0–PD3 input
    ldi r16, 0x00
    out DDRD, r16

    ; PORTC output (7 segment)
    ldi r16, 0xFF
    out DDRC, r16

MAIN:
    in r17, PIND
    andi r17, 0x0F

    cpi r17, 2
    breq SHOW1
    cpi r17, 3
    breq SHOW1
    cpi r17, 5
    breq SHOW1
    cpi r17, 7
    breq SHOW1
    cpi r17, 8
    breq SHOW1
    cpi r17, 9
    breq SHOW1
    cpi r17, 12
    breq SHOW1

    rjmp SHOW0

SHOW1:
    ; Digit 1 (segments b,c)
    ldi r18, 0x06
    out PORTC, r18
    rjmp MAIN

SHOW0:
    ; Digit 0
    ldi r18, 0x3F
    out PORTC, r18
    rjmp MAIN
	
