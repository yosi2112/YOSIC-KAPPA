; Media-command helpers. State DF000h; request DF100h; sector scratch DA000h.
; Only the existing FDC MMIO and public console ABI are used.
McAddr MACRO reg,label
        TrConst reg,80000h+label
        ENDM
McSet MACRO off,value
        ADI R0,RZR,value
        SRM R0,[R13+off]
        ENDM
McFail MACRO
        SRZ R0
        JMP mc_error
        ENDM
; R2 token, R3 expected word string -> R0=0 equal/87 different, ASCII fold.
mc_equal:
        LRM R0,[R2]
        ADI R1,R0,-97
        TrConst R5,80000000h
        ANL R1,R1,R5
        SRZ R1
        JMP mc_equal_folded
        ADI R1,R0,-123
        ANL R1,R1,R5
        SRZ R1
        ADI R0,R0,-32
mc_equal_folded:
        LRM R1,[R3]
        SUB R1,R0,R1
        SRZ R1
        JMP mc_bad
        SNZ R0
        JMP tr_return
        ADI R2,R2,4
        ADI R3,R3,4
        JMP mc_equal
mc_bad: ADI R0,RZR,87
        JMP tr_return
; Parse a drive token (0:..3:). R2 token -> R0 status, R2 drive.
mc_drive:
        LRM R0,[R2+4]
        ADI R0,R0,-58
        LRM R1,[R2+8]
        ORR R0,R0,R1
        SRZ R0
        JMP mc_bad
        LRM R2,[R2]
        ADI R2,R2,-48
        ADI R1,RZR,3
        ANL R1,R2,R1
        SUB R0,R2,R1
        SRZ R0
        JMP mc_bad
        JMP tr_return
mc_arg:
        LRM R2,[R13+24]
        TrConst R3,0dc000h
        ADI R4,RZR,32
        TrCall tr_arg
        SRM R2,[R13+24]
        JMP tr_return
; Configure BD descriptor from state unit+0/profile+8. Disjoint scratch.
mc_descriptor:
        TrConst R8,0df100h
        SRM RZR,[R8]
        LRM R0,[R13]
        SRM R0,[R8+4]
        LRM R0,[R13+8]
        SRM R0,[R8+8]
        TrConst R0,0da000h
        SRM R0,[R8+44]
        ADD R2,R8,RZR
        TrCall bd_open
        JMP tr_return
; Identify current profile, trying both. Returns 23 when neither BPB matches.
mc_detect:
        McSet 8,0
        TrCall mc_descriptor
        ADD R2,R8,RZR
        TrCall bd_identify
        SRZ R0
        JMP mc_detect_second
        JMP tr_return
mc_detect_second:
        McSet 8,1
        TrCall mc_descriptor
        ADD R2,R8,RZR
        TrCall bd_identify
        JMP tr_return
; Operation R0 on current C/H (state +32/+36), sector +40, buffer +44.
mc_request:
        TrConst R8,0df140h
        SRM R0,[R8]
        LRM R0,[R13]
        SRM R0,[R8+4]
        LRM R0,[R13+32]
        SRM R0,[R8+8]
        LRM R0,[R13+36]
        SRM R0,[R8+12]
        LRM R0,[R13+40]
        SRM R0,[R8+16]
        LRM R0,[R13+8]
        SRM R0,[R8+20]
        LRM R0,[R13+44]
        SRM R0,[R8+24]
        ADD R2,R8,RZR
        TrCall fdc_request
        ; A 765 recalibrate steps at most 77 cylinders. From C78/79 the
        ; first request can stop short of track zero; repeat once after idle.
        ADI R1,R0,-23
        SRZ R1
        JMP tr_return
        ADD R2,R8,RZR
        TrCall fdc_request
        JMP tr_return
; Ctrl+C only at safe command boundaries. Other keys are not cancellation.
mc_cancel:
        TrCall tr_getchar
        ADI R0,R2,-3
        SNZ R0
        JMP mc_cancelled
        ADI R0,RZR,0
        JMP tr_return
mc_cancelled:
        ADI R0,RZR,995
        JMP tr_return
mc_confirm:
        LRM R0,[R13+16]
        SNZ R0
        JMP mc_confirm_ask
        ADI R0,RZR,0
        JMP tr_return
mc_confirm_ask:
        McAddr R2,mc_warning
        TrCall tr_puts
mc_confirm_wait:
        TrCall tr_getchar
        ADI R0,R2,1
        SNZ R0
        JMP mc_confirm_wait
        ADI R0,R2,-89
        SRZ R0
        ADI R0,R2,-121
        SRZ R0
        JMP mc_cancelled
        JMP tr_return
mc_error:
        PSH R0
        TrCall fdc_stop
        McAddr R2,mc_errtext
        TrCall tr_puts
        LRM R2,[SP]
        TrCall tr_error
        McAddr R2,mc_crlf
        TrCall tr_puts
        POP R2
        TrCall tr_exit
mc_success:
        TrCall fdc_stop
        McAddr R2,mc_done
        TrCall tr_puts
        ADI R2,RZR,0
        TrCall tr_exit
mc_warning: DD 65,108,108,32,100,97,116,97,32,111,110,32,116,97,114,103,101,116,32,119,105,108,108,32,98,101,32,108,111,115,116,46,32,89,32,116,111,32,99,111,110,116,105,110,117,101,58,32,0
mc_errtext: DD 77,101,100,105,97,32,101,114,114,111,114,32,0
mc_crlf: DD 13,10,0
mc_done: DD 67,111,109,112,108,101,116,101,100,13,10,0
