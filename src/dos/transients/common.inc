; Statically linked YKE runtime. INCLUDE after the application's entry JMP.
; DS/SS/ES=0; SP in E0000h..EFFFFh; R12=88000h (TrInit).
; Calls preserve R6/R8..R13 and balanced SP. R0..R5/R7 are volatile.
; Strings/buffers use one byte per word; caller owns aligned valid RAM.
        INCLUDE ../api.inc
TrInit MACRO
        DosConst R12,88000h
        ENDM
TrConst MACRO reg,value
        LRM reg,[R12+literal-8000h]
        JMP next
literal: DD value
next:
        ENDM
TrCall MACRO target
        LRM R7,[R12+return_word-8000h]
        PSH R7
        JMP target
return_word: DD 18000000h+((returned-tr_ret_slot)/4)
returned:
        ENDM
TrFile MACRO function
        DosCall function
        ENDM
tr_return:
        POP R7
        SRM R7,[R12+tr_ret_slot-8000h]
tr_ret_slot:
        JMP tr_ret_slot
tr_exit:
        DosCall DOS_EXIT
tr_putchar:
        DosCall DOS_PUTCHAR
        JMP tr_return
tr_puts:
        DosCall DOS_PUTS
        JMP tr_return
tr_getchar:
        DosCall DOS_GETCHAR
        JMP tr_return
tr_open:
        DosCall DOS_OPEN
        JMP tr_return
tr_close:
        DosCall DOS_CLOSE
        JMP tr_return
tr_read:
        DosCall DOS_READ
        JMP tr_return
tr_write:
        DosCall DOS_WRITE
        JMP tr_return
; R0=unsigned dividend, R1=positive divisor <= 40000000h.
; R0=quotient, R1=remainder. Fixed 32 iterations.
tr_div:
        PSH R9
        ADD R3,R0,RZR
        ADD R4,R1,RZR
        ADI R5,RZR,0
        ADI R7,RZR,0
        ADI R9,RZR,32
        TrConst R2,80000000h
        PSH R2
tr_div_bit:
        ADD R7,R7,R7
        LRM R2,[SP]
        ANL R2,R3,R2
        SRZ R2
        ADI R7,R7,1
        ADD R3,R3,R3
        ADD R5,R5,R5
        SUB R2,R7,R4
        TrConst R1,80000000h
        ANL R1,R2,R1
        SRZ R1
        JMP tr_div_next
        ADD R7,R2,RZR
        ADI R5,R5,1
tr_div_next:
        ADI R9,R9,-1
        SRZ R9
        JMP tr_div_bit
        ADD R0,R5,RZR
        ADD R1,R7,RZR
        ADI SP,SP,4
        POP R9
        JMP tr_return
; R2=uint32, R3=destination (11 words minimum). R0=0, R2=length.
tr_uint:
        PSH R8
        PSH R9
        PSH R6
        ADD R8,R3,RZR
        ADI R6,RZR,0
        ADD R0,R2,RZR
tr_uint_digit:
        ADI R1,RZR,10
        TrCall tr_div
        ADI R1,R1,48
        PSH R1
        ADI R6,R6,1
        SRZ R0
        JMP tr_uint_digit
        ADD R2,R6,RZR
tr_uint_store:
        POP R1
        SRM R1,[R8]
        ADI R8,R8,4
        ADI R6,R6,-1
        SRZ R6
        JMP tr_uint_store
        SRM RZR,[R8]
        ADI R0,RZR,0
        POP R6
        POP R9
        POP R8
        JMP tr_return
; R2=word string, R4=max digits 1..10. Full NUL-terminated uint32.
; R0=0/87, R2=value on success. Reject overflow, signs and empty input.
tr_parse_uint:
        PSH R8
        ADD R8,R2,RZR
        ADI R0,RZR,0
        ADI R5,RZR,0
        ADI R1,R4,-1
        TrConst R3,80000000h
        ANL R1,R1,R3
        SRZ R1
        JMP tr_parse_bad
        ADI R1,R4,-11
        ANL R1,R1,R3
        SNZ R1
        JMP tr_parse_bad
tr_parse_next:
        LRM R1,[R8]
        SNZ R1
        JMP tr_parse_done
        SNZ R4
        JMP tr_parse_bad
        ADI R1,R1,-48
        ANL R2,R1,R3
        SRZ R2
        JMP tr_parse_bad
        ADI R2,R1,-10
        ANL R2,R2,R3
        SNZ R2
        JMP tr_parse_bad
        ANL R2,R0,R3
        SRZ R2
        JMP tr_parse_bad
        TrConst R2,429496729
        SUB R2,R2,R0
        ANL R5,R2,R3
        SRZ R5
        JMP tr_parse_bad
        SRZ R2
        JMP tr_parse_accumulate
        ADI R2,R1,-6
        ANL R2,R2,R3
        SNZ R2
        JMP tr_parse_bad
tr_parse_accumulate:
        ADD R2,R0,R0
        ADD R0,R2,R2
        ADD R0,R0,R0
        ADD R0,R0,R2
        ADD R0,R0,R1
        ADI R5,RZR,1
        ADI R8,R8,4
        ADI R4,R4,-1
        JMP tr_parse_next
tr_parse_done:
        SNZ R5
        JMP tr_parse_bad
        ADD R2,R0,RZR
        ADI R0,RZR,0
        JMP tr_parse_return
tr_parse_bad:
        ADI R0,RZR,87
tr_parse_return:
        POP R8
        JMP tr_return
; R2=cursor within F1000h..F11FCh, R3=destination, R4=capacity
; including NUL (1..128). R2=next cursor, R3=length; R0=0/1(end)/87.
; Spaces/tabs delimit outside quotes. Quotes are removed; empty quotes valid.
tr_arg:
        PSH R8
        PSH R9
        PSH R6
        ADD R8,R2,RZR
        ADD R9,R3,RZR
        ADI R6,RZR,0
        ADI R5,RZR,0
        ADI R1,RZR,3
        ANL R1,R8,R1
        SRZ R1
        JMP tr_arg_bad
        TrConst R0,0f1000h
        SUB R0,R8,R0
        TrConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP tr_arg_bad
        ADI R0,R4,-1
        ANL R0,R0,R1
        SRZ R0
        JMP tr_arg_bad
        ADI R0,R4,-129
        ANL R0,R0,R1
        SNZ R0
        JMP tr_arg_bad
tr_arg_skip:
        TrCall tr_arg_char
        SRZ R0
        JMP tr_arg_bad
        SNZ R1
        JMP tr_arg_end
        ADI R0,R1,-32
        SNZ R0
        JMP tr_arg_space
        ADI R0,R1,-9
        SNZ R0
        JMP tr_arg_space
        JMP tr_arg_loop
tr_arg_space:
        ADI R8,R8,4
        JMP tr_arg_skip
tr_arg_loop:
        SNZ R1
        JMP tr_arg_done
        ADI R0,R1,-34
        SNZ R0
        JMP tr_arg_quote
        SRZ R5
        JMP tr_arg_copy
        ADI R0,R1,-32
        SNZ R0
        JMP tr_arg_done
        ADI R0,R1,-9
        SNZ R0
        JMP tr_arg_done
tr_arg_copy:
        ADI R0,R4,-1
        SNZ R0
        JMP tr_arg_bad
        SRM R1,[R9]
        ADI R9,R9,4
        ADI R4,R4,-1
        ADI R6,R6,1
        JMP tr_arg_advance
tr_arg_quote:
        ADI R0,RZR,1
        XOR R5,R5,R0
tr_arg_advance:
        ADI R8,R8,4
        TrCall tr_arg_char
        SRZ R0
        JMP tr_arg_bad
        JMP tr_arg_loop
tr_arg_done:
        SRZ R5
        JMP tr_arg_bad
        ADI R0,RZR,0
        JMP tr_arg_finish
tr_arg_end:
        ADI R0,RZR,1
        JMP tr_arg_finish
tr_arg_bad:
        ADI R0,RZR,87
tr_arg_finish:
        SRM RZR,[R9]
        ADD R2,R8,RZR
        ADD R3,R6,RZR
        POP R6
        POP R9
        POP R8
        JMP tr_return
tr_arg_char:
        TrConst R0,0f1200h
        SUB R0,R8,R0
        TrConst R1,80000000h
        ANL R0,R0,R1
        SNZ R0
        JMP tr_arg_char_bad
        LRM R1,[R8]
        ADI R0,RZR,255
        ANL R0,R1,R0
        SUB R0,R1,R0
        SRZ R0
        JMP tr_arg_char_bad
        JMP tr_return
tr_arg_char_bad:
        ADI R0,RZR,87
        JMP tr_return
; R2=destination, R4=capacity incl NUL (1..128). R0=0/87/995
; (Ctrl+C), R2=length. CR or LF terminates. Polling GETCHAR is nonblocking;
; caller must supply a live input stream. Backspace edits, overflow drains.
tr_readline:
        PSH R8
        PSH R9
        PSH R6
        ADD R8,R2,RZR
        ADD R9,R4,RZR
        ADI R6,RZR,0
        ADI R0,R4,-1
        TrConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP tr_line_bad
        ADI R0,R4,-129
        ANL R0,R0,R1
        SNZ R0
        JMP tr_line_bad
        PSH RZR
tr_line_poll:
        DosCall DOS_GETCHAR
        ADI R1,R2,1
        SNZ R1
        JMP tr_line_poll
        ADI R1,R2,-3
        SNZ R1
        JMP tr_line_cancel
        ADI R1,R2,-13
        SNZ R1
        JMP tr_line_done
        ADI R1,R2,-10
        SNZ R1
        JMP tr_line_done
        ADI R1,R2,-8
        SNZ R1
        JMP tr_line_back
        LRM R1,[SP]
        SRZ R1
        JMP tr_line_poll
        SUB R1,R9,R6
        ADI R1,R1,-1
        SNZ R1
        JMP tr_line_overflow
        SRM R2,[R8]
        ADI R8,R8,4
        ADI R6,R6,1
        DosCall DOS_PUTCHAR
        JMP tr_line_poll
tr_line_back:
        LRM R1,[SP]
        ORR R1,R1,R6
        SNZ R1
        JMP tr_line_poll
        LRM R1,[SP]
        SRZ R1
        JMP tr_line_poll
        ADI R8,R8,-4
        ADI R6,R6,-1
        DosCall DOS_PUTCHAR
        ADI R2,RZR,32
        DosCall DOS_PUTCHAR
        ADI R2,RZR,8
        DosCall DOS_PUTCHAR
        JMP tr_line_poll
tr_line_overflow:
        ADI R0,RZR,87
        SRM R0,[SP]
        JMP tr_line_poll
tr_line_cancel:
        ADI R0,RZR,995
        SRM R0,[SP]
tr_line_done:
        POP R0
        SRM RZR,[R8]
        JMP tr_line_return
tr_line_bad:
        ADI R0,RZR,87
tr_line_return:
        ADD R2,R6,RZR
        POP R6
        POP R9
        POP R8
        JMP tr_return
; R2=error number. Numeric diagnostic using only existing console ABI.
tr_error:
        PSH R3
        ADI SP,SP,-44
        ADD R3,SP,RZR
        TrCall tr_uint
        ADD R2,SP,RZR
        TrCall tr_puts
        ADI SP,SP,44
        POP R3
        JMP tr_return
