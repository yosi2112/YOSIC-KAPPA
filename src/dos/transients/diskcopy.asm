; DISKCOPY: full physical sectors, one-track RAM, read-back comparison.
        CPU YOSIC-KAPPA
        JMP start
        INCLUDE common.inc
        INCLUDE fdc.inc
        INCLUDE block-device.inc
        INCLUDE format-fs.inc
start:
        TrInit
        TrConst R13,0df000h
        McSet 16,0
        McSet 48,0
        TrConst R0,0f1000h
        SRM R0,[R13+24]
        TrCall mc_arg
        McFail
        TrConst R2,0dc000h
        McAddr R3,dc_question
        TrCall mc_equal
        SNZ R0
        JMP dc_help
        TrConst R2,0dc000h
        TrCall mc_drive
        McFail
        SRM R2,[R13+4]
        SRM R2,[R13]
        TrCall mc_arg
        McFail
        TrConst R2,0dc000h
        TrCall mc_drive
        McFail
        SRM R2,[R13+12]
        TrCall mc_arg
        ADI R1,R0,-1
        SNZ R1
        JMP dc_begin
        McFail
        TrConst R2,0dc000h
        McAddr R3,dc_y
        TrCall mc_equal
        McFail
        McSet 16,1
        TrCall mc_arg
        ADI R0,R0,-1
        McFail
dc_begin:
        TrCall dc_detect
        McFail
        LRM R0,[R13+8]
        SRM R0,[R13+20]
        ADI R1,RZR,18
        ADI R2,RZR,512
        ADI R3,RZR,160
        SRZ R0
        ADI R1,RZR,8
        SRZ R0
        ADI R2,RZR,1024
        SRZ R0
        ADI R3,RZR,154
        SRM R1,[R13+52]
        SRM R2,[R13+56]
        SRM R3,[R13+28]
        LRM R0,[R13+4]
        LRM R1,[R13+12]
        SUB R0,R0,R1
        SRM R0,[R13+60]
        TrCall mc_confirm
        McFail
dc_track:
        TrCall mc_cancel
        McFail
        LRM R0,[R13+4]
        SRM R0,[R13]
        LRM R0,[R13+20]
        SRM R0,[R13+8]
        LRM R0,[R13+48]
        ADI R1,RZR,2
        TrCall tr_div
        SRM R0,[R13+32]
        SRM R1,[R13+36]
        McSet 40,1
        TrConst R0,0d0000h
        SRM R0,[R13+44]
dc_read_track:
        ADI R0,RZR,0
        TrCall mc_request
        McFail
        TrCall dc_advance
        SNZ R0
        JMP dc_read_track
        LRM R0,[R13+12]
        SRM R0,[R13]
        LRM R0,[R13+60]
        SRZ R0
        JMP dc_target_check
        McAddr R2,dc_insert_target
        TrCall dc_swap
        McFail
dc_target_check:
        TrCall dc_check_target
        McFail
        LRM R0,[R13+48]
        ADI R1,RZR,2
        TrCall tr_div
        SRM R0,[R13+32]
        SRM R1,[R13+36]
        McSet 40,1
        TrConst R0,0da000h
        SRM R0,[R13+44]
        ADI R0,RZR,3
        TrCall mc_request
        McFail
        TrConst R0,0d0000h
        SRM R0,[R13+44]
dc_write_track:
        ADI R0,RZR,1
        TrCall mc_request
        McFail
        LRM R0,[R13+44]
        PSH R0
        TrConst R0,0db000h
        SRM R0,[R13+44]
        ADI R0,RZR,0
        TrCall mc_request
        McFail
        POP R9
        SRM R9,[R13+44]
        TrConst R8,0db000h
        LRM R6,[R13+56]
dc_compare:
        LRM R0,[R9]
        LRM R1,[R8]
        SUB R0,R0,R1
        SRZ R0
        JMP dc_data_error
        ADI R9,R9,4
        ADI R8,R8,4
        ADI R6,R6,-1
        SRZ R6
        JMP dc_compare
        TrCall dc_advance
        SNZ R0
        JMP dc_write_track
        LRM R0,[R13+48]
        ADI R0,R0,1
        SRM R0,[R13+48]
        LRM R1,[R13+28]
        SUB R0,R0,R1
        SNZ R0
        JMP mc_success
        LRM R0,[R13+60]
        SRZ R0
        JMP dc_track
        LRM R0,[R13+4]
        SRM R0,[R13]
        McAddr R2,dc_insert_source
        TrCall dc_swap
        McFail
        TrCall dc_detect
        McFail
        LRM R0,[R13+8]
        LRM R1,[R13+20]
        SUB R0,R0,R1
        SRZ R0
        JMP dc_data_error
        JMP dc_track
dc_data_error:
        ADI R0,RZR,23
        JMP mc_error
dc_advance:
        LRM R0,[R13+44]
        LRM R1,[R13+56]
        ADD R1,R1,R1
        ADD R1,R1,R1
        ADD R0,R0,R1
        SRM R0,[R13+44]
        LRM R0,[R13+40]
        ADI R0,R0,1
        SRM R0,[R13+40]
        LRM R1,[R13+52]
        ADI R1,R1,1
        SUB R0,R0,R1
        SRZ R0
        JMP dc_more
        ADI R0,RZR,1
        JMP tr_return
dc_more:
        ADI R0,RZR,0
        JMP tr_return
dc_detect:
        TrCall mc_detect
        SRZ R0
        JMP dc_detect_retry
        JMP tr_return
dc_detect_retry:
        ; Retrying is read-only and homes a head left beyond the 765's 77-step limit.
        TrCall mc_detect
        JMP tr_return
dc_check_target:
        TrCall dc_detect
        SRZ R0
        JMP dc_target_no_bpb
        LRM R0,[R13+8]
        LRM R1,[R13+20]
        SUB R0,R0,R1
        SRZ R0
        ADI R0,RZR,23
        JMP tr_return
dc_target_no_bpb:
        ADI R1,R0,-23
        SRZ R1
        JMP tr_return
        LRM R0,[R13+20]
        SRM R0,[R13+8]
        McSet 32,0
        McSet 36,0
        McSet 40,1
        TrConst R0,0da000h
        SRM R0,[R13+44]
        ADI R0,RZR,4
        TrCall mc_request
        SRZ R0
        JMP dc_target_other_id
        JMP tr_return
dc_target_other_id:
        ADI R1,R0,-23
        SRZ R1
        JMP tr_return
        LRM R0,[R13+8]
        ADI R1,RZR,1
        XOR R0,R0,R1
        SRM R0,[R13+8]
        ADI R0,RZR,4
        TrCall mc_request
        PSH R0
        LRM R0,[R13+20]
        SRM R0,[R13+8]
        POP R0
        SNZ R0
        JMP dc_target_bad
        ADI R1,R0,-23
        SNZ R1
        ADI R0,RZR,0
        JMP tr_return
dc_target_bad:
        ADI R0,RZR,23
        JMP tr_return
dc_swap:
        PSH R2
        TrCall fdc_stop
        POP R2
        TrCall tr_puts
        LRM R2,[R13+48]
        TrCall tr_error
        McAddr R2,dc_enter
        TrCall tr_puts
dc_swap_wait:
        TrCall tr_getchar
        ADI R0,R2,-3
        SNZ R0
        JMP mc_cancelled
        ADI R0,R2,-13
        SNZ R0
        JMP tr_return
        ADI R0,R2,-10
        SNZ R0
        JMP tr_return
        JMP dc_swap_wait
dc_help:
        McAddr R2,dc_helptext
        TrCall tr_puts
        ADI R2,RZR,0
        TrCall tr_exit
dc_question: DD 47,63,0
dc_helptext: DD 68,73,83,75,67,79,80,89,32,115,111,117,114,99,101,58,32,116,97,114,103,101,116,58,32,91,47,89,93,13,10,0
dc_y: DD 47,89,0
dc_insert_target: DD 73,110,115,101,114,116,32,84,65,82,71,69,84,44,32,116,114,97,99,107,32,0
dc_insert_source: DD 73,110,115,101,114,116,32,83,79,85,82,67,69,44,32,116,114,97,99,107,32,0
dc_enter: DD 59,32,69,110,116,101,114,32,116,111,32,99,111,110,116,105,110,117,101,32,40,67,116,114,108,43,67,32,99,97,110,99,101,108,115,41,13,10,0
        END
