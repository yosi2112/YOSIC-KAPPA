; Inputs R0/R1. Preserve console/CR/code bias/echo/frame registers.
core_mul:
        PSH R11
        Const R11,80000000h
        ADD R3,R0,RZR
        ADD R4,R1,RZR
        ADI R5,RZR,0
        ADI R9,RZR,32
core_mul_bit:
        ADD R5,R5,R5
        ANL R2,R4,R11
        SRZ R2
        ADD R5,R5,R3
        ADD R4,R4,R4
        ADI R9,R9,-1
        SRZ R9
        JMP core_mul_bit
        ADD R0,R5,RZR
        POP R11
        JMP return_sub
; Non-negative input values <=7FFFFFFFh, positive divisor.
; Quotient R0, remainder R1. No data-dependent unbounded subtraction loop.
core_div:
        PSH R11
        Const R11,80000000h
        ADD R3,R0,RZR
        ADD R4,R1,RZR
        ADI R5,RZR,0
        ADI R7,RZR,0
        ADI R9,RZR,32
core_div_bit:
        ADD R7,R7,R7
        ANL R2,R3,R11
        SRZ R2
        ADI R7,R7,1
        ADD R3,R3,R3
        ADD R5,R5,R5
        SUB R2,R7,R4
        ANL R1,R2,R11
        SRZ R1
        JMP core_div_next
        ADD R7,R2,RZR
        ADI R5,R5,1
core_div_next:
        ADI R9,R9,-1
        SRZ R9
        JMP core_div_bit
        ADD R0,R5,RZR
        ADD R1,R7,RZR
        POP R11
        JMP return_sub
