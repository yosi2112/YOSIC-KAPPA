; AS-style parameterized macro driving the MMIO console.
        CPU YOSIC-KAPPA
Console EQU 0FEh
PutChar MACRO ch,reg=R1
        ADI reg,RZR,ch
        SRM reg,[RZR+8]
        ENDM
        ADI R0,RZR,Console
        LSEG DS,R0
        PutChar 'A'
        PutChar 'S',reg=R2
stop:   JMP stop
        END
