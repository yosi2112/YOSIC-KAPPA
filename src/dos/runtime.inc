; Native call/return via a writable JMP slot. R12 biases CS=2 by 8000h,
; allowing signed 16-bit loads to reach literals anywhere in the 64KiB kernel.
s16     FUNCTION x,(x & 7fffh)-(x & 8000h)
Const   MACRO reg,value
        ADI reg,RZR,s16((value+8000h)>>16)
        REPT 16
        ADD reg,reg,reg
        ENDM
        ADI reg,reg,s16(value)
        ENDM
Call    MACRO target
        LRM R2,[R12+return_word-8000h]
        PSH R2
        JMP target
return_word:
        DD 18000000h+((returned-ret_slot)/4)
returned:
        ENDM
