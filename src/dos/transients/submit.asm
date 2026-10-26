; SUBMIT producer. Normal public file calls only; reversed 128-byte records.
        CPU YOSIC-KAPPA
        JMP start
        INCLUDE common.inc
SuAddr MACRO reg,label
        TrConst reg,80000h+label
        ENDM
SuSet MACRO off,value
        ADI R0,RZR,value
        SRM R0,[R13+off]
        ENDM
SuCheck MACRO
        SRZ R0
        JMP su_error
        ENDM
start:
        TrInit
        TrConst R13,0df000h
        SuSet 0,0
        SuSet 4,0
        SuSet 28,0
        SuSet 32,1
        SuSet 36,0
        SuSet 40,0
        SuSet 44,0
        SuSet 48,0
        SuSet 68,0
        TrConst R8,0f1000h
        TrConst R9,0d8000h
        ADI R6,RZR,0
su_arg_skip:
        LRM R0,[R8]
        SNZ R0
        JMP su_args_done
        ADI R1,R0,-32
        SNZ R1
        JMP su_arg_space
        ADI R1,R0,-9
        SNZ R1
        JMP su_arg_space
        ADI R0,R6,-10
        SNZ R0
        JMP su_parameter
        ADI R10,RZR,0
su_arg_copy:
        LRM R0,[R8]
        SNZ R0
        JMP su_arg_end
        ADI R1,R0,-32
        SNZ R1
        JMP su_arg_end
        ADI R1,R0,-9
        SNZ R1
        JMP su_arg_end
        ADI R1,R0,-34
        SNZ R1
        JMP su_parameter
        ADI R1,R10,-127
        SNZ R1
        JMP su_parameter
        SRM R0,[R9]
        ADI R9,R9,4
        ADI R8,R8,4
        ADI R10,R10,1
        JMP su_arg_copy
su_arg_end:
        SRM RZR,[R9]
        ADI R6,R6,1
        TrConst R9,0d8000h
        ADD R0,R6,RZR
        REPT 9
        ADD R0,R0,R0
        ENDM
        ADD R9,R9,R0
        JMP su_arg_skip
su_arg_space:
        ADI R8,R8,4
        JMP su_arg_skip
su_args_done:
        SNZ R6
        JMP su_parameter
        ; All missing argument slots are empty strings.
su_arg_empty:
        ADI R0,R6,-10
        SNZ R0
        JMP su_filename
        SRM RZR,[R9]
        ADI R9,R9,512
        ADI R6,R6,1
        JMP su_arg_empty
su_filename:
        TrConst R8,0d8000h
        TrConst R9,0d9800h
        ADD R6,R9,RZR
        ADI R10,RZR,0
        ADI R11,RZR,0
su_filename_copy:
        LRM R0,[R8]
        SNZ R0
        JMP su_extension
        SRM R0,[R9]
        ADI R1,R0,-46
        SNZ R1
        ADI R10,RZR,1
        ADI R1,R0,-92
        SNZ R1
        JMP su_filename_leaf
        ADI R1,R0,-47
        SNZ R1
        JMP su_filename_leaf
        ADI R1,R0,-58
        SNZ R1
        JMP su_filename_leaf
        JMP su_filename_next
su_filename_leaf:
        ADI R6,R9,4
        ADI R10,RZR,0
su_filename_next:
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R11,R11,1
        JMP su_filename_copy
su_extension:
        SRZ R10
        JMP su_filename_ready
        ADI R0,R11,-124
        TrConst R1,80000000h
        ANL R0,R0,R1
        SNZ R0
        JMP su_parameter
        ADI R0,RZR,46
        SRM R0,[R9]
        ADI R9,R9,4
        ADI R0,RZR,83
        SRM R0,[R9]
        ADI R9,R9,4
        ADI R0,RZR,85
        SRM R0,[R9]
        ADI R9,R9,4
        ADI R0,RZR,66
        SRM R0,[R9]
        ADI R9,R9,4
su_filename_ready:
        SRM RZR,[R9]
        ADD R8,R6,RZR
        SuAddr R9,su_reserved_sub
        TrCall su_equal
        SNZ R0
        JMP su_parameter
        ADD R8,R6,RZR
        SuAddr R9,su_reserved_tmp
        TrCall su_equal
        SNZ R0
        JMP su_parameter
        TrConst R2,0d9800h
        ADI R3,RZR,0
        TrCall tr_open
        SuCheck
        SRM R2,[R13]
        TrConst R0,0d4000h
        SRM R0,[R13+20]
        ADI R0,R0,4
        SRM R0,[R13+16]
        SuSet 24,0
su_source:
        TrCall su_getbyte
        SuCheck
        ADI R0,R2,-26
        SNZ R0
        JMP su_eof
        ADI R0,R2,1
        SNZ R0
        JMP su_eof
        SNZ R2
        JMP su_parameter
        LRM R0,[R13+44]
        ADI R1,R0,-1
        SNZ R1
        JMP su_dollar
        ADI R1,R0,-2
        SNZ R1
        JMP su_control
        ADI R0,R2,-13
        SNZ R0
        JMP su_cr
        ADI R0,R2,-10
        SNZ R0
        JMP su_lf
        SuSet 40,0
        ADI R0,R2,-36
        SNZ R0
        JMP su_dollar_start
        ADI R0,R2,-94
        SNZ R0
        JMP su_control_start
        TrCall su_emit
        SuCheck
        JMP su_source
su_dollar_start:
        SuSet 44,1
        JMP su_source
su_control_start:
        SuSet 44,2
        JMP su_source
su_control:
        TrCall su_upper
        ADI R2,R2,-65
        TrConst R1,80000000h
        ANL R0,R2,R1
        SRZ R0
        JMP su_parameter
        ADI R0,R2,-26
        ANL R0,R0,R1
        SNZ R0
        JMP su_parameter
        ADI R2,R2,1
        JMP su_emit_control
su_dollar:
        ADI R0,R2,-36
        SNZ R0
        JMP su_emit_control
        ADI R2,R2,-48
        TrConst R1,80000000h
        ANL R0,R2,R1
        SRZ R0
        JMP su_parameter
        ADI R0,R2,-10
        ANL R0,R0,R1
        SNZ R0
        JMP su_parameter
        REPT 9
        ADD R2,R2,R2
        ENDM
        TrConst R8,0d8000h
        ADD R8,R8,R2
su_substitute:
        LRM R2,[R8]
        SNZ R2
        JMP su_expanded
        TrCall su_emit
        SuCheck
        ADI R8,R8,4
        JMP su_substitute
su_emit_control:
        TrCall su_emit
        SuCheck
su_expanded:
        SuSet 44,0
        JMP su_source
su_cr:
        TrCall su_line
        SuCheck
        SuSet 40,1
        JMP su_new_line
su_lf:
        LRM R0,[R13+40]
        SRZ R0
        JMP su_skip_lf
        TrCall su_line
        SuCheck
su_new_line:
        LRM R0,[R13+32]
        ADI R0,R0,1
        SRM R0,[R13+32]
        JMP su_source
su_skip_lf:
        SuSet 40,0
        JMP su_source
su_eof:
        LRM R0,[R13+44]
        SRZ R0
        JMP su_parameter
        TrCall su_line
        SuCheck
        LRM R2,[R13]
        TrCall tr_close
        SuCheck
        SuSet 0,0
        SuAddr R2,su_tmp
        ADI R3,RZR,1
        TrCall tr_open
        SuCheck
        SRM R2,[R13+4]
        SuSet 68,1
        LRM R6,[R13+36]
su_write_record:
        SNZ R6
        JMP su_promote
        ADI R6,R6,-1
        ADD R0,R6,R6
        ADD R0,R0,R0
        TrConst R8,0d6000h
        ADD R8,R8,R0
        LRM R8,[R8]
        LRM R10,[R8]
        TrConst R9,0da000h
        ADI R0,RZR,128
su_record_clear:
        SRM RZR,[R9]
        ADI R9,R9,4
        ADI R0,R0,-1
        SRZ R0
        JMP su_record_clear
        TrConst R9,0da000h
        SRM R10,[R9]
        ADI R9,R9,4
        ADI R8,R8,4
su_record_copy:
        LRM R0,[R8]
        SRM R0,[R9]
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R10,R10,-1
        SRZ R10
        JMP su_record_copy
        ADI R0,RZR,36
        SRM R0,[R9+4]
        LRM R2,[R13+4]
        TrConst R3,0da000h
        ADI R4,RZR,128
        TrCall tr_write
        SuCheck
        ADI R0,R2,-128
        SNZ R0
        JMP su_write_record
        ADI R0,RZR,112
        JMP su_error
su_promote:
        LRM R2,[R13+4]
        TrCall tr_close
        SuCheck
        SuSet 4,0
        SuAddr R2,su_move
        DosCall DOS_MOVE
        SuCheck
        SuAddr R2,su_done
        TrCall tr_puts
        ADI R2,RZR,0
        TrCall tr_exit
su_parameter:
        ADI R0,RZR,87
su_error:
        PSH R0
        LRM R2,[R13]
        SNZ R2
        JMP su_error_tmp
        TrCall tr_close
su_error_tmp:
        LRM R2,[R13+4]
        SNZ R2
        JMP su_error_delete
        TrCall tr_close
su_error_delete:
        LRM R0,[R13+68]
        SNZ R0
        JMP su_error_message
        SuAddr R2,su_tmp
        DosCall DOS_DELETE
su_error_message:
        SuAddr R2,su_errline
        TrCall tr_puts
        LRM R2,[R13+32]
        TrCall tr_error
        SuAddr R2,su_err
        TrCall tr_puts
        LRM R2,[SP]
        TrCall tr_error
        SuAddr R2,su_nl
        TrCall tr_puts
        POP R2
        TrCall tr_exit
; Byte stream cache, physical EOF is R2=-1.
su_getbyte:
        LRM R0,[R13+48]
        SNZ R0
        JMP su_refill
        LRM R1,[R13+52]
        LRM R2,[R1]
        ADI R1,R1,4
        SRM R1,[R13+52]
        ADI R0,R0,-1
        SRM R0,[R13+48]
        ADI R0,RZR,0
        JMP tr_return
su_refill:
        LRM R2,[R13]
        TrConst R3,0d0000h
        ADI R4,RZR,512
        TrCall tr_read
        SRZ R0
        JMP tr_return
        SNZ R2
        JMP su_end_stream
        SRM R2,[R13+48]
        TrConst R0,0d0000h
        SRM R0,[R13+52]
        JMP su_getbyte
su_end_stream:
        ADI R2,RZR,-1
        JMP tr_return
su_upper:
        ADI R0,R2,-97
        TrConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP tr_return
        ADI R0,R2,-123
        ANL R0,R0,R1
        SRZ R0
        ADI R2,R2,-32
        JMP tr_return
; Compact expansion budget includes one length byte per nonempty command.
su_emit:
        TrCall su_upper
        LRM R0,[R13+24]
        ADI R1,R0,-125
        SNZ R1
        JMP su_emit_bad
        LRM R1,[R13+28]
        ADI R1,R1,1
        SNZ R0
        ADI R1,R1,1
        ADI R3,R1,-2049
        TrConst R5,80000000h
        ANL R3,R3,R5
        SNZ R3
        JMP su_emit_bad
        SRM R1,[R13+28]
        ADI R0,R0,1
        SRM R0,[R13+24]
        LRM R1,[R13+16]
        SRM R2,[R1]
        ADI R1,R1,4
        SRM R1,[R13+16]
        ADI R0,RZR,0
        JMP tr_return
su_emit_bad:
        ADI R0,RZR,87
        JMP tr_return
su_line:
        LRM R0,[R13+24]
        SNZ R0
        JMP tr_return
        LRM R1,[R13+20]
        SRM R0,[R1]
        LRM R0,[R13+36]
        ADD R2,R0,R0
        ADD R2,R2,R2
        TrConst R3,0d6000h
        ADD R2,R2,R3
        SRM R1,[R2]
        ADI R0,R0,1
        SRM R0,[R13+36]
        LRM R0,[R13+16]
        SRM R0,[R13+20]
        ADI R0,R0,4
        SRM R0,[R13+16]
        SRM RZR,[R13+24]
        ADI R0,RZR,0
        JMP tr_return
; Bounded filename equality; upper ASCII only. R8/R9 preserved for caller.
su_equal:
        PSH R8
        PSH R9
su_equal_loop:
        LRM R2,[R8]
        TrCall su_upper
        LRM R1,[R9]
        SUB R0,R2,R1
        SNZ R0
        JMP su_equal_match
        ADI R0,RZR,87
        JMP su_equal_done
su_equal_match:
        SNZ R2
        JMP su_equal_done
        ADI R8,R8,4
        ADI R9,R9,4
        JMP su_equal_loop
su_equal_done:
        POP R9
        POP R8
        JMP tr_return
su_reserved_sub: DD 36,36,36,46,83,85,66,0
su_reserved_tmp: DD 36,36,36,46,84,77,80,0
su_tmp: DD 48,58,92,36,36,36,46,84,77,80,0
su_move: DD 48,58,92,36,36,36,46,84,77,80,32,48,58,92,36,36,36,46,83,85,66,32,47,89,0
su_done: DD 83,85,66,77,73,84,32,114,101,97,100,121,13,10,0
su_errline: DD 83,85,66,77,73,84,32,108,105,110,101,32,0
su_err: DD 32,101,114,114,111,114,32,0
su_nl: DD 13,10,0
        END
