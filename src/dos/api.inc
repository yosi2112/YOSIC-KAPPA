; Public YKE syscall helpers. Strings contain one byte per 32-bit word.
DOS_EXIT       EQU 0
DOS_PUTCHAR    EQU 1
DOS_PUTS       EQU 2
DOS_GETCHAR    EQU 3
DOS_GETDRIVE   EQU 4
DOS_SETDRIVE   EQU 5
DOS_DIR        EQU 10h
DOS_MKDIR      EQU 11h
DOS_RMDIR      EQU 12h
DOS_COPY       EQU 13h
DOS_MOVE       EQU 14h
DOS_DELETE     EQU 15h
DOS_SYS        EQU 16h
DOS_OPEN       EQU 20h
DOS_CLOSE      EQU 21h
DOS_READ       EQU 22h
DOS_WRITE      EQU 23h
DosConst MACRO reg,value
        ADI reg,RZR,((((value)+8000h)>>16)&7fffh)-((((value)+8000h)>>16)&8000h)
        REPT 16
        ADD reg,reg,reg
        ENDM
        ADI reg,reg,((value)&7fffh)-((value)&8000h)
        ENDM
; R2 holds the argument; R0 returns status and R2 may return data.
DosCall MACRO function
        ADI R1,RZR,function
        DosConst R7,resume
        JMP 0fff0h
resume:
        ENDM
