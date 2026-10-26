; Direct FDC library. INCLUDE after common.inc. No DOS private addresses.
; R2 -> 64-byte request (4-byte aligned) in D0000h..DFFBFh, disjoint from buffer.
; +0 op: 0 read, 1 write, 2 READ ID, 3 FORMAT, 4 probe (READ ID + N check).
; +4 unit 0..3, +8 C, +12 H, +16 R, +20 profile 0=1440 / 1=1232,
; +24 word-byte buffer in D0000h..DFFFFh (512/1024 bytes or 4*SPT IDs).
; +28..52 result ST0/ST1/ST2/C/H/R/N. +56/+60 reserved.
; R0=0/19(protect)/21(not ready or timeout)/23(data)/87(parameter).
; Preserves R6/R8..R13/SP. All exits reset controller and stop motors.
; Cancellation at command boundaries: TrCall fdc_stop. In-flight writes may
; already have changed media; this is an idle recovery primitive, not undo.
FdcSend MACRO reg
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
        JMP fdc_timeout
        JMP poll
done:
        ENDM
FdcByte MACRO value
        ADI R3,RZR,value
        FdcSend R3
        ENDM
FdcReceive MACRO reg
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
        JMP fdc_timeout
        JMP poll
done:
        ENDM
fdc_stop:
        TrConst R0,0fe010ch
        ADI R1,RZR,1
        SRM R1,[R0]
        SRM RZR,[R0]
        ADI R0,RZR,0
        JMP tr_return
fdc_request:
        PSH R8
        PSH R9
        PSH R10
        PSH R11
        PSH R6
        ADD R8,R2,RZR
        TrConst R5,80000000h
        ADI R0,RZR,3
        ANL R0,R8,R0
        SRZ R0
        JMP fdc_parameter
        TrConst R0,0d0000h
        SUB R0,R8,R0
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_parameter
        TrConst R0,0dffc1h
        SUB R0,R8,R0
        ANL R0,R0,R5
        SNZ R0
        JMP fdc_parameter
        LRM R0,[R8]
        ADI R1,R0,-5
        ; Individually reject signed negative and op >= 5.
        LRM R0,[R8]
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_parameter
        ANL R1,R1,R5
        SNZ R1
        JMP fdc_parameter
        LRM R0,[R8+4]
        ADI R1,RZR,3
        ANL R1,R0,R1
        SUB R0,R0,R1
        SRZ R0
        JMP fdc_parameter
        LRM R0,[R8+12]
        ADI R1,RZR,1
        ANL R1,R0,R1
        SUB R0,R0,R1
        SRZ R0
        JMP fdc_parameter
        LRM R0,[R8+20]
        ADI R1,RZR,1
        ANL R1,R0,R1
        SUB R0,R0,R1
        SRZ R0
        JMP fdc_parameter
        ADI R11,RZR,512
        ADI R7,RZR,80
        SRZ R1
        ADI R11,RZR,1024
        SRZ R1
        ADI R7,RZR,77
        LRM R0,[R8+8]
        ANL R1,R0,R5
        SRZ R1
        JMP fdc_parameter
        SUB R0,R0,R7
        ANL R0,R0,R5
        SNZ R0
        JMP fdc_parameter
        LRM R0,[R8+20]
        ADI R7,RZR,18
        SRZ R0
        ADI R7,RZR,8
        LRM R0,[R8+16]
        ADI R1,R0,-1
        ANL R1,R1,R5
        SRZ R1
        JMP fdc_parameter
        SUB R0,R7,R0
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_parameter
        LRM R9,[R8+24]
        ADI R0,RZR,3
        ANL R0,R9,R0
        SRZ R0
        JMP fdc_parameter
        TrConst R0,0d0000h
        SUB R0,R9,R0
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_parameter
        LRM R0,[R8]
        ADI R0,R0,-2
        SNZ R0
        ADI R11,RZR,4
        LRM R0,[R8]
        ADI R0,R0,-4
        SNZ R0
        ADI R11,RZR,4
        LRM R0,[R8]
        ADI R0,R0,-3
        SNZ R0
        ADD R11,R7,R7
        SNZ R0
        ADD R11,R11,R11
        ADD R0,R11,R11
        ADD R0,R0,R0
        ADD R1,R9,R0
        TrConst R0,0e0001h
        SUB R0,R1,R0
        ANL R0,R0,R5
        SNZ R0
        JMP fdc_parameter
        ; Half-open intervals [buffer,end) and [request,request+64).
        SUB R0,R8,R1
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_overlap_second
        JMP fdc_valid
fdc_overlap_second:
        ADI R0,R8,64
        SUB R0,R9,R0
        ANL R0,R0,R5
        SRZ R0
        JMP fdc_parameter
fdc_valid:
        LRM R0,[R8]
        ADI R0,R0,-1
        SRZ R0
        JMP fdc_start_motor
        ADD R2,R9,RZR
        ADD R7,R11,RZR
fdc_validate_bytes:
        LRM R0,[R2]
        ADI R1,RZR,255
        ANL R1,R0,R1
        SUB R0,R0,R1
        SRZ R0
        JMP fdc_parameter
        ADI R2,R2,4
        ADI R7,R7,-1
        SRZ R7
        JMP fdc_validate_bytes
fdc_start_motor:
        ADI R6,RZR,16
        LRM R1,[R8+4]
fdc_motor_bit:
        SNZ R1
        JMP fdc_motor_ready
        ADD R6,R6,R6
        ADI R1,R1,-1
        JMP fdc_motor_bit
fdc_motor_ready:
        TrConst R10,0fe0000h
        ADI R0,R6,1
        SRM R0,[R10+10ch]
        SRM R6,[R10+10ch]
        ; Allow all four 256us ready polls before draining ready-change IRQs.
        ADI R0,RZR,512
fdc_settle:
        ADI R0,R0,-1
        SRZ R0
        JMP fdc_settle
        ADI R4,RZR,0e0h
        ADI R5,RZR,0c0h
        LRM R0,[R10+108h]
        ANL R0,R0,R6
        SNZ R0
        JMP fdc_timeout
        FdcByte 3
        FdcByte 0f1h
        FdcByte 3
        ADI R7,RZR,4
fdc_drain:
        FdcByte 8
        FdcReceive R3
        ADI R0,R3,-80h
        SNZ R0
        JMP fdc_drained
        FdcReceive R3
fdc_drained:
        ADI R7,R7,-1
        SRZ R7
        JMP fdc_drain
        FdcByte 7
        LRM R3,[R8+4]
        FdcSend R3
        SRM RZR,[R8+56]
        TrCall fdc_wait_seek
        FdcByte 0fh
        LRM R3,[R8+12]
        ADD R3,R3,R3
        ADD R3,R3,R3
        LRM R1,[R8+4]
        ADD R3,R3,R1
        FdcSend R3
        LRM R3,[R8+8]
        FdcSend R3
        LRM R3,[R8+8]
        SRM R3,[R8+56]
        TrCall fdc_wait_seek
        ; Read-only media are rejected before a write/format command.
        FdcByte 4
        LRM R3,[R8+4]
        FdcSend R3
        FdcReceive R3
        ADI R0,RZR,40h
        ANL R3,R3,R0
        SRZ R3
        JMP fdc_protected_check
fdc_choose:
        LRM R0,[R8]
        ADI R1,R0,-2
        SNZ R1
        JMP fdc_read_id
        ADI R1,R0,-4
        SNZ R1
        JMP fdc_read_id
        ADI R1,R0,-3
        SNZ R1
        JMP fdc_format
        SNZ R0
        JMP fdc_read_command
        FdcByte 45h
        JMP fdc_data_args
fdc_read_command:
        FdcByte 46h
fdc_data_args:
        LRM R3,[R8+12]
        ADD R3,R3,R3
        ADD R3,R3,R3
        LRM R1,[R8+4]
        ADD R3,R3,R1
        FdcSend R3
        LRM R3,[R8+8]
        FdcSend R3
        LRM R3,[R8+12]
        FdcSend R3
        LRM R3,[R8+16]
        FdcSend R3
        LRM R3,[R8+20]
        ADI R3,R3,2
        FdcSend R3
        ; EOT=the requested physical sector. TC stops after its full data.
        LRM R3,[R8+16]
        FdcSend R3
        FdcByte 1bh
        FdcByte 0ffh
        LRM R7,[R8]
        JMP fdc_transfer_start
fdc_read_id:
        FdcByte 4ah
        LRM R3,[R8+12]
        ADD R3,R3,R3
        ADD R3,R3,R3
        LRM R1,[R8+4]
        ADD R3,R3,R1
        FdcSend R3
        JMP fdc_result
fdc_format:
        LRM R3,[R8+20]
        ADI R7,RZR,18
        SRZ R3
        ADI R7,RZR,8
        ADD R2,R9,RZR
        ADI R1,RZR,1
fdc_format_ids:
        LRM R0,[R8+8]
        SRM R0,[R2]
        LRM R0,[R8+12]
        SRM R0,[R2+4]
        SRM R1,[R2+8]
        ADI R0,R3,2
        SRM R0,[R2+12]
        ADI R2,R2,16
        ADI R1,R1,1
        ADI R7,R7,-1
        SRZ R7
        JMP fdc_format_ids
        FdcByte 4dh
        LRM R3,[R8+12]
        ADD R3,R3,R3
        ADD R3,R3,R3
        LRM R1,[R8+4]
        ADD R3,R3,R1
        FdcSend R3
        LRM R3,[R8+20]
        ADI R3,R3,2
        FdcSend R3
        LRM R3,[R8+20]
        ADI R0,RZR,18
        SRZ R3
        ADI R0,RZR,8
        ADD R3,R0,RZR
        FdcSend R3
        LRM R3,[R8+20]
        ADI R0,RZR,6ch
        SRZ R3
        ADI R0,RZR,35h
        ADD R3,R0,RZR
        FdcSend R3
        FdcByte 0e5h
        ADI R7,RZR,3
fdc_transfer_start:
        ADI R0,RZR,32767
        ADD R0,R0,R0
        ADD R0,R0,R0
        SRZ R7
        JMP fdc_write_transfer
; Maximum wait-to-DATA path stays within the 13us read / 15us write budget.
fdc_read_transfer:
        LRM R1,[R10+100h]
        ANL R2,R1,R4
        SUB R2,R2,R4
        SRZ R2
        JMP fdc_read_wait
        LRM R3,[R10+104h]
        SRM R3,[R9]
        ADI R9,R9,4
        ADI R11,R11,-1
        SNZ R11
        JMP fdc_tc
        JMP fdc_read_transfer
fdc_read_wait:
        ADI R0,R0,-1
        SNZ R0
        JMP fdc_expired
        JMP fdc_read_transfer
fdc_write_transfer:
        LRM R1,[R10+100h]
        ANL R2,R1,R4
        ADI R2,R2,-0a0h
        SRZ R2
        JMP fdc_write_wait
        LRM R3,[R9]
        SRM R3,[R10+104h]
        ADI R9,R9,4
        ADI R11,R11,-1
        SNZ R11
        JMP fdc_written
        JMP fdc_write_transfer
fdc_write_wait:
        ADI R0,R0,-1
        SNZ R0
        JMP fdc_expired
        JMP fdc_write_transfer
fdc_written:
        LRM R7,[R8]
        ADI R7,R7,-3
        SNZ R7
        JMP fdc_result
fdc_tc:
        ADI R0,R6,2
        SRM R0,[R10+10ch]
        JMP fdc_result
fdc_expired:
        ANL R2,R1,R4
        SUB R2,R2,R5
        SNZ R2
        JMP fdc_result
        JMP fdc_timeout
fdc_result:
        ; Receive all seven bytes even on RO, CRC, missing ID or early end.
        FdcReceive R3
        SRM R3,[R8+28]
        FdcReceive R3
        SRM R3,[R8+32]
        FdcReceive R3
        SRM R3,[R8+36]
        FdcReceive R3
        SRM R3,[R8+40]
        FdcReceive R3
        SRM R3,[R8+44]
        FdcReceive R3
        SRM R3,[R8+48]
        FdcReceive R3
        SRM R3,[R8+52]
        LRM R0,[R8+28]
        ADI R1,RZR,0c0h
        ANL R0,R0,R1
        LRM R1,[R8+32]
        ORR R0,R0,R1
        LRM R1,[R8+36]
        ORR R0,R0,R1
        SRZ R0
        JMP fdc_data_error
        LRM R0,[R8+8]
        LRM R1,[R8]
        ADI R2,R1,-2
        TrConst R3,80000000h
        ANL R2,R2,R3
        SRZ R2
        ADI R0,R0,1
        LRM R2,[R8+40]
        SUB R0,R0,R2
        LRM R2,[R8+12]
        LRM R3,[R8+44]
        SUB R2,R2,R3
        ORR R0,R0,R2
        SRZ R0
        JMP fdc_data_error
        LRM R0,[R8+20]
        ADI R0,R0,2
        LRM R2,[R8+52]
        SUB R0,R0,R2
        SRZ R0
        JMP fdc_data_error
        ; READ ID/probe require the requested profile. Data/format also check
        ; the controller's next-sector result.
        ADI R0,R1,-2
        SNZ R0
        JMP fdc_id_copy
        ADI R0,R1,-4
        SNZ R0
        JMP fdc_id_copy
        ADI R2,RZR,1
        ADI R0,R1,-3
        SRZ R0
        JMP fdc_check_r
        LRM R0,[R8+20]
        ADI R2,RZR,19
        SRZ R0
        ADI R2,RZR,9
fdc_check_r:
        LRM R0,[R8+48]
        SUB R0,R0,R2
        SRZ R0
        JMP fdc_data_error
        SNZ R11
        JMP fdc_success
        JMP fdc_data_error
fdc_id_copy:
        LRM R9,[R8+24]
        LRM R0,[R8+40]
        SRM R0,[R9]
        LRM R0,[R8+44]
        SRM R0,[R9+4]
        LRM R0,[R8+48]
        SRM R0,[R9+8]
        LRM R0,[R8+52]
        SRM R0,[R9+12]
fdc_success:
        ADI R0,RZR,0
        JMP fdc_finish
fdc_protected_check:
        LRM R0,[R8]
        ADI R1,R0,-1
        SNZ R1
        JMP fdc_protected
        ADI R1,R0,-3
        SNZ R1
        JMP fdc_protected
        JMP fdc_choose
fdc_data_error:
        LRM R0,[R8+32]
        ADI R1,RZR,2
        ANL R0,R0,R1
        SRZ R0
        JMP fdc_protected
        ADI R0,RZR,23
        JMP fdc_finish
fdc_protected:
        ADI R0,RZR,19
        JMP fdc_finish
fdc_timeout:
        ADI R0,RZR,21
        JMP fdc_finish
fdc_parameter:
        ADI R0,RZR,87
fdc_finish:
        PSH R0
        TrCall fdc_stop
        POP R0
        POP R6
        POP R11
        POP R10
        POP R9
        POP R8
        JMP tr_return
; Nested wait: errors unwind its TrCall frame before the request frame.
fdc_wait_seek:
        ADI R0,RZR,32767
        ADI R7,RZR,1
fdc_wait_irq:
        LRM R1,[R10+108h]
        ANL R1,R1,R7
        SRZ R1
        JMP fdc_seek_sense
        ADI R0,R0,-1
        SNZ R0
        JMP fdc_seek_timeout
        JMP fdc_wait_irq
fdc_seek_sense:
        ; FdcSend/Receive errors also need to discard the nested return word.
        POP R7
        SRM R7,[R8+60]
        FdcByte 8
        FdcReceive R3
        ADI R0,RZR,0d8h
        ANL R7,R3,R0
        FdcReceive R3
        LRM R0,[R8+56]
        SUB R3,R3,R0
        ORR R7,R7,R3
        SRZ R7
        JMP fdc_data_error
        LRM R7,[R8+60]
        PSH R7
        JMP tr_return
fdc_seek_timeout:
        ADI SP,SP,4
        JMP fdc_timeout
