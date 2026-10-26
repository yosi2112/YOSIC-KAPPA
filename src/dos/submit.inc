; Internal SUBMIT consumer. No public API, application flags or private EXIT.
; Scratch 18000h..1820Fh (control + retained record), file helpers at 4F00h.
; The last record is persisted as consumed before it is dispatched.
SubFail MACRO
        SRZ R0
        JMP submit_fault
        ENDM
submit_next:
        PSH R6
        ExecConst R1,18000h
        SRM RZR,[R1]
        LRM R0,[R1+4]
        SRZ R0
        JMP submit_none
        Call submit_locate
        ADI R1,R0,-2
        SNZ R1
        JMP submit_none
        SubFail
        LRM R0,[R13+20]
        SNZ R0
        JMP submit_empty
submit_key:
        LRM R0,[R10]
        ADI R1,RZR,1
        ANL R0,R0,R1
        SNZ R0
        JMP submit_read_tail
        LRM R1,[R10+4]
        ADI R0,R1,-10
        SRZ R0
        JMP submit_key_abort
        SNZ R6
        JMP submit_key_abort
        ADI R6,RZR,0
        JMP submit_key
submit_key_abort:
        ADI R0,RZR,995
        JMP submit_fault
submit_empty:
        Call submit_unlink
        SubFail
        JMP submit_none
submit_read_tail:
        LRM R0,[R13+20]
        ADI R0,R0,-128
        SRM R0,[R13+84]
        ADI R1,RZR,512
        Call core_div
        SRM R1,[R13+60]
        LRM R1,[R13+32]
        LRM R1,[R1+36]
        Call core_div
        SRM R1,[R13+76]
        ADD R0,R0,R0
        ADD R0,R0,R0
        LRM R1,[R13+124]
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R13+64]
        FileSmall 0
        Call file_sector
        ADI SP,SP,4
        SubFail
        LRM R0,[R13+60]
        ADD R0,R0,R0
        ADD R0,R0,R0
        LRM R8,[R13+120]
        ADD R8,R8,R0
        ExecConst R9,18010h
        ADI R3,RZR,128
submit_retain:
        LRM R0,[R8]
        SRM R0,[R9]
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R3,R3,-1
        SRZ R3
        JMP submit_retain
        ExecConst R8,18010h
        LRM R6,[R8]
        ADI R0,R6,-1
        ExecConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP submit_invalid
        ADI R0,R6,-126
        ANL R0,R0,R1
        SNZ R0
        JMP submit_invalid
        ADI R8,R8,4
        ADD R3,R6,RZR
        ADI R9,RZR,Buffer
submit_text:
        LRM R0,[R8]
        SNZ R0
        JMP submit_invalid
        ADI R1,RZR,255
        ANL R1,R0,R1
        SUB R1,R0,R1
        SRZ R1
        JMP submit_invalid
        SRM R0,[R9]
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R3,R3,-1
        SRZ R3
        JMP submit_text
        SRM RZR,[R9]
        LRM R0,[R8]
        SRZ R0
        JMP submit_invalid
        LRM R0,[R8+4]
        ADI R0,R0,-36
        SRZ R0
        JMP submit_invalid
        ADI R8,R8,8
        ADI R3,RZR,125
        SUB R3,R3,R6
submit_padding:
        SNZ R3
        JMP submit_consume
        LRM R0,[R8]
        SRZ R0
        JMP submit_invalid
        ADI R8,R8,4
        ADI R3,R3,-1
        JMP submit_padding
submit_consume:
        Call submit_truncate
        SubFail
        ExecConst R1,18000h
        ADI R0,RZR,1
        SRM R0,[R1]
        ADI R0,RZR,Buffer
        PSH R0
        Call core_puts
        ADI SP,SP,4
        NewLine
        ADI R0,RZR,1
        POP R6
        JMP return_sub
submit_invalid:
        ADI R0,RZR,13
submit_fault:
        Call submit_stop
submit_none:
        ADI R0,RZR,0
        POP R6
        JMP return_sub
; Resolve only 0:\$$$.SUB. All helpers use the selected FS state, never CWD.
submit_locate:
        ExecConst R13,4f00h
        ExecConst R0,40000h
        SRM R0,[R13+116]
        ExecConst R0,46000h
        SRM R0,[R13+120]
        ExecConst R0,64000h
        SRM R0,[R13+124]
        ExecConst R0,13fe4h
        SRM RZR,[R0]
        ExecArg 20000h+submit_path
        FileSmall 0
        Call core_resolve
        ADI SP,SP,8
        ADI R1,R0,-3
        SNZ R1
        ADI R0,RZR,2
        SRZ R0
        JMP return_sub
        ExecConst R0,13fc4h
        LRM R0,[R0]
        SNZ R0
        JMP submit_missing
        SRM R0,[R13+36]
        ExecConst R1,40000h
        SUB R1,R0,R1
        SRM R1,[R13+16]
        LRM R1,[R0+44]
        ADI R2,RZR,31
        ANL R1,R1,R2
        SRZ R1
        JMP submit_denied
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R13+28]
        LRM R0,[R13+36]
        ADI R0,R0,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        SRM R0,[R13+20]
        ADI R1,RZR,127
        ANL R1,R0,R1
        SRZ R1
        JMP submit_bad_file
        ExecConst R1,131073
        SUB R0,R0,R1
        ExecConst R1,80000000h
        ANL R0,R0,R1
        SNZ R0
        JMP submit_bad_file
        ExecConst R0,13fe0h
        LRM R0,[R0]
        SRM R0,[R13+32]
        SRM RZR,[R13+8]
        ExecConst R0,13ff8h
        LRM R0,[R0]
        SRM R0,[R13+12]
        Call file_chain
        JMP return_sub
submit_missing:
        ADI R0,RZR,2
        JMP return_sub
submit_denied:
        ADI R0,RZR,5
        JMP return_sub
submit_bad_file:
        ADI R0,RZR,13
        JMP return_sub
submit_unlink:
        FileField 36
        Call core_unlink_file
        ADI SP,SP,4
        JMP return_sub
submit_truncate:
        LRM R0,[R13+84]
        SNZ R0
        JMP submit_unlink
        SRM R0,[R13+20]
        PSH R0
        LRM R0,[R13+32]
        LRM R0,[R0+36]
        ADI R1,RZR,512
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R0,R0,-1
        Call core_div
        SRM R0,[R13+88]
        Call file_commit
        SRZ R0
        JMP return_sub
        LRM R0,[R13+88]
        LRM R1,[R13+40]
        SUB R1,R1,R0
        SNZ R1
        JMP submit_truncated
        SRM R1,[R13+92]
        ADD R0,R0,R0
        ADD R0,R0,R0
        LRM R1,[R13+124]
        ADD R0,R0,R1
        SRM R0,[R13+96]
        LRM R0,[R0-4]
        PSH R0
        FileSmall 4095
        Call core_fatset
        ADI SP,SP,8
        FileField 96
        FileField 92
        Call core_free_chain
        ADI SP,SP,8
        Call core_fatflush
        JMP return_sub
submit_truncated:
        ADI R0,RZR,0
        JMP return_sub
; Called after either builtin dispatch or YKE return with its real status.
submit_result:
        ExecConst R1,18000h
        LRM R2,[R1]
        SRM RZR,[R1]
        SNZ R2
        JMP return_sub
        SNZ R0
        JMP return_sub
submit_stop:
        PSH R0
        Call submit_locate
        ADI R1,R0,-2
        SNZ R1
        JMP submit_stop_report
        SRZ R0
        JMP submit_discard_failed
        Call submit_unlink
        SRZ R0
        JMP submit_discard_failed
        JMP submit_stop_report
submit_discard_failed:
        ExecConst R1,18004h
        ADI R2,RZR,1
        SRM R2,[R1]
        PSH R0
        PutText "SUBMIT discard failed"
        NewLine
        Call core_error_message
        ADI SP,SP,4
submit_stop_report:
        PutText "SUBMIT stopped"
        NewLine
        Call core_error_message
        POP R0
        JMP return_sub
submit_reset:
        ExecConst R1,18000h
        SRM RZR,[R1]
        SRM RZR,[R1+4]
        Call submit_locate
        ADI R1,R0,-2
        SNZ R1
        JMP submit_truncated
        ADI R0,RZR,995
        JMP submit_stop
submit_path: DD 48,58,92,36,36,36,46,83,85,66,0
