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
Expect  MACRO offset,value
        Const R1,value
        LRM R0,[RZR+1800h+offset]
        SUB R0,R0,R1
        SRZ R0
        JMP bad_header
        ENDM

entry:  Const R10,0fe0000h
        ADI SP,RZR,7800h
        ADD SP,SP,SP
        ADI R4,RZR,0e0h
        ADI R12,RZR,0c0h
        Print "KAPPA IPL"
        ADI R0,RZR,11h
        SRM R0,[R10+10ch]
        ADI R0,RZR,10h
        SRM R0,[R10+10ch]
        LRM R1,[R10+108h]
        ANL R1,R1,R0
        SNZ R1
        JMP no_drive
        ; Let the reset-ready polling cycle complete before draining SIS.
        ADI R0,RZR,512
settle: ADI R0,R0,-1
        SNZ R0
        JMP settled
        JMP settle
settled:
        Send 3
        Send 0f1h
        Send 3
        ADI R7,RZR,4
drain:  Send 8
        Receive R3
        ADI R1,R3,-80h
        SNZ R1
        JMP drained_one
        Receive R3
drained_one:
        ADI R7,R7,-1
        SRZ R7
        JMP drain
        Send 7
        Send 0
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
        ORR R13,R13,R3
        SRZ R13
        JMP fdc_error
        ADI R5,RZR,1
        ADI R6,RZR,1800h
        ADI R7,RZR,1
        ADI R8,RZR,512
        ADI R11,RZR,0

read_sector:
        Send 46h
        Send 0
        Send 0
        Send 0
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
        ; Not-ready path is eight instructions. DATA follows MSR in four.
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
        JMP read_expired
        JMP read_poll
read_expired:
        ; A controller result before 512 bytes is a disk error, not a timeout.
        SUB R2,R1,R12
        ADI R2,R2,-10h
        SRZ R2
        JMP timeout
        JMP fdc_error
transfer_done:
        ADI R0,RZR,12h
        SRM R0,[R10+10ch]
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        SRZ R3
        JMP fdc_error
        Receive R3
        ADI R1,R5,1
        SUB R3,R3,R1
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
        ADI R0,R5,-1
        SRZ R0
        JMP payload_next
        Expect 0,3150424bh
        Expect 4,1
        Expect 12,2000h
        Expect 16,2000h
        Expect 28,0
        LRM R8,[RZR+1808h]
        Const R1,0ffffc003h
        ANL R0,R8,R1
        SRZ R0
        JMP bad_header
        SNZ R8
        JMP bad_header
        Const R3,80000000h
        ADI R0,R8,-8193
        ANL R0,R0,R3
        SNZ R0
        JMP bad_header
        ; Compute ceil(size/512) from the already bounded size.
        ADD R0,R8,RZR
        ADI R1,RZR,0
count_sectors:
        ADI R1,R1,1
        ADI R0,R0,-512
        SNZ R0
        JMP count_done
        ANL R2,R0,R3
        SNZ R2
        JMP count_sectors
count_done:
        LRM R7,[RZR+1818h]
        SUB R1,R1,R7
        SRZ R1
        JMP bad_header
        ADI R5,RZR,2
        ADI R6,RZR,2000h
        ADI R11,RZR,0
        JMP read_sector
payload_next:
        ADI R5,R5,1
        ADI R7,R7,-1
        SRZ R7
        JMP read_sector
        SNZ R8
        JMP checksum
        JMP bad_header
checksum:
        LRM R0,[RZR+1814h]
        SUB R0,R0,R11
        SRZ R0
        JMP bad_checksum
        Print "IPL: BOOT"
        SRM RZR,[R10+10ch]
        LSEG DS,RZR
        LSEG SS,RZR
        LSEG ES,RZR
        JMP handoff
no_drive:
        Print "IPL: NO DRIVE"
        JMP stop
fdc_error:
        Print "IPL: FDC ERROR"
        JMP stop
bad_header:
        Print "IPL: BAD HEADER"
        JMP stop
bad_checksum:
        Print "IPL: CHECKSUM"
        JMP stop
timeout:
        Print "IPL: TIMEOUT"
stop:   SRM RZR,[R10+10ch]
halt:   JMP halt
        ORG 1ffch
handoff:
        LSEG CS,RZR
        END
