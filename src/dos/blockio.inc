; block(lba, word-byte-buffer, drive0..3, writeFlag) -> DOS error or zero.
; DATA transfer loop is handwritten to meet the FDC's 13/15us deadlines.
IOSendReg MACRO reg
        ADI R0,RZR,32767
poll:   LRM R1,[R10+100h]
        ANL R2,R1,R5
        ADI R2,R2,-80h
        SRZ R2
        JMP wait
        SRM reg,[R10+104h]
        JMP done
wait:   ADI R0,R0,-1
        SNZ R0
        JMP io_timeout
        JMP poll
done:
        ENDM
IOSend MACRO value
        ADI R3,RZR,value
        IOSendReg R3
        ENDM
IOReceive MACRO reg
        ADI R0,RZR,32767
poll:   LRM R1,[R10+100h]
        ANL R2,R1,R4
        SUB R2,R2,R5
        SRZ R2
        JMP wait
        LRM reg,[R10+104h]
        JMP done
wait:   ADI R0,R0,-1
        SNZ R0
        JMP io_timeout
        JMP poll
done:
        ENDM
core_block:
        PSH R8
        ADD R8,SP,RZR
        PSH R11
        LRM R0,[R8+8]
        SNZ R0
        JMP io_begin
        LRM R0,[R8+12]
        REPT 9
        ADD R0,R0,R0
        ENDM
        Const R1,50000h
        ADD R0,R0,R1
        LRM R0,[R0+40]
        SNZ R0
        JMP io_begin
        ADI R0,RZR,19
        POP R11
        POP R8
        JMP return_sub
io_begin:
        LRM R0,[R8+20]
        SRM R0,[RZR+4b00h]
        LRM R0,[R8+16]
        SRM R0,[RZR+4b04h]
        LRM R0,[R8+12]
        SRM R0,[RZR+4b08h]
        LRM R0,[R8+8]
        SRM R0,[RZR+4b0ch]
        SRM RZR,[RZR+4b24h]
        LRM R0,[RZR+4b08h]
        REPT 9
        ADD R0,R0,R0
        ENDM
        Const R1,50000h
        ADD R0,R0,R1
        LRM R3,[R0+48]
        SNZ R3
        JMP io_default_spt
        SRM R3,[RZR+4b2ch]
        JMP io_spt_ready
io_default_spt:
        ADI R3,RZR,18
        SRM R3,[RZR+4b2ch]
io_spt_ready:
        LRM R0,[RZR+4b00h]
        LRM R1,[RZR+4b2ch]
        ADD R1,R1,R1
        Call core_div
        SRM R0,[RZR+4b10h]
        ADD R0,R1,RZR
        LRM R1,[RZR+4b2ch]
        Call core_div
        SRM R0,[RZR+4b14h]
        ADI R1,R1,1
        SRM R1,[RZR+4b18h]
        ADD R0,R0,R0
        ADD R0,R0,R0
        LRM R1,[RZR+4b08h]
        ADD R0,R0,R1
        SRM R0,[RZR+4b1ch]
        ADI R0,RZR,16
io_motor:
        SNZ R1
        JMP io_motor_ready
        ADD R0,R0,R0
        ADI R1,R1,-1
        JMP io_motor
io_motor_ready:
        SRM R0,[RZR+4b20h]
        ADI R1,R0,1
        SRM R1,[R10+10ch]
        SRM R0,[R10+10ch]
        ADI R4,RZR,0e0h
        ADI R5,RZR,0c0h
        ADI R0,RZR,512
io_settle:
        ADI R0,R0,-1
        SRZ R0
        JMP io_settle
        LRM R0,[RZR+4b20h]
        LRM R1,[R10+108h]
        ANL R1,R1,R0
        SNZ R1
        JMP io_not_ready
        IOSend 3
        IOSend 0f1h
        IOSend 3
        ADI R7,RZR,4
io_drain:
        IOSend 8
        IOReceive R3
        ADI R1,R3,-80h
        SNZ R1
        JMP io_drained_one
        IOReceive R3
io_drained_one:
        ADI R7,R7,-1
        SRZ R7
        JMP io_drain
        ; RESET clears controller PCN but leaves the mechanical cylinder.
        ; Establish physical track zero before every absolute seek.
        IOSend 7
        LRM R3,[RZR+4b08h]
        IOSendReg R3
        ADI R0,RZR,32767
        ADI R7,RZR,1
io_recal_wait:
        LRM R1,[R10+108h]
        ANL R1,R1,R7
        SRZ R1
        JMP io_recal_done
        ADI R0,R0,-1
        SNZ R0
        JMP io_timeout
        JMP io_recal_wait
io_recal_done:
        IOSend 8
        IOReceive R3
        LRM R1,[RZR+4b08h]
        ADI R1,R1,20h
        SUB R7,R3,R1
        IOReceive R3
        ORR R7,R7,R3
        SRZ R7
        JMP io_bad_result
        IOSend 0fh
        LRM R3,[RZR+4b1ch]
        IOSendReg R3
        LRM R3,[RZR+4b10h]
        IOSendReg R3
        ADI R0,RZR,32767
        ADI R7,RZR,1
io_seek_wait:
        LRM R1,[R10+108h]
        ANL R1,R1,R7
        SRZ R1
        JMP io_seek_done
        ADI R0,R0,-1
        SNZ R0
        JMP io_timeout
        JMP io_seek_wait
io_seek_done:
        IOSend 8
        IOReceive R3
        LRM R1,[RZR+4b1ch]
        ADI R1,R1,20h
        SUB R7,R3,R1
        IOReceive R3
        LRM R1,[RZR+4b10h]
        SUB R3,R3,R1
        ORR R7,R7,R3
        SRZ R7
        JMP io_bad_result
        LRM R7,[RZR+4b0ch]
        SRZ R7
        JMP io_write_command
        IOSend 46h
        JMP io_command_args
io_write_command:
        IOSend 45h
io_command_args:
        LRM R3,[RZR+4b1ch]
        IOSendReg R3
        LRM R3,[RZR+4b10h]
        IOSendReg R3
        LRM R3,[RZR+4b14h]
        IOSendReg R3
        LRM R3,[RZR+4b18h]
        IOSendReg R3
        IOSend 2
        LRM R3,[RZR+4b2ch]
        IOSendReg R3
        IOSend 1bh
        IOSend 0ffh
        LRM R9,[RZR+4b04h]
        ADI R11,RZR,512
        ADI R0,RZR,32767
        ADD R0,R0,R0
        ADD R0,R0,R0
        SRZ R7
        JMP io_write_transfer
io_transfer:
        LRM R1,[R10+100h]
        ANL R2,R1,R4
        SUB R2,R2,R4
        SRZ R2
        JMP io_transfer_wait
        LRM R3,[R10+104h]
        SRM R3,[R9]
        ADI R9,R9,4
        ADI R11,R11,-1
        SNZ R11
        JMP io_tc
        JMP io_transfer
io_transfer_wait:
        ADI R0,R0,-1
        SNZ R0
        JMP io_transfer_expired
        JMP io_transfer
io_write_transfer:
        LRM R1,[R10+100h]
        ANL R2,R1,R4
        ADI R2,R2,-0a0h
        SRZ R2
        JMP io_write_wait
        LRM R3,[R9]
        SRM R3,[R10+104h]
        ADI R9,R9,4
        ADI R11,R11,-1
        SNZ R11
        JMP io_tc
        JMP io_write_transfer
io_write_wait:
        ADI R0,R0,-1
        SNZ R0
        JMP io_transfer_expired
        JMP io_write_transfer
io_transfer_expired:
        ANL R2,R1,R4
        SUB R2,R2,R5
        SNZ R2
        JMP io_result
        JMP io_timeout
io_tc:
        LRM R0,[RZR+4b20h]
        ADI R0,R0,2
        SRM R0,[R10+10ch]
io_result:
        ADD R7,R11,RZR
        IOReceive R3
        LRM R1,[RZR+4b1ch]
        SUB R3,R3,R1
        ORR R7,R7,R3
        IOReceive R3
        SRM R3,[RZR+4b28h]
        ORR R7,R7,R3
        IOReceive R3
        ORR R7,R7,R3
        LRM R11,[RZR+4b10h]
        LRM R3,[RZR+4b18h]
        LRM R1,[RZR+4b2ch]
        SUB R1,R3,R1
        SRZ R1
        JMP io_result_c
        ADI R11,R11,1
io_result_c:
        IOReceive R3
        SUB R3,R3,R11
        ORR R7,R7,R3
        IOReceive R3
        LRM R1,[RZR+4b14h]
        SUB R3,R3,R1
        ORR R7,R7,R3
        LRM R11,[RZR+4b18h]
        ADI R11,R11,1
        LRM R1,[RZR+4b2ch]
        ADI R1,R1,1
        SUB R1,R11,R1
        SRZ R1
        JMP io_result_r
        ADI R11,RZR,1
io_result_r:
        IOReceive R3
        SUB R3,R3,R11
        ORR R7,R7,R3
        IOReceive R3
        ADI R3,R3,-2
        ORR R7,R7,R3
        SRZ R7
        JMP io_bad_result
        JMP io_finish
io_bad_result:
        ADI R0,RZR,23
        LRM R1,[RZR+4b28h]
        ADI R2,RZR,2
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,19
        JMP io_error
io_not_ready:
        ADI R0,RZR,21
        JMP io_error
io_timeout:
        ADI R0,RZR,21
io_error:
        SRM R0,[RZR+4b24h]
io_finish:
        SRM RZR,[R10+10ch]
        LRM R0,[RZR+4b24h]
        SRZ R0
        JMP io_return
        LRM R0,[R8+8]
        SNZ R0
        JMP io_return
        LRM R7,[R8+12]
        REPT 9
        ADD R7,R7,R7
        ENDM
        Const R0,50000h
        ADD R7,R7,R0
        LRM R0,[R8+20]
        LRM R1,[R8+16]
        Call core_media_written
io_return:
        LRM R0,[RZR+4b24h]
        POP R11
        POP R8
        JMP return_sub
