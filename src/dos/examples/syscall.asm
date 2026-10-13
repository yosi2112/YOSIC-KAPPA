; Output and exit entirely through the public R1/JMP interface.
        CPU YOSIC-KAPPA
        INCLUDE ../api.inc
        DosConst R2,80000h+message
        DosCall DOS_PUTS
        ADI R2,RZR,7
        DosCall DOS_EXIT
        ; EXIT never reaches this instruction.
        JMP $
message:
        DD 72,69,76,76,79,32,83,89,83,67,65,76,76,13,10,0
        END
