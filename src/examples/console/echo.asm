; Byte-transparent console echo using only MMIO.
; EOF enters a self-loop; the CLI instruction limit ends the example.
    ADI R0,RZR,0xFE
    LSEG DS,R0
    ADI R4,RZR,1
    ADI R5,RZR,4
poll:
    LRM R1,[RZR]
    ANL R2,R1,R4
    SRZ R2
    JMP receive
    ANL R2,R1,R5
    SNZ R2
    JMP poll
done:
    JMP done
receive:
    LRM R3,[RZR+4]
    SRM R3,[RZR+8]
    JMP poll
