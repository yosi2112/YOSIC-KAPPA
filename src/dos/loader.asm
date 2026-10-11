; KAPPA boot contract 1. All disk bytes arrive through uPD765A MMIO.
        CPU YOSIC-KAPPA
s16     FUNCTION x,(x & 7fffh)-(x & 8000h)
Const   MACRO reg,value
        ADI reg,RZR,s16((value+8000h)>>16)
        REPT 16
        ADD reg,reg,reg
        ENDM
        ADI reg,reg,s16(value)
        ENDM
Print   MACRO text
        IRPC ch,text
        ADI R3,RZR,'CH'
        SRM R3,[R10+8]
        ENDM
        ADI R3,RZR,10
        SRM R3,[R10+8]
        ENDM
SendReg MACRO reg
        ADI R0,RZR,32767
.poll:  LRM R1,[R10+100h]
        ANL R2,R1,R12
        ADI R2,R2,-80h
        SRZ R2
        JMP .wait
        SRM reg,[R10+104h]
        JMP .done
.wait:  ADI R0,R0,-1
        SNZ R0
        JMP timeout
        JMP .poll
.done:
        ENDM
Send    MACRO value
        ADI R3,RZR,value
        SendReg R3
        ENDM
Receive MACRO reg
        ADI R0,RZR,32767
.poll:  LRM R1,[R10+100h]
        ANL R2,R1,R4
        SUB R2,R2,R12
        SRZ R2
        JMP .wait
        LRM reg,[R10+104h]
        JMP .done
.wait:  ADI R0,R0,-1
        SNZ R0
        JMP timeout
        JMP .poll
.done:
        ENDM
        INCLUDE kernel.inc
Cylinder EQU 1900h
Head     EQU 1904h
        ORG 2000h
entry:  Const R10,0fe0000h
        ADI R4,RZR,0e0h
        ADI R12,RZR,0c0h
        SRM RZR,[RZR+Cylinder]
        ADI R0,RZR,1
        SRM R0,[RZR+Head]
        ADI R0,RZR,10h
        SRM R0,[R10+10ch]
        ; IPL leaves the FDC specified and physically at cylinder zero.
        ADI R0,RZR,512
settle: ADI R0,R0,-1
        SNZ R0
        JMP settled
        JMP settle
settled:
        ADI R7,RZR,4
 drain: Send 8
        Receive R3
        ADI R1,R3,-80h
        SNZ R1
        JMP drained_one
        Receive R3
 drained_one:
        ADI R7,R7,-1
        SRZ R7
        JMP drain
        ADI R5,RZR,1
        Const R6,20000h
        ADI R7,RZR,KernelSectors
        Const R8,KernelBytes
        ADI R11,RZR,0
read_sector:
        Send 46h
        LRM R3,[RZR+Head]
        ADD R3,R3,R3
        ADD R3,R3,R3
        SendReg R3
        LRM R3,[RZR+Cylinder]
        SendReg R3
        LRM R3,[RZR+Head]
        SendReg R3
        SendReg R5
        Send 2
        Send 18
        Send 1bh
        Send 0ffh
        Const R9,10000h
        ADI R13,RZR,512
        ADI R0,RZR,32767
        ADD R0,R0,R0
        ADD R0,R0,R0
read_poll:
        LRM R1,[R10+100h]
        ANL R2,R1,R4
        SUB R2,R2,R4
        SRZ R2
        JMP read_wait
        LRM R3,[R10+104h]
        SRM R3,[R9]
        ADI R9,R9,4
        ADI R13,R13,-1
        SNZ R13
        JMP transfer_done
        JMP read_poll
read_wait:
        ADI R0,R0,-1
        SNZ R0
        JMP timeout
        JMP read_poll
transfer_done:
        ADI R0,RZR,12h
        SRM R0,[R10+10ch]
        Receive R3
        LRM R13,[RZR+Head]
        ADD R13,R13,R13
        ADD R13,R13,R13
        SUB R3,R3,R13
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        LRM R13,[RZR+Cylinder]
        ADI R2,R5,-18
        SRZ R2
        JMP result_cylinder
        ADI R13,R13,1
result_cylinder:
        Receive R3
        SUB R3,R3,R13
        SRZ R3
        JMP fdc_error
        LRM R13,[RZR+Head]
        Receive R3
        SUB R3,R3,R13
        SRZ R3
        JMP fdc_error
        ADI R13,R5,1
        ADI R2,R13,-19
        SRZ R2
        JMP result_sector
        ADI R13,RZR,1
result_sector:
        Receive R3
        SUB R3,R3,R13
        SRZ R3
        JMP fdc_error
        Receive R3
        ADI R3,R3,-2
        SRZ R3
        JMP fdc_error
        Const R9,10000h
        ADI R13,RZR,128
unpack:
        SNZ R8
        JMP copied
        LRM R0,[R9+12]
        REPT 8
        ADD R0,R0,R0
        ENDM
        LRM R1,[R9+8]
        ORR R0,R0,R1
        REPT 8
        ADD R0,R0,R0
        ENDM
        LRM R1,[R9+4]
        ORR R0,R0,R1
        REPT 8
        ADD R0,R0,R0
        ENDM
        LRM R1,[R9]
        ORR R0,R0,R1
        SRM R0,[R6]
        ADD R11,R11,R0
        ADI R6,R6,4
        ADI R9,R9,16
        ADI R8,R8,-4
        ADI R13,R13,-1
        SRZ R13
        JMP unpack
copied:
        ADI R7,R7,-1
        SNZ R7
        JMP checksum
        ADI R5,R5,1
        ADI R0,R5,-19
        SNZ R0
        JMP next_track
        JMP read_sector
next_track:
        ADI R5,RZR,1
        LRM R0,[RZR+Head]
        ADI R1,RZR,1
        XOR R0,R0,R1
        SRM R0,[RZR+Head]
        SRZ R0
        JMP read_sector
        LRM R0,[RZR+Cylinder]
        ADI R0,R0,1
        SRM R0,[RZR+Cylinder]
        Send 0fh
        Send 0
        LRM R3,[RZR+Cylinder]
        SendReg R3
        ADI R0,RZR,32767
        ADI R3,RZR,1
seek_wait:
        LRM R1,[R10+108h]
        ANL R1,R1,R3
        SRZ R1
        JMP seek_done
        ADI R0,R0,-1
        SNZ R0
        JMP timeout
        JMP seek_wait
seek_done:
        Send 8
        Receive R3
        ADI R13,R3,-20h
        Receive R3
        LRM R1,[RZR+Cylinder]
        SUB R3,R3,R1
        ORR R13,R13,R3
        SRZ R13
        JMP fdc_error
        JMP read_sector
checksum:
        Const R0,KernelChecksum
        SUB R0,R0,R11
        SRZ R0
        JMP bad_checksum
        SRM RZR,[R10+10ch]
        ; CS changes at FFFCh, so the next fetch is CS=2, PC=0.
        Const R1,80000000h
        Const R9,0fffch
        SRM R1,[R9]
        ADI R0,RZR,2
        JMP 0fffch
fdc_error:
        Print "DOS: FDC ERROR"
        JMP stop
bad_checksum:
        Print "DOS: CHECKSUM"
        JMP stop
timeout:
        Print "DOS: TIMEOUT"
stop:   SRM RZR,[R10+10ch]
halt:   JMP halt
        END
