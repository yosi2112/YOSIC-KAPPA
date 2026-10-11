; YKE payload: CS=8, DS=SS=ES=0. R1 is a word-byte argument tail.
; Write console output, clobber shell registers, then return with status 7.
        CPU YOSIC-KAPPA
        ADI R10,RZR,0feh
        REPT 16
        ADD R10,R10,R10
        ENDM
        IRPC ch,"HELLO YKE"
        ADI R0,RZR,'CH'
        SRM R0,[R10+8]
        ENDM
        ADI R0,RZR,13
        SRM R0,[R10+8]
        ADI R0,RZR,10
        SRM R0,[R10+8]
        ADI R6,RZR,123
        ADI R11,RZR,0
        ADI R12,RZR,0
        ADI R0,RZR,7
        ADI R5,RZR,2
        JMP 0fff8h
        END
