; Relocatable template copied verbatim to generated offset zero. All jumps are
; PC relative; all literal/pointer fixups are relative to rt_base, never DULL.
RtConst MACRO reg,value
        LRM reg,[R12+literal-rt_base-8000h]
        JMP next
literal: DD value
next:
        ENDM
RtCall MACRO target
        LRM R7,[R12+return_word-rt_base-8000h]
        PSH R7
        JMP target
return_word: DD 18000000h+((returned-rt_return_slot)/4)
returned:
        ENDM
RtSys MACRO function
        ADI R1,RZR,function
        RtConst R7,resume-rt_base
gate:   DD 18000000h+((0fff0h-(gate-rt_base))/4)
resume:
        ENDM
; The JMP to the gate is emitted directly: the template's original segment
; position is irrelevant.
rt_base:
        JMP rt_start
rt_return:
        POP R7
rt_return_register:
        SRM R7,[R12+rt_return_slot-rt_base-8000h]
rt_return_slot:
        JMP rt_return_slot
rt_start:
        DosConst R12,88000h
        RtConst R11,0d0000h
        ADD R3,R11,RZR
        ADI R4,RZR,256
rt_zero_globals:
        SRM RZR,[R3]
        ADI R3,R3,4
        ADI R4,R4,-1
        SRZ R4
        JMP rt_zero_globals
        ADI R10,RZR,0
        RtConst R13,0d0400h
        SRM RZR,[R13]
rt_main_jump:
        JMP rt_main_jump
rt_finish:
        ADI R2,RZR,0
        RtSys DOS_EXIT
rt_putchar:
        RtSys DOS_PUTCHAR
        SRZ R0
        JMP rt_io_error
        JMP rt_return
rt_putstr:
        RtSys DOS_PUTS
        SRZ R0
        JMP rt_io_error
        JMP rt_return
rt_newline:
        ADI R2,RZR,13
        RtCall rt_putchar
        ADI R2,RZR,10
        RtCall rt_putchar
        JMP rt_return
; Normalized signed16 invariant is maintained after constants and arithmetic.
rt_wrap:
        RtConst R0,0ffffh
        ANL R2,R2,R0
        RtConst R0,08000h
        ANL R0,R2,R0
        SRZ R0
        JMP rt_wrap_negative
        JMP rt_return
rt_wrap_negative:
        RtConst R0,0ffff0000h
        ORR R2,R2,R0
        JMP rt_return
; R3 left, R2 right. Fixed 16 iterations, low16 product.
rt_multiply:
        ADD R4,R3,RZR
        ADD R5,R2,RZR
        ADI R2,RZR,0
        ADI R0,RZR,1
        ADI R1,RZR,16
rt_multiply_bit:
        ANL R3,R5,R0
        SRZ R3
        ADD R2,R2,R4
        ADD R4,R4,R4
        ADD R0,R0,R0
        ADI R1,R1,-1
        SRZ R1
        JMP rt_multiply_bit
        JMP rt_wrap
; Magnitude division: unsigned 0..32768, quotient R2, remainder R3.
; Signed inputs R3/R2; truncation toward zero, remainder dividend's sign.
rt_divide:
        SNZ R2
        JMP rt_zero_divide
        RtConst R0,0ffff8000h
        SUB R0,R3,R0
        SRZ R0
        JMP rt_divide_signs
        ADI R0,R2,1
        SNZ R0
        JMP rt_overflow
rt_divide_signs:
        RtConst R0,80000000h
        ANL R4,R3,R0
        ANL R5,R2,R0
        XOR R5,R4,R5
        PSH R4
        PSH R5
        SRZ R4
        SUB R3,RZR,R3
        ANL R0,R2,R0
        SRZ R0
        SUB R2,RZR,R2
        ADD R5,R2,RZR
        ADD R2,R3,RZR
        ADI R3,RZR,0
        ADI R4,RZR,0
        RtConst R0,08000h
        ADI R1,RZR,16
rt_divide_bit:
        ADD R3,R3,R3
        ANL R7,R2,R0
        SRZ R7
        ADI R3,R3,1
        ADD R2,R2,R2
        ADD R4,R4,R4
        SUB R7,R3,R5
        RtConst R0,80000000h
        ANL R0,R7,R0
        SRZ R0
        JMP rt_divide_next
        ADD R3,R7,RZR
        ADI R4,R4,1
rt_divide_next:
        RtConst R0,08000h
        ADI R1,R1,-1
        SRZ R1
        JMP rt_divide_bit
        ADD R2,R4,RZR
        POP R5
        POP R4
        SRZ R5
        SUB R2,RZR,R2
        SRZ R4
        SUB R3,RZR,R3
        JMP rt_return
; Signed compare cannot overflow 32 bits because both operands are signed16.
rt_compare:
        SUB R5,R3,R2
        ADI R2,RZR,0
        ADI R0,R4,-61
        SNZ R0
        JMP rt_compare_eq
        ADI R0,R4,-T_NE
        SNZ R0
        JMP rt_compare_ne
        ADI R0,R4,-60
        SNZ R0
        JMP rt_compare_lt
        ADI R0,R4,-T_GE
        SNZ R0
        JMP rt_compare_ge
        ADI R0,R4,-T_LE
        SNZ R0
        JMP rt_compare_le
        ; > is true when right-left is negative.
        SUB R5,RZR,R5
        JMP rt_compare_lt
rt_compare_eq:
        SNZ R5
        ADI R2,RZR,1
        JMP rt_return
rt_compare_ne:
        SRZ R5
        ADI R2,RZR,1
        JMP rt_return
rt_compare_le:
        SNZ R5
        JMP rt_compare_true
rt_compare_lt:
        RtConst R0,80000000h
        ANL R0,R5,R0
        SRZ R0
        ADI R2,RZR,1
        JMP rt_return
rt_compare_ge:
        RtConst R0,80000000h
        ANL R0,R5,R0
        SNZ R0
        ADI R2,RZR,1
        JMP rt_return
rt_compare_true:
        ADI R2,RZR,1
        JMP rt_return
; Decimal output uses repeated subtraction at the five decimal places.
rt_putint:
        PSH R8
        PSH R9
        PSH R6
        ADD R8,R2,RZR
        RtConst R0,80000000h
        ANL R0,R8,R0
        SRZ R0
        JMP rt_putint_negative
        JMP rt_putint_places
rt_putint_negative:
        SUB R8,RZR,R8
        ADI R2,RZR,45
        RtCall rt_putchar
rt_putint_places:
        RtConst R9,80000h+rt_places-rt_base
        ADI R6,RZR,0
rt_putint_place:
        LRM R4,[R9]
        ADI R5,RZR,0
rt_putint_subtract:
        SUB R0,R8,R4
        RtConst R1,80000000h
        ANL R1,R0,R1
        SRZ R1
        JMP rt_putint_digit
        ADD R8,R0,RZR
        ADI R5,R5,1
        JMP rt_putint_subtract
rt_putint_digit:
        SNZ R5
        JMP rt_putint_leading
        ADI R6,RZR,1
rt_putint_leading:
        ADI R0,R4,-1
        SNZ R0
        ADI R6,RZR,1
        SRZ R6
        JMP rt_putint_emit
        JMP rt_putint_next
rt_putint_emit:
        ADI R2,R5,48
        RtCall rt_putchar
rt_putint_next:
        ADI R9,R9,4
        LRM R0,[R9]
        SRZ R0
        JMP rt_putint_place
        POP R6
        POP R9
        POP R8
        JMP rt_return
rt_places: DD 10000,1000,100,10,1,0

; GET keeps the exact previous wrapped accumulator per digit. Backspace thus
; restores the entered prefix even after 16-bit overflow. Limit 255 digits.
; CRLF is consumed once across successive GET calls using one private flag.
rt_getint:
        PSH R8
        PSH R9
        ADI R8,RZR,0
        ADI R9,RZR,0
rt_getint_key:
        RtSys DOS_GETCHAR
        SRZ R0
        JMP rt_io_error
        ADI R0,R2,1
        SNZ R0
        JMP rt_getint_key
        LRM R0,[R13]
        SRM RZR,[R13]
        SRZ R0
        JMP rt_getint_skip_lf
        JMP rt_getint_dispatch
rt_getint_skip_lf:
        ADI R0,R2,-10
        SNZ R0
        JMP rt_getint_key
rt_getint_dispatch:
        ADI R0,R2,-13
        SNZ R0
        JMP rt_getint_cr
        ADI R0,R2,-10
        SNZ R0
        JMP rt_getint_enter
        ADI R0,R2,-8
        SNZ R0
        JMP rt_getint_backspace
        ADI R0,R2,-48
        RtConst R1,80000000h
        ANL R1,R0,R1
        SRZ R1
        JMP rt_getint_key
        ADI R0,R2,-58
        RtConst R1,80000000h
        ANL R1,R0,R1
        SNZ R1
        JMP rt_getint_key
        ADI R0,R9,-255
        SNZ R0
        JMP rt_getint_key
        PSH R8
        PSH R2
        RtCall rt_putchar
        POP R2
        ADD R0,R8,R8
        ADD R8,R0,R0
        ADD R8,R8,R8
        ADD R8,R8,R0
        ADI R2,R2,-48
        ADD R8,R8,R2
        RtConst R0,0ffffh
        ANL R8,R8,R0
        ADI R9,R9,1
        JMP rt_getint_key
rt_getint_backspace:
        SNZ R9
        JMP rt_getint_key
        POP R8
        ADI R9,R9,-1
        ADI R2,RZR,8
        RtCall rt_putchar
        ADI R2,RZR,32
        RtCall rt_putchar
        ADI R2,RZR,8
        RtCall rt_putchar
        JMP rt_getint_key
rt_getint_cr:
        ADI R0,RZR,1
        SRM R0,[R13]
rt_getint_enter:
        SNZ R9
        JMP rt_getint_key
        RtCall rt_newline
        ADD R0,R9,R9
        ADD R0,R0,R0
        ADD SP,SP,R0
        ADD R2,R8,RZR
        POP R9
        POP R8
        JMP rt_wrap
; Guard at every function entry leaves 4 KiB for bounded expression/GET work.
rt_guard:
        RtConst R0,0e1000h
        SUB R0,SP,R0
        RtConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP rt_stack_error
        JMP rt_return
rt_zero_divide:
        RtConst R2,80000h+rt_zero_message-rt_base
        ADI R8,RZR,1
        JMP rt_fatal
rt_overflow:
        RtConst R2,80000h+rt_overflow_message-rt_base
        ADI R8,RZR,2
        JMP rt_fatal
rt_stack_error:
        RtConst R2,80000h+rt_stack_message-rt_base
        ADI R8,RZR,8
        JMP rt_fatal
rt_io_error:
        RtConst R2,80000h+rt_io_message-rt_base
        ADI R8,RZR,5
rt_fatal:
        RtSys DOS_PUTS
        ADD R2,R8,RZR
        RtSys DOS_EXIT
rt_zero_message: DD 68,85,76,76,58,32,100,105,118,105,100,101,32,98,121,32,122,101,114,111,13,10,0
rt_overflow_message: DD 68,85,76,76,58,32,100,105,118,105,100,101,32,111,118,101,114,102,108,111,119,13,10,0
rt_stack_message: DD 68,85,76,76,58,32,115,116,97,99,107,32,108,105,109,105,116,13,10,0
rt_io_message: DD 68,85,76,76,58,32,99,111,110,115,111,108,101,32,101,114,114,111,114,13,10,0
rt_end:
