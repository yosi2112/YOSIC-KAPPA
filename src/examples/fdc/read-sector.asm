; Read 128 bytes from drive 0 into the console using non-DMA MMIO.
; Host: --disk 0 FILE --steps 500000
FDCDATA EQU 104h
FDCMSR EQU 100h
CONTROL EQU 10ch
send MACRO value
    ADI R1,RZR,value
    SRM R1,[RZR+FDCDATA]
    ENDM
    ADI R0,RZR,0feh
    LSEG DS,R0
    ADI R0,RZR,10h
    SRM R0,[RZR+CONTROL]
    send 3
    send 0f1h
    send 3
    send 46h
    send 0
    send 0
    send 0
    send 1
    send 0
    send 1
    send 1bh
    send 128
    ADI R4,RZR,80h
    ADI R5,RZR,128
poll:
    LRM R1,[RZR+FDCMSR]
    ANL R2,R1,R4
    SNZ R2
    JMP poll
    LRM R3,[RZR+FDCDATA]
    SRM R3,[RZR+8]
    ADI R5,R5,-1
    SRZ R5
    JMP poll
    ADI R0,RZR,12h
    SRM R0,[RZR+CONTROL]
done:
    JMP done
