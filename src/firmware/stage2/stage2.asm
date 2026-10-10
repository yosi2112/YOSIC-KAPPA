; Position-independent stage2, entered at RAM 2000h with DS=SS=ES=0.
        CPU YOSIC-KAPPA
        ADI R10,RZR,0feh
        REPT 16
        ADD R10,R10,R10
        ENDM
        IRPC ch,"STAGE2 OK"
        ADI R1,RZR,'CH'
        SRM R1,[R10+8]
        ENDM
        ADI R1,RZR,10
        SRM R1,[R10+8]
        ADI R1,RZR,1357h
        REPT 16
        ADD R1,R1,R1
        ENDM
        ADI R1,R1,2468h
        SRM R1,[RZR+400h]
halt:   JMP halt
        END
