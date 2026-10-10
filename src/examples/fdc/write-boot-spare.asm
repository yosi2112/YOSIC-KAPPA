; Write 512 bytes of 41h to the unused sector 18 of a KPB1 boot disk.
; Drive 0, MFM, C0 H0 R18 N2. Requires --disk-copy 0 NEWFILE.
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
    send 45h
    send 0
    send 0
    send 0
    send 18
    send 2
    send 18
    send 1bh
    send 0ffh
    ADI R4,RZR,80h
    ADI R5,RZR,512
poll:
    LRM R1,[RZR+FDCMSR]
    ANL R2,R1,R4
    SNZ R2
    JMP poll
    ADI R3,RZR,41h
    SRM R3,[RZR+FDCDATA]
    ADI R5,R5,-1
    SRZ R5
    JMP poll
    ADI R0,RZR,12h
    SRM R0,[RZR+CONTROL]
done:
    JMP done
