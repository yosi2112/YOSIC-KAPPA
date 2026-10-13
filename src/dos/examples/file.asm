; Create DEMO.DAT using handle calls; the disk must permit saving.
        CPU YOSIC-KAPPA
        INCLUDE ../api.inc
        DosConst R2,80000h+path
        ADI R3,RZR,1
        DosCall DOS_OPEN
        SRZ R0
        JMP failed
        ADD R6,R2,RZR
        ADD R2,R6,RZR
        DosConst R3,80000h+bytes
        ADI R4,RZR,5
        DosCall DOS_WRITE
        SRZ R0
        JMP failed
        ADD R2,R6,RZR
        DosCall DOS_CLOSE
        SRZ R0
        JMP failed
        ADI R2,RZR,0
        DosCall DOS_EXIT
failed:
        ADD R2,R0,RZR
        DosCall DOS_EXIT
path:
        DD 68,69,77,79,46,68,65,84,0
bytes:
        DD 0,1,127,128,255
        END
