        ; Generated native code. Source model: src/dos/core.py; canonical assembly, no generator required
core_putc:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R12+core_literal_2-8000h]
        LRM R1,[R8+8]
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_1
        ADI R0,RZR,0
core_return_1:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_puts:
        PSH R8
        ADD R8,SP,RZR
core_while_4:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_5
        LRM R0,[R8+8]
        LRM R0,[R0]
        PSH R0
        Call core_putc
        ADI SP,SP,4
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_4
core_while_end_5:
        ADI R0,RZR,0
        JMP core_return_3
        ADI R0,RZR,0
core_return_3:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_newline:
        PSH R8
        ADD R8,SP,RZR
        ADI R0,RZR,13
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,10
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,0
        JMP core_return_6
        ADI R0,RZR,0
core_return_6:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_decimal:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+8]
        ADI R1,RZR,10
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_8
        LRM R0,[R8+8]
        ADI R1,RZR,10
        Call core_div
        PSH R0
        Call core_decimal
        ADI SP,SP,4
        JMP core_if_end_9
core_else_8:
core_if_end_9:
        ADI R0,RZR,48
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,10
        Call core_div
        ADD R0,R1,RZR
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,0
        JMP core_return_7
        ADI R0,RZR,0
core_return_7:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_upper:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+8]
        ADI R1,RZR,97
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_14
        LRM R0,[R8+8]
        ADI R1,RZR,122
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_14:
        SNZ R0
        JMP core_else_12
        LRM R0,[R8+8]
        ADI R0,R0,-32
        JMP core_return_11
        JMP core_if_end_13
core_else_12:
core_if_end_13:
        LRM R0,[R8+8]
        JMP core_return_11
        ADI R0,RZR,0
core_return_11:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_le16:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+8]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8+8]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,8
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        JMP core_return_15
        ADI R0,RZR,0
core_return_15:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_le32:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+8]
        PSH R0
        Call core_le16
        ADI SP,SP,4
        PSH R0
        LRM R0,[R8+8]
        ADI R0,R0,8
        PSH R0
        Call core_le16
        ADI SP,SP,4
        ADI R1,RZR,16
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        JMP core_return_16
        ADI R0,RZR,0
core_return_16:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_set16:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,255
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8+12]
        ADI R0,R0,4
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,8
        ADI R1,RZR,256
        Call core_div
        ADI R1,RZR,255
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_17
        ADI R0,RZR,0
core_return_17:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_clear:
        PSH R8
        ADD R8,SP,RZR
core_while_19:
        LRM R0,[R8+8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_20
        LRM R0,[R8+12]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8+12]
        ADI R0,R0,4
        SRM R0,[R8+12]
        LRM R0,[R8+8]
        ADI R0,R0,-1
        SRM R0,[R8+8]
        JMP core_while_19
core_while_end_20:
        ADI R0,RZR,0
        JMP core_return_18
        ADI R0,RZR,0
core_return_18:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_copytext:
        PSH R8
        ADD R8,SP,RZR
core_while_22:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_23
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8+12]
        ADI R0,R0,4
        SRM R0,[R8+12]
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_22
core_while_end_23:
        LRM R0,[R8+12]
        ADI R1,RZR,0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_21
        ADI R0,RZR,0
core_return_21:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_memcopy:
        PSH R8
        ADD R8,SP,RZR
core_while_25:
        LRM R0,[R8+8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_26
        LRM R0,[R8+16]
        PSH R0
        LRM R0,[R8+12]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8+16]
        ADI R0,R0,4
        SRM R0,[R8+16]
        LRM R0,[R8+12]
        ADI R0,R0,4
        SRM R0,[R8+12]
        LRM R0,[R8+8]
        ADI R0,R0,-1
        SRM R0,[R8+8]
        JMP core_while_25
core_while_end_26:
        ADI R0,RZR,0
        JMP core_return_24
        ADI R0,RZR,0
core_return_24:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_set32:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8+8]
        LRM R1,[R12+core_literal_28-8000h]
        ANL R0,R0,R1
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R8+12]
        ADI R0,R0,8
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,16
        LRM R1,[R12+core_literal_29-8000h]
        Call core_div
        PSH R0
        Call core_set16
        ADI SP,SP,8
        ADI R0,RZR,0
        JMP core_return_27
        ADI R0,RZR,0
core_return_27:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_equal:
        PSH R8
        ADD R8,SP,RZR
core_while_31:
        LRM R0,[R8+12]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_32
        LRM R0,[R8+12]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_33
        ADI R0,RZR,1
        JMP core_return_30
        JMP core_if_end_34
core_else_33:
core_if_end_34:
        LRM R0,[R8+12]
        ADI R0,R0,4
        SRM R0,[R8+12]
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_31
core_while_end_32:
        ADI R0,RZR,0
        JMP core_return_30
        ADI R0,RZR,0
core_return_30:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_init:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-8
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_36:
        LRM R0,[R8-4]
        ADI R1,RZR,4
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_37
        LRM R0,[R12+core_literal_38-8000h]
        PSH R0
        LRM R0,[R8-4]
        ADI R1,RZR,512
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,4
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,36
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,64
        ADI R1,RZR,92
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,68
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_36
core_while_end_37:
        LRM R0,[R12+core_literal_39-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_40-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_35
        ADI R0,RZR,0
core_return_35:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_select:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-48
        LRM R0,[R8+8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_45
        LRM R0,[R8+8]
        ADI R1,RZR,4
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_45:
        SNZ R0
        JMP core_else_43
        ADI R0,RZR,15
        JMP core_return_42
        JMP core_if_end_44
core_else_43:
core_if_end_44:
        LRM R0,[R12+core_literal_38-8000h]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,512
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-40]
        LRM R0,[R12+core_literal_46-8000h]
        LRM R1,[R8-40]
        SRM R1,[R0]
        Call core_media_refresh
        LRM R0,[R8-40]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_49
        LRM R0,[R12+core_literal_40-8000h]
        LRM R0,[R0]
        LRM R1,[R8+8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_49:
        SNZ R0
        JMP core_else_47
        ADI R0,RZR,0
        JMP core_return_42
        JMP core_if_end_48
core_else_47:
core_if_end_48:
        ADI R0,RZR,0
        PSH R0
        LRM R0,[R12+core_literal_50-8000h]
        PSH R0
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-44]
        LRM R0,[R8-44]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_51
        LRM R0,[R8-44]
        JMP core_return_42
        JMP core_if_end_52
core_else_51:
core_if_end_52:
        ADI R0,RZR,0
        SRM R0,[R8-36]
        LRM R0,[R12+core_literal_50-8000h]
        LRM R0,[R0]
        ADI R1,RZR,75
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_55
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,66
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_55
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,8
        LRM R0,[R0]
        ADI R1,RZR,80
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_55
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,12
        LRM R0,[R0]
        ADI R1,RZR,49
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_55:
        SNZ R0
        JMP core_else_53
        ADI R0,RZR,162
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        PSH R0
        LRM R0,[R12+core_literal_50-8000h]
        PSH R0
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-44]
        LRM R0,[R8-44]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_56
        LRM R0,[R8-44]
        JMP core_return_42
        JMP core_if_end_57
core_else_56:
core_if_end_57:
        JMP core_if_end_54
core_else_53:
core_if_end_54:
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,44
        PSH R0
        Call core_le16
        ADI SP,SP,4
        ADI R1,RZR,512
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_60
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,52
        LRM R0,[R0]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_60
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,64
        LRM R0,[R0]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_60:
        SNZ R0
        JMP core_else_58
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_59
core_else_58:
core_if_end_59:
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,56
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-28]
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,68
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-12]
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,76
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-48]
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,88
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-16]
        LRM R0,[R8-28]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_63
        LRM R0,[R8-12]
        ADI R1,RZR,16
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_63
        LRM R0,[R8-12]
        ADI R1,RZR,512
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_63
        LRM R0,[R8-12]
        ADI R1,RZR,15
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_63:
        SNZ R0
        JMP core_else_61
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_62
core_else_61:
core_if_end_62:
        LRM R0,[R8-16]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_66
        LRM R0,[R8-16]
        ADI R1,RZR,9
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_66
        LRM R0,[R8-48]
        LRM R1,[R8-36]
        ADD R0,R0,R1
        ADI R1,RZR,2880
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_66
        LRM R0,[R8-48]
        ADI R1,RZR,64
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
core_bool_end_66:
        SNZ R0
        JMP core_else_64
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_65
core_else_64:
core_if_end_65:
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,2040
        LRM R0,[R0]
        ADI R1,RZR,85
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_69
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,2044
        LRM R0,[R0]
        ADI R1,RZR,170
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_69:
        SNZ R0
        JMP core_else_67
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_68
core_else_67:
core_if_end_68:
        LRM R0,[R8-36]
        LRM R1,[R8-28]
        ADD R0,R0,R1
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        PSH R0
        LRM R0,[R8-16]
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        PSH R0
        LRM R0,[R8-12]
        ADI R1,RZR,16
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-8]
        LRM R0,[R8-48]
        LRM R1,[R8-36]
        ADD R0,R0,R1
        LRM R1,[R8-8]
        SUB R0,R0,R1
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_72
        LRM R0,[R8-4]
        ADI R1,RZR,4085
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_72
        LRM R0,[R8-4]
        ADI R0,R0,2
        ADI R1,RZR,3
        Call core_mul
        PSH R0
        LRM R0,[R8-16]
        ADI R1,RZR,1024
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
core_bool_end_72:
        SNZ R0
        JMP core_else_70
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_71
core_else_70:
core_if_end_71:
        LRM R0,[R8-40]
        ADI R0,R0,8
        LRM R1,[R8-36]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,12
        LRM R1,[R8-20]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,16
        LRM R1,[R8-16]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,20
        LRM R1,[R8-32]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,24
        LRM R1,[R8-12]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,28
        LRM R1,[R8-8]
        SRM R1,[R0]
        LRM R0,[R8-40]
        ADI R0,R0,32
        LRM R1,[R8-4]
        SRM R1,[R0]
        ADI R0,RZR,0
        SRM R0,[R8-24]
core_while_73:
        LRM R0,[R8-24]
        LRM R1,[R8-16]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_74
        LRM R0,[R8-20]
        LRM R1,[R8-24]
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R12+core_literal_75-8000h]
        PSH R0
        LRM R0,[R8-24]
        ADI R1,RZR,2048
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-44]
        LRM R0,[R8-44]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_76
        LRM R0,[R8-44]
        JMP core_return_42
        JMP core_if_end_77
core_else_76:
core_if_end_77:
        LRM R0,[R8-24]
        ADI R0,R0,1
        SRM R0,[R8-24]
        JMP core_while_73
core_while_end_74:
        LRM R0,[R12+core_literal_75-8000h]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_50-8000h]
        ADI R0,R0,84
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_80
        LRM R0,[R12+core_literal_75-8000h]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,255
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_80
        LRM R0,[R12+core_literal_75-8000h]
        ADI R0,R0,8
        LRM R0,[R0]
        ADI R1,RZR,255
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_80:
        SNZ R0
        JMP core_else_78
        ADI R0,RZR,13
        JMP core_return_42
        JMP core_if_end_79
core_else_78:
core_if_end_79:
        LRM R0,[R8-40]
        ADI R0,R0,4
        ADI R1,RZR,1
        SRM R1,[R0]
        LRM R0,[R12+core_literal_40-8000h]
        LRM R1,[R8+8]
        SRM R1,[R0]
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_42
        ADI R0,RZR,0
core_return_42:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_fatget:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-8]
        LRM R0,[R8+8]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_84
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_84:
        SNZ R0
        JMP core_else_82
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_81
        JMP core_if_end_83
core_else_82:
core_if_end_83:
        LRM R0,[R12+core_literal_75-8000h]
        PSH R0
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,2
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-4]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,8
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-12]
        LRM R0,[R8+8]
        ADI R1,RZR,1
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_86
        LRM R0,[R8-12]
        ADI R1,RZR,4
        ADI R1,RZR,16
        Call core_div
        JMP core_return_81
        JMP core_if_end_87
core_else_86:
core_if_end_87:
        LRM R0,[R8-12]
        ADI R1,RZR,4095
        ANL R0,R0,R1
        JMP core_return_81
        ADI R0,RZR,0
core_return_81:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_fatset:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        LRM R0,[R12+core_literal_75-8000h]
        PSH R0
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8+12]
        ADI R1,RZR,2
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-4]
        LRM R0,[R8+12]
        ADI R1,RZR,1
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_89
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8-4]
        LRM R0,[R0]
        ADI R1,RZR,15
        ANL R0,R0,R1
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,4
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADI R1,RZR,240
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        ORR R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-4]
        ADI R0,R0,4
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,4
        ADI R1,RZR,16
        Call core_div
        ADI R1,RZR,255
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        JMP core_if_end_90
core_else_89:
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,255
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-4]
        ADI R0,R0,4
        PSH R0
        LRM R0,[R8-4]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,240
        ANL R0,R0,R1
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,8
        ADI R1,RZR,256
        Call core_div
        ADI R1,RZR,15
        ANL R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        ORR R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
core_if_end_90:
        ADI R0,RZR,0
        JMP core_return_88
        ADI R0,RZR,0
core_return_88:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_fatflush:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-16
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_92:
        LRM R0,[R8-4]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_93
        ADI R0,RZR,0
        SRM R0,[R8-8]
core_while_94:
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R8-12]
        ADI R0,R0,16
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_95
        LRM R0,[R8-12]
        ADI R0,R0,12
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8-12]
        ADI R0,R0,16
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-8]
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R12+core_literal_75-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADI R1,RZR,2048
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R8-12]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_96
        LRM R0,[R12+core_literal_40-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-16]
        JMP core_return_91
        JMP core_if_end_97
core_else_96:
core_if_end_97:
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP core_while_94
core_while_end_95:
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_92
core_while_end_93:
        ADI R0,RZR,0
        JMP core_return_91
        ADI R0,RZR,0
core_return_91:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_allocate:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-8
        ADI R0,RZR,2
        SRM R0,[R8-4]
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-8]
core_while_99:
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_100
        LRM R0,[R8-4]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_101
        LRM R0,[R8-4]
        PSH R0
        ADI R0,RZR,4095
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        LRM R0,[R8-4]
        JMP core_return_98
        JMP core_if_end_102
core_else_101:
core_if_end_102:
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_99
core_while_end_100:
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,112
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_98
        ADI R0,RZR,0
core_return_98:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_read_dir_sector:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        LRM R1,[R8+8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_104
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_107
        LRM R0,[R12+core_literal_85-8000h]
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-4]
        JMP core_return_103
        JMP core_if_end_108
core_else_107:
core_if_end_108:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R1,[R8+8]
        SRM R1,[R0]
        JMP core_if_end_105
core_else_104:
core_if_end_105:
        ADI R0,RZR,0
        JMP core_return_103
        ADI R0,RZR,0
core_return_103:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_dir_entry:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R8+12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_110
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-12]
        ADI R0,R0,24
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_112
        ADI R0,RZR,0
        JMP core_return_109
        JMP core_if_end_113
core_else_112:
core_if_end_113:
        LRM R0,[R8-12]
        ADI R0,R0,20
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,16
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-8]
        JMP core_if_end_111
core_else_110:
        LRM R0,[R8-12]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-4]
core_while_114:
        LRM R0,[R8+8]
        ADI R1,RZR,16
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_115
        LRM R0,[R8+12]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        SRM R0,[R8+12]
        LRM R0,[R8+12]
        ADI R1,RZR,4088
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_116
        ADI R0,RZR,0
        JMP core_return_109
        JMP core_if_end_117
core_else_116:
core_if_end_117:
        LRM R0,[R8+12]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_120
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_120:
        SNZ R0
        JMP core_else_118
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_109
        JMP core_if_end_119
core_else_118:
core_if_end_119:
        LRM R0,[R8-4]
        ADI R0,R0,-1
        SRM R0,[R8-4]
        LRM R0,[R8+8]
        ADI R0,R0,-16
        SRM R0,[R8+8]
        JMP core_while_114
core_while_end_115:
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-12]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_121
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_109
        JMP core_if_end_122
core_else_121:
core_if_end_122:
        LRM R0,[R8-12]
        ADI R0,R0,28
        LRM R0,[R0]
        LRM R1,[R8+12]
        ADD R0,R0,R1
        ADI R0,R0,-2
        SRM R0,[R8-8]
core_if_end_111:
        LRM R0,[R8-8]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_123
        ADI R0,RZR,0
        JMP core_return_109
        JMP core_if_end_124
core_else_123:
core_if_end_124:
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,15
        ANL R0,R0,R1
        ADI R1,RZR,128
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        JMP core_return_109
        ADI R0,RZR,0
core_return_109:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_name_equal:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_126:
        LRM R0,[R8-4]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_127
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_128
        ADI R0,RZR,0
        JMP core_return_125
        JMP core_if_end_129
core_else_128:
core_if_end_129:
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_126
core_while_end_127:
        ADI R0,RZR,1
        JMP core_return_125
        ADI R0,RZR,0
core_return_125:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_find:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_132:
        LRM R0,[R8-12]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_133
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-12]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_134
        ADI R0,RZR,0
        JMP core_return_131
        JMP core_if_end_135
core_else_134:
core_if_end_135:
        LRM R0,[R8-4]
        LRM R0,[R0]
        SRM R0,[R8-8]
        LRM R0,[R8+8]
        SNZ R0
        JMP core_else_136
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_140
        LRM R0,[R8-8]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_140:
        SNZ R0
        JMP core_else_138
        LRM R0,[R8-4]
        JMP core_return_131
        JMP core_if_end_139
core_else_138:
core_if_end_139:
        JMP core_if_end_137
core_else_136:
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_141
        ADI R0,RZR,0
        JMP core_return_131
        JMP core_if_end_142
core_else_141:
core_if_end_142:
        LRM R0,[R8-8]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_145
        LRM R0,[R8-4]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,8
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_145
        LRM R0,[R8-4]
        PSH R0
        Call core_name_equal
        ADI SP,SP,4
core_bool_end_145:
        SNZ R0
        JMP core_else_143
        LRM R0,[R8-4]
        JMP core_return_131
        JMP core_if_end_144
core_else_143:
core_if_end_144:
core_if_end_137:
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_132
core_while_end_133:
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_131
        ADI R0,RZR,0
core_return_131:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_path:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-32
        LRM R0,[R12+core_literal_39-8000h]
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8+8]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,58
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_147
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R0,R0,-48
        SRM R0,[R8-4]
        LRM R0,[R8+8]
        ADI R0,R0,8
        SRM R0,[R8+8]
        JMP core_if_end_148
core_else_147:
core_if_end_148:
        LRM R0,[R8-4]
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_149
        LRM R0,[R8-32]
        JMP core_return_146
        JMP core_if_end_150
core_else_149:
core_if_end_150:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-28]
        ADI R0,RZR,1
        SRM R0,[R8-20]
        LRM R0,[R12+core_literal_151-8000h]
        ADI R1,RZR,92
        SRM R1,[R0]
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_152
        LRM R0,[R8-28]
        ADI R0,R0,68
        SRM R0,[R8-16]
core_while_154:
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_155
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-16]
        ADI R0,R0,4
        SRM R0,[R8-16]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        JMP core_while_154
core_while_end_155:
        JMP core_if_end_153
core_else_152:
core_if_end_153:
core_while_156:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_158
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_158
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,9
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_158:
        SNZ R0
        JMP core_while_end_157
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_159
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_156
        JMP core_if_end_160
core_else_159:
core_if_end_160:
        LRM R0,[R8+8]
        SRM R0,[R8-24]
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_161:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_163
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_163
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_163
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,9
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_163:
        SNZ R0
        JMP core_while_end_162
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_161
core_while_end_162:
        LRM R0,[R8-12]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_166
        LRM R0,[R8-24]
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_166:
        SNZ R0
        JMP core_else_164
        JMP core_while_156
        JMP core_if_end_165
core_else_164:
core_if_end_165:
        LRM R0,[R8-12]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_169
        LRM R0,[R8-24]
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_169
        LRM R0,[R8-24]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_169:
        SNZ R0
        JMP core_else_167
core_while_170:
        LRM R0,[R8-20]
        ADI R1,RZR,1
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_172
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,-1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_172:
        SNZ R0
        JMP core_while_end_171
        LRM R0,[R8-20]
        ADI R0,R0,-1
        SRM R0,[R8-20]
        JMP core_while_170
core_while_end_171:
        LRM R0,[R8-20]
        ADI R1,RZR,1
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_173
        LRM R0,[R8-20]
        ADI R0,R0,-1
        SRM R0,[R8-20]
        JMP core_if_end_174
core_else_173:
core_if_end_174:
        JMP core_while_156
        JMP core_if_end_168
core_else_167:
core_if_end_168:
        LRM R0,[R8-20]
        ADI R1,RZR,1
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_175
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,92
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        JMP core_if_end_176
core_else_175:
core_if_end_176:
        LRM R0,[R8-20]
        LRM R1,[R8-12]
        ADD R0,R0,R1
        ADI R1,RZR,64
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_177
        ADI R0,RZR,206
        JMP core_return_146
        JMP core_if_end_178
core_else_177:
core_if_end_178:
        ADI R0,RZR,0
        SRM R0,[R8-8]
core_while_179:
        LRM R0,[R8-8]
        LRM R1,[R8-12]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_180
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R8-24]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        Call core_upper
        ADI SP,SP,4
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP core_while_179
core_while_end_180:
        JMP core_while_156
core_while_end_157:
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_146
        ADI R0,RZR,0
core_return_146:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_make_name:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-16
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_182:
        LRM R0,[R8-12]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_183
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,32
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_182
core_while_end_183:
        ADI R0,RZR,0
        SRM R0,[R8-16]
        ADI R0,RZR,0
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_184:
        LRM R0,[R8-12]
        LRM R1,[R8+8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_185
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        Call core_upper
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_186
        LRM R0,[R8-8]
        SRZ R0
        JMP core_bool_end_190
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_190:
        SNZ R0
        JMP core_else_188
        ADI R0,RZR,3
        JMP core_return_181
        JMP core_if_end_189
core_else_188:
core_if_end_189:
        ADI R0,RZR,1
        SRM R0,[R8-8]
        ADI R0,RZR,8
        SRM R0,[R8-16]
        JMP core_if_end_187
core_else_186:
        LRM R0,[R8-4]
        ADI R1,RZR,33
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,34
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,42
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,43
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,44
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,47
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,58
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,59
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,60
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,61
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,62
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,63
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,91
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,93
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_193
        LRM R0,[R8-4]
        ADI R1,RZR,124
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_193:
        SNZ R0
        JMP core_else_191
        ADI R0,RZR,3
        JMP core_return_181
        JMP core_if_end_192
core_else_191:
core_if_end_192:
        LRM R0,[R8-8]
        SNZ R0
        JMP core_bool_end_197
        LRM R0,[R8-16]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_197:
        SRZ R0
        JMP core_bool_end_196
        LRM R0,[R8-8]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_198
        LRM R0,[R8-16]
        ADI R1,RZR,8
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_198:
core_bool_end_196:
        SNZ R0
        JMP core_else_194
        ADI R0,RZR,3
        JMP core_return_181
        JMP core_if_end_195
core_else_194:
core_if_end_195:
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-16]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-16]
        ADI R0,R0,1
        SRM R0,[R8-16]
core_if_end_187:
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_184
core_while_end_185:
        ADI R0,RZR,0
        JMP core_return_181
        ADI R0,RZR,0
core_return_181:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_resolve:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        LRM R0,[R8+12]
        PSH R0
        Call core_path
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_200
        LRM R0,[R8-4]
        JMP core_return_199
        JMP core_if_end_201
core_else_200:
core_if_end_201:
        LRM R0,[R8+8]
        PSH R0
        Call core_walk
        ADI SP,SP,4
        JMP core_return_199
        ADI R0,RZR,0
core_return_199:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_walk:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-28
        LRM R0,[R12+core_literal_203-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_204-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_205-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_206-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_151-8000h]
        ADI R0,R0,4
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-4]
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_207
        LRM R0,[R12+core_literal_203-8000h]
        ADI R1,RZR,1
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_202
        JMP core_if_end_208
core_else_207:
core_if_end_208:
core_while_209:
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_210
        LRM R0,[R8-20]
        SRM R0,[R8-24]
        ADI R0,RZR,0
        SRM R0,[R8-16]
core_while_211:
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_213
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_213:
        SNZ R0
        JMP core_while_end_212
        LRM R0,[R8-16]
        ADI R0,R0,1
        SRM R0,[R8-16]
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-20]
        JMP core_while_211
core_while_end_212:
        LRM R0,[R8-24]
        PSH R0
        LRM R0,[R8-16]
        PSH R0
        Call core_make_name
        ADI SP,SP,8
        SRM R0,[R8-28]
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_214
        LRM R0,[R8-28]
        JMP core_return_202
        JMP core_if_end_215
core_else_214:
core_if_end_215:
        LRM R0,[R8-4]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_find
        ADI SP,SP,8
        SRM R0,[R8-8]
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRM R0,[R8-12]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_216
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_218
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_202
        JMP core_if_end_219
core_else_218:
core_if_end_219:
        LRM R0,[R8+8]
        SNZ R0
        JMP core_bool_end_222
        LRM R0,[R8-12]
core_bool_end_222:
        SNZ R0
        JMP core_else_220
        LRM R0,[R12+core_literal_205-8000h]
        LRM R1,[R8-4]
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_202
        JMP core_if_end_221
core_else_220:
core_if_end_221:
        ADI R0,RZR,3
        JMP core_return_202
        JMP core_if_end_217
core_else_216:
core_if_end_217:
        LRM R0,[R8-12]
        SNZ R0
        JMP core_else_223
        LRM R0,[R12+core_literal_204-8000h]
        LRM R1,[R8-8]
        SRM R1,[R0]
        LRM R0,[R12+core_literal_205-8000h]
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R12+core_literal_206-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_202
        JMP core_if_end_224
core_else_223:
core_if_end_224:
        LRM R0,[R8-8]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_225
        ADI R0,RZR,3
        JMP core_return_202
        JMP core_if_end_226
core_else_225:
core_if_end_226:
        LRM R0,[R8-8]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-20]
        JMP core_while_209
core_while_end_210:
        ADI R0,RZR,3
        JMP core_return_202
        ADI R0,RZR,0
core_return_202:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_print_name:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_228:
        LRM R0,[R8-4]
        ADI R1,RZR,8
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_230
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_230:
        SNZ R0
        JMP core_while_end_229
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        Call core_putc
        ADI SP,SP,4
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_228
core_while_end_229:
        LRM R0,[R8+8]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_231
        ADI R0,RZR,46
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,8
        SRM R0,[R8-4]
core_while_233:
        LRM R0,[R8-4]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_235
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_235:
        SNZ R0
        JMP core_while_end_234
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        Call core_putc
        ADI SP,SP,4
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_233
core_while_end_234:
        JMP core_if_end_232
core_else_231:
core_if_end_232:
        ADI R0,RZR,0
        JMP core_return_227
        ADI R0,RZR,0
core_return_227:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_directory_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-40
        ADI R0,RZR,0
        SRM R0,[R8-4]
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_237
        ADI R0,RZR,87
        JMP core_return_236
        JMP core_if_end_238
core_else_237:
core_if_end_238:
        LRM R0,[R12+core_literal_240-8000h]
        SRM R0,[R8-36]
        ADI R0,RZR,0
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-24]
core_while_241:
        LRM R0,[R8-24]
        LRM R1,[R8-8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_242
        LRM R0,[R12+core_literal_243-8000h]
        PSH R0
        LRM R0,[R8-24]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        PSH R0
        LRM R0,[R12+core_literal_247-8000h]
        PSH R0
        Call core_equal
        ADI SP,SP,8
        SNZ R0
        JMP core_else_244
        ADI R0,RZR,1
        SRM R0,[R8-4]
        JMP core_if_end_245
core_else_244:
        LRM R0,[R8-40]
        LRM R0,[R0]
        ADI R1,RZR,47
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_250
        LRM R0,[R8-20]
core_bool_end_250:
        SNZ R0
        JMP core_else_248
        ADI R0,RZR,87
        JMP core_return_236
        JMP core_if_end_249
core_else_248:
        LRM R0,[R8-40]
        SRM R0,[R8-36]
        ADI R0,RZR,1
        SRM R0,[R8-20]
core_if_end_249:
core_if_end_245:
        LRM R0,[R8-24]
        ADI R0,R0,1
        SRM R0,[R8-24]
        JMP core_while_241
core_while_end_242:
        LRM R0,[R8-36]
        PSH R0
        Call core_search
        ADI SP,SP,4
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_251
        LRM R0,[R8-32]
        JMP core_return_236
        JMP core_if_end_252
core_else_251:
core_if_end_252:
        LRM R0,[R12+core_literal_253-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R8-4]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_254
        LRM R0,[R12+core_literal_257-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        ADI R0,RZR,48
        PSH R0
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,58
        PSH R0
        Call core_putc
        ADI SP,SP,4
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_newline
        JMP core_if_end_255
core_else_254:
core_if_end_255:
        ADI R0,RZR,0
        SRM R0,[R8-28]
core_while_258:
        LRM R0,[R8-28]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_259
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_262
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_262:
        SNZ R0
        JMP core_else_260
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_236
        JMP core_if_end_261
core_else_260:
core_if_end_261:
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_265
        LRM R0,[R8-16]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,14
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_265
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        Call core_matches
        ADI SP,SP,8
core_bool_end_265:
        SNZ R0
        JMP core_else_263
        LRM R0,[R8-16]
        PSH R0
        Call core_print_name
        ADI SP,SP,4
        LRM R0,[R8-4]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_267
        LRM R0,[R8-16]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_269
        LRM R0,[R12+core_literal_272-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_270
core_else_269:
        LRM R0,[R12+core_literal_274-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        LRM R0,[R8-16]
        ADI R0,R0,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        PSH R0
        Call core_decimal
        ADI SP,SP,4
core_if_end_270:
        JMP core_if_end_268
core_else_267:
core_if_end_268:
        Call core_newline
        JMP core_if_end_264
core_else_263:
core_if_end_264:
        LRM R0,[R8-28]
        ADI R0,R0,1
        SRM R0,[R8-28]
        JMP core_while_258
core_while_end_259:
        ADI R0,RZR,13
        JMP core_return_236
        ADI R0,RZR,0
core_return_236:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_mkdir_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-32
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_276
        ADI R0,RZR,87
        JMP core_return_275
        JMP core_if_end_277
core_else_276:
core_if_end_277:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        SRM R0,[R8+8]
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_278
        LRM R0,[R8-32]
        JMP core_return_275
        JMP core_if_end_279
core_else_278:
core_if_end_279:
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SRZ R0
        JMP core_bool_end_282
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_282:
        SNZ R0
        JMP core_else_280
        ADI R0,RZR,5
        JMP core_return_275
        JMP core_if_end_281
core_else_280:
core_if_end_281:
        LRM R0,[R12+core_literal_205-8000h]
        LRM R0,[R0]
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        PSH R0
        Call core_free_entry
        ADI SP,SP,4
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_283
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_285
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_275
        JMP core_if_end_286
core_else_285:
core_if_end_286:
        ADI R0,RZR,112
        JMP core_return_275
        JMP core_if_end_284
core_else_283:
core_if_end_284:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-20]
        LRM R0,[R8-8]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-24]
        Call core_allocate
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_287
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_275
        JMP core_if_end_288
core_else_287:
core_if_end_288:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-28]
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        ADI R0,RZR,512
        PSH R0
        Call core_clear
        ADI SP,SP,8
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_290:
        LRM R0,[R8-12]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_291
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,32
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,128
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,32
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_290
core_while_end_291:
        LRM R0,[R12+core_literal_289-8000h]
        ADI R1,RZR,46
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,44
        ADI R1,RZR,16
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,104
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,128
        ADI R1,RZR,46
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,132
        ADI R1,RZR,46
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,172
        ADI R1,RZR,16
        SRM R1,[R0]
        LRM R0,[R12+core_literal_289-8000h]
        ADI R0,R0,232
        PSH R0
        LRM R0,[R8-16]
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R8-28]
        ADI R0,R0,28
        LRM R0,[R0]
        LRM R1,[R8-4]
        ADD R0,R0,R1
        ADI R0,R0,-2
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_292
        LRM R0,[R12+core_literal_40-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-32]
        JMP core_return_275
        JMP core_if_end_293
core_else_292:
core_if_end_293:
        Call core_fatflush
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_294
        LRM R0,[R8-32]
        JMP core_return_275
        JMP core_if_end_295
core_else_294:
core_if_end_295:
        LRM R0,[R8-20]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_296
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_275
        JMP core_if_end_297
core_else_296:
core_if_end_297:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-24]
        ADD R0,R0,R1
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        PSH R0
        ADI R0,RZR,32
        PSH R0
        Call core_clear
        ADI SP,SP,8
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_298:
        LRM R0,[R8-12]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_299
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_298
core_while_end_299:
        LRM R0,[R8-8]
        ADI R0,R0,44
        ADI R1,RZR,16
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,104
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R8-20]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-28]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-32]
        LRM R0,[R8-32]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_300
        LRM R0,[R8-4]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        Call core_fatflush
        JMP core_if_end_301
core_else_300:
core_if_end_301:
        LRM R0,[R8-32]
        JMP core_return_275
        ADI R0,RZR,0
core_return_275:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_rmdir_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-40
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_303
        ADI R0,RZR,87
        JMP core_return_302
        JMP core_if_end_304
core_else_303:
core_if_end_304:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        SRM R0,[R8+8]
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_305
        LRM R0,[R8-40]
        JMP core_return_302
        JMP core_if_end_306
core_else_305:
core_if_end_306:
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        SRM R0,[R8-16]
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SRZ R0
        JMP core_bool_end_309
        LRM R0,[R8-16]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_309:
        SNZ R0
        JMP core_else_307
        ADI R0,RZR,5
        JMP core_return_302
        JMP core_if_end_308
core_else_307:
core_if_end_308:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-36]
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADI R0,R0,64
        PSH R0
        Call core_equal
        ADI SP,SP,8
        SNZ R0
        JMP core_else_310
        ADI R0,RZR,16
        JMP core_return_302
        JMP core_if_end_311
core_else_310:
core_if_end_311:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-28]
        LRM R0,[R8-16]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-32]
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        SRM R0,[R8-20]
        LRM R0,[R8-36]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-4]
core_while_312:
        LRM R0,[R8-20]
        ADI R1,RZR,4088
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_313
        LRM R0,[R8-20]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_316
        LRM R0,[R8-20]
        PSH R0
        LRM R0,[R8-36]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_316
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_316:
        SNZ R0
        JMP core_else_314
        ADI R0,RZR,13
        JMP core_return_302
        JMP core_if_end_315
core_else_314:
core_if_end_315:
        LRM R0,[R8-20]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        SRM R0,[R8-20]
        LRM R0,[R8-4]
        ADI R0,R0,-1
        SRM R0,[R8-4]
        JMP core_while_312
core_while_end_313:
        ADI R0,RZR,2
        SRM R0,[R8-24]
core_while_317:
        LRM R0,[R8-24]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_318
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R8-24]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-12]
        LRM R0,[R8-12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_321
        LRM R0,[R8-12]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_321:
        SNZ R0
        JMP core_else_319
        JMP core_while_end_318
        JMP core_if_end_320
core_else_319:
core_if_end_320:
        LRM R0,[R8-12]
        LRM R0,[R0]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_322
        ADI R0,RZR,5
        JMP core_return_302
        JMP core_if_end_323
core_else_322:
core_if_end_323:
        LRM R0,[R8-24]
        ADI R0,R0,1
        SRM R0,[R8-24]
        JMP core_while_317
core_while_end_318:
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_324
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_302
        JMP core_if_end_325
core_else_324:
core_if_end_325:
        LRM R0,[R8-28]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_326
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_302
        JMP core_if_end_327
core_else_326:
core_if_end_327:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-32]
        ADD R0,R0,R1
        ADI R1,RZR,229
        SRM R1,[R0]
        LRM R0,[R8-28]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-36]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_328
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-40]
        JMP core_return_302
        JMP core_if_end_329
core_else_328:
core_if_end_329:
        LRM R0,[R8-36]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-4]
core_while_330:
        LRM R0,[R8-8]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_332
        LRM R0,[R8-8]
        ADI R1,RZR,4088
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
core_bool_end_332:
        SNZ R0
        JMP core_while_end_331
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_333
        ADI R0,RZR,13
        JMP core_return_302
        JMP core_if_end_334
core_else_333:
core_if_end_334:
        LRM R0,[R8-8]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        SRM R0,[R8-20]
        LRM R0,[R8-8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        LRM R0,[R8-20]
        SRM R0,[R8-8]
        LRM R0,[R8-4]
        ADI R0,R0,-1
        SRM R0,[R8-4]
        JMP core_while_330
core_while_end_331:
        Call core_fatflush
        JMP core_return_302
        ADI R0,RZR,0
core_return_302:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_prompt_drive:
        PSH R8
        ADD R8,SP,RZR
        ADI R0,RZR,48
        PSH R0
        LRM R0,[R12+core_literal_39-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        PSH R0
        Call core_putc
        ADI SP,SP,4
        ADI R0,RZR,0
        JMP core_return_335
        ADI R0,RZR,0
core_return_335:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_error_message:
        PSH R8
        ADD R8,SP,RZR
        LRM R0,[R8+8]
        ADI R1,RZR,3
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_339
        LRM R0,[R8+8]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_339:
        SNZ R0
        JMP core_else_337
        LRM R0,[R12+core_literal_341-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_338
core_else_337:
        LRM R0,[R8+8]
        ADI R1,RZR,5
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_342
        LRM R0,[R12+core_literal_345-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_343
core_else_342:
        LRM R0,[R8+8]
        ADI R1,RZR,15
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_346
        LRM R0,[R12+core_literal_349-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_347
core_else_346:
        LRM R0,[R8+8]
        ADI R1,RZR,16
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_350
        LRM R0,[R12+core_literal_353-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_351
core_else_350:
        LRM R0,[R8+8]
        ADI R1,RZR,19
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_354
        LRM R0,[R12+core_literal_357-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_355
core_else_354:
        LRM R0,[R8+8]
        ADI R1,RZR,21
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_358
        LRM R0,[R12+core_literal_361-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_359
core_else_358:
        LRM R0,[R8+8]
        ADI R1,RZR,112
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_362
        LRM R0,[R12+core_literal_365-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_363
core_else_362:
        LRM R0,[R8+8]
        ADI R1,RZR,87
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_366
        LRM R0,[R12+core_literal_369-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_367
core_else_366:
        LRM R0,[R8+8]
        ADI R1,RZR,183
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_370
        LRM R0,[R12+core_literal_373-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_371
core_else_370:
        LRM R0,[R8+8]
        ADI R1,RZR,82
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_374
        LRM R0,[R12+core_literal_377-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_375
core_else_374:
        LRM R0,[R8+8]
        ADI R1,RZR,206
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_378
        LRM R0,[R12+core_literal_381-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_379
core_else_378:
        LRM R0,[R12+core_literal_383-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
core_if_end_379:
core_if_end_375:
core_if_end_371:
core_if_end_367:
core_if_end_363:
core_if_end_359:
core_if_end_355:
core_if_end_351:
core_if_end_347:
core_if_end_343:
core_if_end_338:
        Call core_newline
        ADI R0,RZR,0
        JMP core_return_336
        ADI R0,RZR,0
core_return_336:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_fs_command:
        MediaLock
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,0
        SRM R1,[R0]
core_while_385:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_387
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,9
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_387:
        SNZ R0
        JMP core_while_end_386
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_385
core_while_end_386:
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R12+core_literal_391-8000h]
        PSH R0
        Call core_equal
        ADI SP,SP,8
        SNZ R0
        JMP core_else_388
        LRM R0,[R8+12]
        ADI R0,R0,-9
        SNZ R0
        JMP core_chdir_help
        LRM R0,[R8+12]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_392
        LRM R0,[R12+core_literal_395-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_393
core_else_392:
        LRM R0,[R8+12]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_396
        LRM R0,[R12+core_literal_399-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_397
core_else_396:
        LRM R0,[R8+12]
        ADI R1,RZR,3
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_400
        LRM R0,[R12+core_literal_403-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_401
core_else_400:
        LRM R0,[R8+12]
        ADI R1,RZR,4
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_404
        LRM R0,[R12+core_literal_407-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_405
core_else_404:
        LRM R0,[R8+12]
        ADI R1,RZR,5
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_408
        LRM R0,[R12+core_literal_411-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_409
core_else_408:
        LRM R0,[R8+12]
        ADI R1,RZR,6
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_412
        LRM R0,[R12+core_literal_415-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_413
core_else_412:
        LRM R0,[R8+12]
        ADI R1,RZR,7
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_416
        LRM R0,[R12+core_literal_419-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP core_if_end_417
core_else_416:
        LRM R0,[R12+core_literal_421-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
core_if_end_417:
core_if_end_413:
core_if_end_409:
core_if_end_405:
core_if_end_401:
core_if_end_397:
core_if_end_393:
        Call core_newline
        ADI R0,RZR,0
        JMP core_return_384
        JMP core_if_end_389
core_else_388:
core_if_end_389:
        ADI R0,RZR,87
        SRM R0,[R8-4]
        LRM R0,[R8+12]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_422
        LRM R0,[R8+8]
        PSH R0
        Call core_directory_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_423
core_else_422:
        LRM R0,[R8+12]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_424
        LRM R0,[R8+8]
        PSH R0
        Call core_mkdir_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_425
core_else_424:
        LRM R0,[R8+12]
        ADI R1,RZR,3
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_426
        LRM R0,[R8+8]
        PSH R0
        Call core_rmdir_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_427
core_else_426:
        LRM R0,[R8+12]
        ADI R1,RZR,4
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_428
        LRM R0,[R8+8]
        PSH R0
        Call core_drive_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_429
core_else_428:
        LRM R0,[R8+12]
        ADI R1,RZR,5
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_430
        LRM R0,[R8+8]
        PSH R0
        Call core_copy_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_431
core_else_430:
        LRM R0,[R8+12]
        ADI R1,RZR,6
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_432
        LRM R0,[R8+8]
        PSH R0
        Call core_move_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_433
core_else_432:
        LRM R0,[R8+12]
        ADI R1,RZR,7
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_434
        LRM R0,[R8+8]
        PSH R0
        Call core_delete_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_435
core_else_434:
        LRM R0,[R8+12]
        ADI R1,RZR,8
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_436
        LRM R0,[R8+8]
        PSH R0
        Call core_sys_command
        ADI SP,SP,4
        SRM R0,[R8-4]
        JMP core_if_end_437
core_else_436:
        LRM R0,[R8+12]
        ADI R0,R0,-9
        SRZ R0
        JMP core_if_end_437
        LRM R0,[R8+8]
        PSH R0
        Call core_chdir_command
        ADI SP,SP,4
        SRM R0,[R8-4]
core_if_end_437:
core_if_end_435:
core_if_end_433:
core_if_end_431:
core_if_end_429:
core_if_end_427:
core_if_end_425:
core_if_end_423:
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_438
        LRM R0,[R8-4]
        PSH R0
        Call core_error_message
        ADI SP,SP,4
        LRM R0,[R12+core_literal_40-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        JMP core_if_end_439
core_else_438:
core_if_end_439:
        LRM R0,[R8-4]
        JMP core_return_384
        ADI R0,RZR,0
core_return_384:
        MediaUnlock
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_tokens:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-16
        ADI R0,RZR,0
        SRM R0,[R8-8]
        LRM R0,[R12+core_literal_441-8000h]
        SRM R0,[R8-12]
core_while_442:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_443
core_while_444:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_446
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,9
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_446:
        SNZ R0
        JMP core_while_end_445
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_444
core_while_end_445:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_447
        JMP core_while_end_443
        JMP core_if_end_448
core_else_447:
core_if_end_448:
        LRM R0,[R8-8]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_449
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_440
        JMP core_if_end_450
core_else_449:
core_if_end_450:
        LRM R0,[R12+core_literal_243-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-12]
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,43
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_451
        LRM R0,[R8-12]
        ADI R1,RZR,43
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,4
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,8
        SRM R0,[R8-12]
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_442
        JMP core_if_end_452
core_else_451:
core_if_end_452:
        ADI R0,RZR,0
        SRM R0,[R8-16]
core_while_453:
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_454
        LRM R0,[R8+8]
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,34
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_455
        ADI R0,RZR,1
        LRM R1,[R8-16]
        SUB R0,R0,R1
        SRM R0,[R8-16]
        JMP core_if_end_456
core_else_455:
        LRM R0,[R8-4]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_460
        LRM R0,[R8-4]
        ADI R1,RZR,9
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_460
        LRM R0,[R8-4]
        ADI R1,RZR,43
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_460:
        SNZ R0
        JMP core_bool_end_459
        LRM R0,[R8-16]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_459:
        SNZ R0
        JMP core_else_457
        JMP core_while_end_454
        JMP core_if_end_458
core_else_457:
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_upper
        ADI SP,SP,4
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,4
        SRM R0,[R8-12]
core_if_end_458:
core_if_end_456:
        LRM R0,[R8+8]
        ADI R0,R0,4
        SRM R0,[R8+8]
        JMP core_while_453
core_while_end_454:
        LRM R0,[R8-16]
        SNZ R0
        JMP core_else_461
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_440
        JMP core_if_end_462
core_else_461:
core_if_end_462:
        LRM R0,[R8-12]
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,4
        SRM R0,[R8-12]
        JMP core_while_442
core_while_end_443:
        LRM R0,[R8-8]
        JMP core_return_440
        ADI R0,RZR,0
core_return_440:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_make_pattern:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-24
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_clear
        ADI SP,SP,8
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_464:
        LRM R0,[R8-12]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_465
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,32
        SRM R1,[R0]
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_464
core_while_end_465:
        ADI R0,RZR,0
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-24]
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_466:
        LRM R0,[R8-12]
        LRM R1,[R8+8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_467
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-12]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        Call core_upper
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_468
        LRM R0,[R8-8]
        SRZ R0
        JMP core_bool_end_472
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_472:
        SNZ R0
        JMP core_else_470
        ADI R0,RZR,87
        JMP core_return_463
        JMP core_if_end_471
core_else_470:
core_if_end_471:
        ADI R0,RZR,1
        SRM R0,[R8-8]
        ADI R0,RZR,8
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-24]
        JMP core_if_end_469
core_else_468:
        LRM R0,[R8-4]
        ADI R1,RZR,42
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_473
        ADI R0,RZR,8
        SRM R0,[R8-16]
        LRM R0,[R8-8]
        SNZ R0
        JMP core_else_475
        ADI R0,RZR,11
        SRM R0,[R8-16]
        JMP core_if_end_476
core_else_475:
core_if_end_476:
core_while_477:
        LRM R0,[R8-20]
        LRM R1,[R8-16]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_478
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,63
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        JMP core_while_477
core_while_end_478:
        ADI R0,RZR,1
        SRM R0,[R8-24]
        JMP core_if_end_474
core_else_473:
        LRM R0,[R8-24]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_479
        LRM R0,[R8-4]
        ADI R1,RZR,33
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,34
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,43
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,44
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,47
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,58
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,59
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,60
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,61
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,62
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,91
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,93
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_483
        LRM R0,[R8-4]
        ADI R1,RZR,124
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_483:
        SNZ R0
        JMP core_else_481
        ADI R0,RZR,87
        JMP core_return_463
        JMP core_if_end_482
core_else_481:
core_if_end_482:
        LRM R0,[R8-8]
        SNZ R0
        JMP core_bool_end_487
        LRM R0,[R8-20]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_487:
        SRZ R0
        JMP core_bool_end_486
        LRM R0,[R8-8]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_488
        LRM R0,[R8-20]
        ADI R1,RZR,8
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_488:
core_bool_end_486:
        SNZ R0
        JMP core_else_484
        ADI R0,RZR,87
        JMP core_return_463
        JMP core_if_end_485
core_else_484:
core_if_end_485:
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        JMP core_if_end_480
core_else_479:
core_if_end_480:
core_if_end_474:
core_if_end_469:
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_466
core_while_end_467:
        LRM R0,[R8-24]
        SNZ R0
        JMP core_bool_end_491
        LRM R0,[R8-8]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_491:
        SNZ R0
        JMP core_else_489
core_while_492:
        LRM R0,[R8-20]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_493
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,63
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,1
        SRM R0,[R8-20]
        JMP core_while_492
core_while_end_493:
        JMP core_if_end_490
core_else_489:
core_if_end_490:
        ADI R0,RZR,0
        JMP core_return_463
        ADI R0,RZR,0
core_return_463:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_matches:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-8
        ADI R0,RZR,0
        SRM R0,[R8-8]
core_while_495:
        LRM R0,[R8-8]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_496
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,63
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_499
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_499:
        SNZ R0
        JMP core_else_497
        ADI R0,RZR,0
        JMP core_return_494
        JMP core_if_end_498
core_else_497:
core_if_end_498:
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP core_while_495
core_while_end_496:
        ADI R0,RZR,1
        JMP core_return_494
        ADI R0,RZR,0
core_return_494:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_search:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-20
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_503
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SRZ R0
        JMP core_bool_end_504
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
core_bool_end_504:
core_bool_end_503:
        SNZ R0
        JMP core_else_501
        LRM R0,[R12+core_literal_253-8000h]
        PSH R0
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_505:
        LRM R0,[R8-4]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_506
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,63
        SRM R1,[R0]
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_505
core_while_end_506:
        ADI R0,RZR,0
        JMP core_return_500
        JMP core_if_end_502
core_else_501:
core_if_end_502:
        LRM R0,[R8+8]
        PSH R0
        Call core_path
        ADI SP,SP,4
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_507
        LRM R0,[R8-20]
        JMP core_return_500
        JMP core_if_end_508
core_else_507:
core_if_end_508:
        LRM R0,[R12+core_literal_151-8000h]
        ADI R0,R0,4
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-12]
core_while_509:
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_510
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_511
        LRM R0,[R8-16]
        ADI R0,R0,4
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-12]
        JMP core_if_end_512
core_else_511:
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
core_if_end_512:
        LRM R0,[R8-16]
        ADI R0,R0,4
        SRM R0,[R8-16]
        JMP core_while_509
core_while_end_510:
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R8-12]
        PSH R0
        Call core_make_pattern
        ADI SP,SP,8
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_513
        LRM R0,[R8-20]
        JMP core_return_500
        JMP core_if_end_514
core_else_513:
core_if_end_514:
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R12+core_literal_151-8000h]
        ADI R0,R0,4
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_515
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SRM R1,[R0]
        JMP core_if_end_516
core_else_515:
        LRM R0,[R8-8]
        ADI R0,R0,-4
        ADI R1,RZR,0
        SRM R1,[R0]
core_if_end_516:
        ADI R0,RZR,0
        PSH R0
        Call core_walk
        ADI SP,SP,4
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_517
        LRM R0,[R8-20]
        JMP core_return_500
        JMP core_if_end_518
core_else_517:
core_if_end_518:
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_521
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_521:
        SNZ R0
        JMP core_else_519
        ADI R0,RZR,3
        JMP core_return_500
        JMP core_if_end_520
core_else_519:
core_if_end_520:
        LRM R0,[R12+core_literal_253-8000h]
        PSH R0
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_500
        ADI R0,RZR,0
core_return_500:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_free_entry:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-28
        LRM R0,[R8+8]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_find
        ADI SP,SP,8
        SRM R0,[R8-12]
        LRM R0,[R8-12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_525
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_525:
        SNZ R0
        JMP core_else_523
        LRM R0,[R8-12]
        JMP core_return_522
        JMP core_if_end_524
core_else_523:
core_if_end_524:
        LRM R0,[R8+8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_526
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,112
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_527
core_else_526:
core_if_end_527:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-20]
        LRM R0,[R8+8]
        SRM R0,[R8-28]
        LRM R0,[R8-20]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-4]
core_while_528:
        LRM R0,[R8-28]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        ADI R1,RZR,4088
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_529
        LRM R0,[R8-28]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        SRM R0,[R8-28]
        LRM R0,[R8-4]
        ADI R0,R0,-1
        SRM R0,[R8-4]
        LRM R0,[R8-28]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_532
        LRM R0,[R8-28]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_532
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_532:
        SNZ R0
        JMP core_else_530
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_531
core_else_530:
core_if_end_531:
        JMP core_while_528
core_while_end_529:
        Call core_allocate
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_533
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_534
core_else_533:
core_if_end_534:
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        ADI R0,RZR,512
        PSH R0
        Call core_clear
        ADI SP,SP,8
        LRM R0,[R8-20]
        ADI R0,R0,28
        LRM R0,[R0]
        LRM R1,[R8-8]
        ADD R0,R0,R1
        ADI R0,R0,-2
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-20]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_535
        LRM R0,[R8-8]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        LRM R0,[R12+core_literal_85-8000h]
        LRM R1,[R8-24]
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_536
core_else_535:
core_if_end_536:
        LRM R0,[R8-28]
        PSH R0
        LRM R0,[R8-8]
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        Call core_fatflush
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_537
        LRM R0,[R12+core_literal_85-8000h]
        LRM R1,[R8-24]
        SRM R1,[R0]
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_538
core_else_537:
core_if_end_538:
        LRM R0,[R8-16]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_539
        ADI R0,RZR,0
        JMP core_return_522
        JMP core_if_end_540
core_else_539:
core_if_end_540:
        LRM R0,[R12+core_literal_106-8000h]
        JMP core_return_522
        ADI R0,RZR,0
core_return_522:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_chain_load:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R8-12]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-8]
        LRM R0,[R8+12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_544
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-8]
        ADI R1,RZR,512
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
core_bool_end_544:
        SNZ R0
        JMP core_else_542
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_541
        JMP core_if_end_543
core_else_542:
core_if_end_543:
        LRM R0,[R8+16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_545
        LRM R0,[R8+12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_547
        ADI R0,RZR,0
        JMP core_return_541
        JMP core_if_end_548
core_else_547:
core_if_end_548:
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_541
        JMP core_if_end_546
core_else_545:
core_if_end_546:
        LRM R0,[R8+16]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_551
        LRM R0,[R8+16]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_551:
        SNZ R0
        JMP core_else_549
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_541
        JMP core_if_end_550
core_else_549:
core_if_end_550:
        LRM R0,[R12+core_literal_552-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,2
        PSH R0
        Call core_clear
        ADI SP,SP,8
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_553:
        LRM R0,[R8+16]
        ADI R1,RZR,4088
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_554
        LRM R0,[R8+16]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_557
        LRM R0,[R8+16]
        PSH R0
        LRM R0,[R8-8]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_557
        LRM R0,[R8-4]
        LRM R1,[R8-8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_557
        LRM R0,[R12+core_literal_552-8000h]
        PSH R0
        LRM R0,[R8+16]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
core_bool_end_557:
        SNZ R0
        JMP core_else_555
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_541
        JMP core_if_end_556
core_else_555:
core_if_end_556:
        LRM R0,[R12+core_literal_552-8000h]
        PSH R0
        LRM R0,[R8+16]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R1,RZR,1
        SRM R1,[R0]
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8+16]
        SRM R1,[R0]
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        LRM R0,[R8+16]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        SRM R0,[R8+16]
        JMP core_while_553
core_while_end_554:
        LRM R0,[R8-4]
        ADI R1,RZR,512
        Call core_mul
        LRM R1,[R8+12]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_558
        LRM R0,[R12+core_literal_85-8000h]
        ADI R1,RZR,13
        SRM R1,[R0]
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_541
        JMP core_if_end_559
core_else_558:
core_if_end_559:
        LRM R0,[R8-4]
        JMP core_return_541
        ADI R0,RZR,0
core_return_541:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_free_chain:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        ADI R0,RZR,0
        SRM R0,[R8-4]
core_while_561:
        LRM R0,[R8-4]
        LRM R1,[R8+8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_562
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-4]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_561
core_while_end_562:
        ADI R0,RZR,0
        JMP core_return_560
        ADI R0,RZR,0
core_return_560:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_unlink_file:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-16
        LRM R0,[R8+8]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,17
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_564
        ADI R0,RZR,5
        JMP core_return_563
        JMP core_if_end_565
core_else_564:
core_if_end_565:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-8]
        LRM R0,[R8+8]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-12]
        LRM R0,[R8+8]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        PSH R0
        LRM R0,[R8+8]
        ADI R0,R0,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        PSH R0
        LRM R0,[R12+core_literal_566-8000h]
        PSH R0
        Call core_chain_load
        ADI SP,SP,12
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_567
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_563
        JMP core_if_end_568
core_else_567:
core_if_end_568:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-12]
        ADD R0,R0,R1
        ADI R1,RZR,229
        SRM R1,[R0]
        LRM R0,[R8-8]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_569
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-16]
        JMP core_return_563
        JMP core_if_end_570
core_else_569:
core_if_end_570:
        LRM R0,[R12+core_literal_566-8000h]
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_free_chain
        ADI SP,SP,8
        Call core_fatflush
        JMP core_return_563
        ADI R0,RZR,0
core_return_563:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_delete_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-48
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-36]
        ADI R0,RZR,0
        SRM R0,[R8-32]
        ADI R0,RZR,0
        SRM R0,[R8-24]
core_while_572:
        LRM R0,[R8-24]
        LRM R1,[R8-8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_573
        LRM R0,[R12+core_literal_243-8000h]
        PSH R0
        LRM R0,[R8-24]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-44]
        LRM R0,[R8-44]
        PSH R0
        LRM R0,[R12+core_literal_577-8000h]
        PSH R0
        Call core_equal
        ADI SP,SP,8
        SNZ R0
        JMP core_else_574
        ADI R0,RZR,1
        SRM R0,[R8-32]
        JMP core_if_end_575
core_else_574:
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_580
        LRM R0,[R8-44]
        LRM R0,[R0]
        ADI R1,RZR,47
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_580:
        SNZ R0
        JMP core_else_578
        LRM R0,[R8-44]
        SRM R0,[R8-36]
        JMP core_if_end_579
core_else_578:
        ADI R0,RZR,87
        JMP core_return_571
core_if_end_579:
core_if_end_575:
        LRM R0,[R8-24]
        ADI R0,R0,1
        SRM R0,[R8-24]
        JMP core_while_572
core_while_end_573:
        LRM R0,[R8-8]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_583
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_583:
        SNZ R0
        JMP core_else_581
        ADI R0,RZR,87
        JMP core_return_571
        JMP core_if_end_582
core_else_581:
core_if_end_582:
        LRM R0,[R8-36]
        PSH R0
        Call core_search
        ADI SP,SP,4
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_584
        LRM R0,[R8-40]
        JMP core_return_571
        JMP core_if_end_585
core_else_584:
core_if_end_585:
        LRM R0,[R12+core_literal_253-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        ADI R0,RZR,1
        SRM R0,[R8-4]
        ADI R0,RZR,0
        SRM R0,[R8-24]
core_while_586:
        LRM R0,[R8-24]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_587
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        LRM R0,[R8-24]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADI R1,RZR,63
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_588
        ADI R0,RZR,0
        SRM R0,[R8-4]
        JMP core_if_end_589
core_else_588:
core_if_end_589:
        LRM R0,[R8-24]
        ADI R0,R0,1
        SRM R0,[R8-24]
        JMP core_while_586
core_while_end_587:
        LRM R0,[R8-4]
        SNZ R0
        JMP core_bool_end_592
        LRM R0,[R8-32]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_592:
        SNZ R0
        JMP core_else_590
        LRM R0,[R12+core_literal_594-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_answer
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_595
        ADI R0,RZR,0
        JMP core_return_571
        JMP core_if_end_596
core_else_595:
core_if_end_596:
        JMP core_if_end_591
core_else_590:
core_if_end_591:
        ADI R0,RZR,0
        SRM R0,[R8-28]
        ADI R0,RZR,0
        SRM R0,[R8-20]
core_while_597:
        LRM R0,[R8-28]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_598
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_601
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_601:
        SNZ R0
        JMP core_else_599
        JMP core_while_end_598
        JMP core_if_end_600
core_else_599:
core_if_end_600:
        LRM R0,[R8-16]
        LRM R0,[R0]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_604
        LRM R0,[R8-16]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,30
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_604
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        Call core_matches
        ADI SP,SP,8
core_bool_end_604:
        SNZ R0
        JMP core_else_602
        ADI R0,RZR,1
        SRM R0,[R8-20]
        ADI R0,RZR,1
        SRM R0,[R8-48]
        LRM R0,[R8-32]
        SNZ R0
        JMP core_else_605
        LRM R0,[R8-16]
        PSH R0
        Call core_print_name
        ADI SP,SP,4
        LRM R0,[R12+core_literal_608-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_answer
        SRM R0,[R8-48]
        JMP core_if_end_606
core_else_605:
core_if_end_606:
        LRM R0,[R8-48]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_609
        ADI R0,RZR,0
        JMP core_return_571
        JMP core_if_end_610
core_else_609:
core_if_end_610:
        LRM R0,[R8-48]
        SNZ R0
        JMP core_else_611
        LRM R0,[R8-16]
        PSH R0
        Call core_unlink_file
        ADI SP,SP,4
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_613
        LRM R0,[R8-40]
        JMP core_return_571
        JMP core_if_end_614
core_else_613:
core_if_end_614:
        JMP core_if_end_612
core_else_611:
core_if_end_612:
        JMP core_if_end_603
core_else_602:
core_if_end_603:
        LRM R0,[R8-28]
        ADI R0,R0,1
        SRM R0,[R8-28]
        JMP core_while_597
core_while_end_598:
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_615
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_571
        JMP core_if_end_616
core_else_615:
core_if_end_616:
        LRM R0,[R8-20]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_617
        ADI R0,RZR,2
        JMP core_return_571
        JMP core_if_end_618
core_else_617:
core_if_end_618:
        ADI R0,RZR,0
        JMP core_return_571
        ADI R0,RZR,0
core_return_571:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_mapped_name:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-8
        ADI R0,RZR,0
        SRM R0,[R8-8]
core_while_620:
        LRM R0,[R8-8]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_621
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,63
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_622
        LRM R0,[R8+12]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-4]
        JMP core_if_end_623
core_else_622:
core_if_end_623:
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP core_while_620
core_while_end_621:
        LRM R0,[R12+core_literal_130-8000h]
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_626
        LRM R0,[R12+core_literal_130-8000h]
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_626:
        SNZ R0
        JMP core_else_624
        ADI R0,RZR,87
        JMP core_return_619
        JMP core_if_end_625
core_else_624:
core_if_end_625:
        ADI R0,RZR,0
        JMP core_return_619
        ADI R0,RZR,0
core_return_619:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_copy_one:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-64
        LRM R0,[R8+24]
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_628
        LRM R0,[R8-64]
        JMP core_return_627
        JMP core_if_end_629
core_else_628:
core_if_end_629:
        LRM R0,[R8+20]
        PSH R0
        LRM R0,[R8+16]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-28]
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_630
        ADI R0,RZR,13
        JMP core_return_627
        JMP core_if_end_631
core_else_630:
core_if_end_631:
        LRM R0,[R12+core_literal_632-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,32
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R12+core_literal_633-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-52]
        LRM R0,[R8-28]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-56]
        LRM R0,[R12+core_literal_632-8000h]
        ADI R0,R0,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        SRM R0,[R8-48]
        LRM R0,[R12+core_literal_632-8000h]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        PSH R0
        LRM R0,[R8-48]
        PSH R0
        LRM R0,[R12+core_literal_636-8000h]
        PSH R0
        Call core_chain_load
        ADI SP,SP,12
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_634
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_635
core_else_634:
core_if_end_635:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-60]
        LRM R0,[R8+12]
        PSH R0
        Call core_destination_path
        ADI SP,SP,4
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_637
        LRM R0,[R8-64]
        JMP core_return_627
        JMP core_if_end_638
core_else_637:
core_if_end_638:
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        SRM R0,[R8-28]
        LRM R0,[R12+core_literal_205-8000h]
        LRM R0,[R0]
        SRM R0,[R8-44]
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SRZ R0
        JMP core_bool_end_641
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_642
        LRM R0,[R8-28]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
core_bool_end_642:
core_bool_end_641:
        SNZ R0
        JMP core_else_639
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        SRM R0,[R8-44]
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R12+core_literal_633-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-44]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_find
        ADI SP,SP,8
        SRM R0,[R8-28]
        JMP core_if_end_640
core_else_639:
core_if_end_640:
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_643
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_644
core_else_643:
core_if_end_644:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        LRM R0,[R0]
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-40]
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_645
        LRM R0,[R8+24]
        LRM R1,[R8-20]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_649
        LRM R0,[R8-52]
        PSH R0
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_649
        LRM R0,[R8-56]
        PSH R0
        LRM R0,[R8-28]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_649:
        SNZ R0
        JMP core_else_647
        ADI R0,RZR,82
        JMP core_return_627
        JMP core_if_end_648
core_else_647:
core_if_end_648:
        LRM R0,[R8-28]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,17
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_650
        ADI R0,RZR,5
        JMP core_return_627
        JMP core_if_end_651
core_else_650:
core_if_end_651:
        LRM R0,[R8+8]
        SNZ R0
        JMP core_else_652
        LRM R0,[R12+core_literal_655-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        LRM R0,[R8-28]
        PSH R0
        Call core_print_name
        ADI SP,SP,4
        LRM R0,[R12+core_literal_657-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_answer
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_658
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_627
        JMP core_if_end_659
core_else_658:
core_if_end_659:
        JMP core_if_end_653
core_else_652:
core_if_end_653:
        LRM R0,[R8-28]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        PSH R0
        LRM R0,[R8-28]
        ADI R0,R0,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        PSH R0
        LRM R0,[R12+core_literal_566-8000h]
        PSH R0
        Call core_chain_load
        ADI SP,SP,12
        SRM R0,[R8-40]
        LRM R0,[R8-40]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_660
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_661
core_else_660:
core_if_end_661:
        JMP core_if_end_646
core_else_645:
        LRM R0,[R8-44]
        PSH R0
        Call core_free_entry
        ADI SP,SP,4
        SRM R0,[R8-28]
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_662
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_663
core_else_662:
core_if_end_663:
core_if_end_646:
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R8-28]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-16]
        LRM R0,[R8-48]
        ADI R0,R0,511
        ADI R1,RZR,512
        Call core_div
        SRM R0,[R8-8]
        ADI R0,RZR,0
        SRM R0,[R8-36]
core_while_665:
        LRM R0,[R8-36]
        LRM R1,[R8-8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_666
        Call core_allocate
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_667
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-36]
        PSH R0
        Call core_free_chain
        ADI SP,SP,8
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_668
core_else_667:
core_if_end_668:
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_670
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADI R0,R0,-1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_fatset
        ADI SP,SP,8
        JMP core_if_end_671
core_else_670:
core_if_end_671:
        LRM R0,[R8-36]
        ADI R0,R0,1
        SRM R0,[R8-36]
        JMP core_while_665
core_while_end_666:
        Call core_fatflush
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_672
        LRM R0,[R8-64]
        JMP core_return_627
        JMP core_if_end_673
core_else_672:
core_if_end_673:
        ADI R0,RZR,0
        SRM R0,[R8-36]
core_while_674:
        LRM R0,[R8-36]
        LRM R1,[R8-8]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_675
        LRM R0,[R8-60]
        ADI R0,R0,28
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_636-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R0,R0,-2
        PSH R0
        LRM R0,[R12+core_literal_676-8000h]
        PSH R0
        LRM R0,[R8+24]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_677
        LRM R0,[R8-24]
        ADI R0,R0,28
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R0,R0,-2
        PSH R0
        LRM R0,[R12+core_literal_676-8000h]
        PSH R0
        LRM R0,[R8-20]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-64]
        JMP core_if_end_678
core_else_677:
core_if_end_678:
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_679
        LRM R0,[R8-64]
        SRM R0,[R8-32]
        LRM R0,[R8-20]
        PSH R0
        Call core_select
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_681
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-8]
        PSH R0
        Call core_free_chain
        ADI SP,SP,8
        Call core_fatflush
        JMP core_if_end_682
core_else_681:
core_if_end_682:
        LRM R0,[R8-32]
        JMP core_return_627
        JMP core_if_end_680
core_else_679:
core_if_end_680:
        LRM R0,[R8-36]
        ADI R0,R0,1
        SRM R0,[R8-36]
        JMP core_while_674
core_while_end_675:
        LRM R0,[R8-12]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_683
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_627
        JMP core_if_end_684
core_else_683:
core_if_end_684:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-16]
        ADD R0,R0,R1
        SRM R0,[R8-28]
        LRM R0,[R8-28]
        PSH R0
        LRM R0,[R12+core_literal_632-8000h]
        PSH R0
        ADI R0,RZR,32
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-28]
        PSH R0
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-28]
        ADI R0,R0,44
        ADI R1,RZR,32
        SRM R1,[R0]
        ADI R0,RZR,0
        SRM R0,[R8-4]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_685
        LRM R0,[R12+core_literal_669-8000h]
        LRM R0,[R0]
        SRM R0,[R8-4]
        JMP core_if_end_686
core_else_685:
core_if_end_686:
        LRM R0,[R8-28]
        ADI R0,R0,104
        PSH R0
        LRM R0,[R8-4]
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R8-28]
        ADI R0,R0,112
        PSH R0
        LRM R0,[R8-48]
        PSH R0
        Call core_set32
        ADI SP,SP,8
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-20]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_687
        LRM R0,[R12+core_literal_669-8000h]
        PSH R0
        LRM R0,[R8-8]
        PSH R0
        Call core_free_chain
        ADI SP,SP,8
        Call core_fatflush
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-64]
        JMP core_return_627
        JMP core_if_end_688
core_else_687:
core_if_end_688:
        LRM R0,[R12+core_literal_566-8000h]
        PSH R0
        LRM R0,[R8-40]
        PSH R0
        Call core_free_chain
        ADI SP,SP,8
        Call core_fatflush
        JMP core_return_627
        ADI R0,RZR,0
core_return_627:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_destination_path:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-28
        LRM R0,[R8+8]
        PSH R0
        Call core_path
        ADI SP,SP,4
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_690
        LRM R0,[R8-24]
        JMP core_return_689
        JMP core_if_end_691
core_else_690:
core_if_end_691:
        LRM R0,[R12+core_literal_151-8000h]
        ADI R0,R0,4
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        SRM R0,[R8-12]
        ADI R0,RZR,0
        SRM R0,[R8-16]
        ADI R0,RZR,0
        SRM R0,[R8-28]
core_while_692:
        LRM R0,[R8-20]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_693
        LRM R0,[R8-20]
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,92
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_694
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-12]
        ADI R0,RZR,0
        SRM R0,[R8-16]
        ADI R0,RZR,0
        SRM R0,[R8-28]
        JMP core_if_end_695
core_else_694:
        LRM R0,[R8-16]
        ADI R0,R0,1
        SRM R0,[R8-16]
        LRM R0,[R8-4]
        ADI R1,RZR,42
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_698
        LRM R0,[R8-4]
        ADI R1,RZR,63
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_698:
        SNZ R0
        JMP core_else_696
        ADI R0,RZR,1
        SRM R0,[R8-28]
        JMP core_if_end_697
core_else_696:
core_if_end_697:
core_if_end_695:
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-20]
        JMP core_while_692
core_while_end_693:
        LRM R0,[R8-28]
        SNZ R0
        JMP core_else_699
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-16]
        PSH R0
        Call core_make_pattern
        ADI SP,SP,8
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_701
        LRM R0,[R8-24]
        JMP core_return_689
        JMP core_if_end_702
core_else_701:
core_if_end_702:
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R12+core_literal_633-8000h]
        PSH R0
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        Call core_mapped_name
        ADI SP,SP,8
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_703
        LRM R0,[R8-24]
        JMP core_return_689
        JMP core_if_end_704
core_else_703:
core_if_end_704:
        LRM R0,[R8-12]
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-8]
core_while_705:
        LRM R0,[R8-8]
        ADI R1,RZR,11
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_706
        LRM R0,[R8-8]
        ADI R1,RZR,8
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_709
        LRM R0,[R12+core_literal_130-8000h]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_709:
        SNZ R0
        JMP core_else_707
        LRM R0,[R8-20]
        ADI R1,RZR,46
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-20]
        JMP core_if_end_708
core_else_707:
core_if_end_708:
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R8-8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,32
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_710
        LRM R0,[R8-20]
        LRM R1,[R8-4]
        SRM R1,[R0]
        LRM R0,[R8-20]
        ADI R0,R0,4
        SRM R0,[R8-20]
        JMP core_if_end_711
core_else_710:
core_if_end_711:
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP core_while_705
core_while_end_706:
        LRM R0,[R8-20]
        ADI R1,RZR,0
        SRM R1,[R0]
        JMP core_if_end_700
core_else_699:
core_if_end_700:
        ADI R0,RZR,1
        PSH R0
        Call core_walk
        ADI SP,SP,4
        JMP core_return_689
        ADI R0,RZR,0
core_return_689:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_copy_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-48
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_715
        LRM R0,[R8-8]
        ADI R1,RZR,3
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
core_bool_end_715:
        SNZ R0
        JMP core_else_713
        ADI R0,RZR,87
        JMP core_return_712
        JMP core_if_end_714
core_else_713:
core_if_end_714:
        ADI R0,RZR,1
        SRM R0,[R8-40]
        LRM R0,[R8-8]
        ADI R1,RZR,3
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_716
        LRM R0,[R12+core_literal_243-8000h]
        ADI R0,R0,8
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_721-8000h]
        PSH R0
        Call core_equal
        ADI SP,SP,8
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_718
        ADI R0,RZR,87
        JMP core_return_712
        JMP core_if_end_719
core_else_718:
core_if_end_719:
        ADI R0,RZR,0
        SRM R0,[R8-40]
        JMP core_if_end_717
core_else_716:
core_if_end_717:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        SRM R0,[R8-44]
        LRM R0,[R12+core_literal_243-8000h]
        ADI R0,R0,4
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R8-44]
        PSH R0
        Call core_search
        ADI SP,SP,4
        SRM R0,[R8-48]
        LRM R0,[R8-48]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_722
        LRM R0,[R8-48]
        JMP core_return_712
        JMP core_if_end_723
core_else_722:
core_if_end_723:
        LRM R0,[R12+core_literal_253-8000h]
        LRM R0,[R0]
        SRM R0,[R8-16]
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        SRM R0,[R8-20]
        ADI R0,RZR,0
        SRM R0,[R8-36]
        ADI R0,RZR,0
        SRM R0,[R8-32]
core_while_724:
        LRM R0,[R8-32]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_725
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R8-32]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_728
        LRM R0,[R8-24]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_728:
        SNZ R0
        JMP core_else_726
        JMP core_while_end_725
        JMP core_if_end_727
core_else_726:
core_if_end_727:
        LRM R0,[R8-24]
        LRM R0,[R0]
        ADI R1,RZR,229
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_731
        LRM R0,[R8-24]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,30
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_731
        LRM R0,[R8-24]
        PSH R0
        LRM R0,[R12+core_literal_266-8000h]
        PSH R0
        Call core_matches
        ADI SP,SP,8
core_bool_end_731:
        SNZ R0
        JMP core_else_729
        LRM R0,[R12+core_literal_732-8000h]
        PSH R0
        LRM R0,[R8-36]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R1,[R8-32]
        SRM R1,[R0]
        LRM R0,[R8-36]
        ADI R0,R0,1
        SRM R0,[R8-36]
        JMP core_if_end_730
core_else_729:
core_if_end_730:
        LRM R0,[R8-32]
        ADI R0,R0,1
        SRM R0,[R8-32]
        JMP core_while_724
core_while_end_725:
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_733
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_712
        JMP core_if_end_734
core_else_733:
core_if_end_734:
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_735
        ADI R0,RZR,2
        JMP core_return_712
        JMP core_if_end_736
core_else_735:
core_if_end_736:
        LRM R0,[R8-36]
        ADI R1,RZR,1
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_737
        LRM R0,[R8-12]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-48]
        LRM R0,[R8-48]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_741
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        ADD R1,R0,RZR
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_742
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_742:
core_bool_end_741:
        SNZ R0
        JMP core_else_739
        ADI R0,RZR,87
        JMP core_return_712
        JMP core_if_end_740
core_else_739:
core_if_end_740:
        JMP core_if_end_738
core_else_737:
core_if_end_738:
        ADI R0,RZR,0
        SRM R0,[R8-4]
        ADI R0,RZR,0
        SRM R0,[R8-28]
core_while_743:
        LRM R0,[R8-28]
        LRM R1,[R8-36]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_744
        LRM R0,[R8-20]
        PSH R0
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R12+core_literal_732-8000h]
        PSH R0
        LRM R0,[R8-28]
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-40]
        PSH R0
        Call core_copy_one
        ADI SP,SP,20
        SRM R0,[R8-48]
        LRM R0,[R8-48]
        ADI R1,RZR,0
        SUB R1,R1,R0
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_745
        LRM R0,[R8-48]
        JMP core_return_712
        JMP core_if_end_746
core_else_745:
core_if_end_746:
        LRM R0,[R8-48]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_747
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_if_end_748
core_else_747:
core_if_end_748:
        LRM R0,[R8-28]
        ADI R0,R0,1
        SRM R0,[R8-28]
        JMP core_while_743
core_while_end_744:
        LRM R0,[R12+core_literal_750-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        LRM R0,[R8-4]
        PSH R0
        Call core_decimal
        ADI SP,SP,4
        LRM R0,[R12+core_literal_752-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_newline
        ADI R0,RZR,0
        JMP core_return_712
        ADI R0,RZR,0
core_return_712:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_drive_value:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-4
        LRM R0,[R8+8]
        LRM R0,[R0]
        ADI R0,R0,-48
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_756
        LRM R0,[R8-4]
        ADI R1,RZR,4
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
core_bool_end_756:
        SNZ R0
        JMP core_else_754
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_753
        JMP core_if_end_755
core_else_754:
core_if_end_755:
        LRM R0,[R8+8]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_759
        LRM R0,[R8+8]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,58
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_760
        LRM R0,[R8+8]
        ADI R0,R0,8
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_760:
core_bool_end_759:
        SNZ R0
        JMP core_else_757
        LRM R0,[R8-4]
        JMP core_return_753
        JMP core_if_end_758
core_else_757:
core_if_end_758:
        ADI R0,RZR,1
        SUB R0,RZR,R0
        JMP core_return_753
        ADI R0,RZR,0
core_return_753:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_drive_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_762
        Call core_prompt_drive
        ADI R0,RZR,58
        PSH R0
        Call core_putc
        ADI SP,SP,4
        Call core_newline
        ADI R0,RZR,0
        JMP core_return_761
        JMP core_if_end_763
core_else_762:
core_if_end_763:
        LRM R0,[R8-4]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_764
        ADI R0,RZR,87
        JMP core_return_761
        JMP core_if_end_765
core_else_764:
core_if_end_765:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        PSH R0
        Call core_drive_number
        ADI SP,SP,4
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_766
        ADI R0,RZR,15
        JMP core_return_761
        JMP core_if_end_767
core_else_766:
core_if_end_767:
        LRM R0,[R8-8]
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRM R0,[R8-12]
        LRM R0,[R8-12]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_768
        LRM R0,[R12+core_literal_39-8000h]
        LRM R1,[R8-8]
        SRM R1,[R0]
        JMP core_if_end_769
core_else_768:
core_if_end_769:
        LRM R0,[R8-12]
        JMP core_return_761
        ADI R0,RZR,0
core_return_761:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
; Numeric DRIVE argument, 0..3. drive_value also accepts SYS's colon suffix.
; Manual assembly synchronized with core.py while the generator is absent.
core_drive_number:
        LRM R1,[SP+4]
        LRM R0,[R1]
        LRM R2,[R1+4]
        SRZ R2
        JMP core_drive_number_invalid
        ADI R0,R0,-48
        Const R2,80000000h
        ANL R1,R0,R2
        SRZ R1
        JMP core_drive_number_invalid
        ADI R1,R0,-4
        ANL R1,R1,R2
        SNZ R1
        JMP core_drive_number_invalid
        JMP return_sub
core_drive_number_invalid:
        ADI R0,RZR,-1
        JMP return_sub
core_move_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-64
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_771
        ADI R0,RZR,87
        JMP core_return_770
        JMP core_if_end_772
core_else_771:
core_if_end_772:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        SRM R0,[R8-56]
        LRM R0,[R12+core_literal_243-8000h]
        ADI R0,R0,4
        LRM R0,[R0]
        SRM R0,[R8-16]
        LRM R0,[R8-56]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_773
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_774
core_else_773:
core_if_end_774:
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SNZ R0
        JMP core_else_775
        ADI R0,RZR,5
        JMP core_return_770
        JMP core_if_end_776
core_else_775:
core_if_end_776:
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,1
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_777
        ADI R0,RZR,5
        JMP core_return_770
        JMP core_if_end_778
core_else_777:
core_if_end_778:
        ; Renaming a current directory or its ancestor would stale its path.
        LRM R0,[R8-36]
        LRM R0,[R0+44]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_move_cwd_safe
        Call core_cwd_busy
        SNZ R0
        JMP core_move_cwd_safe
        ADI R0,RZR,5
        JMP core_return_770
core_move_cwd_safe:
        LRM R0,[R12+core_literal_779-8000h]
        PSH R0
        LRM R0,[R8-36]
        PSH R0
        ADI R0,RZR,32
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        SRM R0,[R8-60]
        LRM R0,[R12+core_literal_205-8000h]
        LRM R0,[R0]
        SRM R0,[R8-44]
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        SRM R0,[R8-12]
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-48]
        LRM R0,[R8-36]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-52]
        LRM R0,[R12+core_literal_780-8000h]
        PSH R0
        ADI R0,RZR,48
        LRM R1,[R8-60]
        ADD R0,R0,R1
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_780-8000h]
        ADI R0,R0,4
        ADI R1,RZR,58
        SRM R1,[R0]
        LRM R0,[R12+core_literal_780-8000h]
        ADI R0,R0,8
        PSH R0
        LRM R0,[R12+core_literal_151-8000h]
        PSH R0
        Call core_copytext
        ADI SP,SP,8
        LRM R0,[R8-16]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_781
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_782
core_else_781:
core_if_end_782:
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        SRM R0,[R8-36]
        LRM R0,[R12+core_literal_205-8000h]
        LRM R0,[R0]
        SRM R0,[R8-32]
        LRM R0,[R12+core_literal_203-8000h]
        LRM R0,[R0]
        SRZ R0
        JMP core_bool_end_785
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_786
        LRM R0,[R8-36]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
core_bool_end_786:
core_bool_end_785:
        SNZ R0
        JMP core_else_783
        LRM R0,[R12+core_literal_206-8000h]
        LRM R0,[R0]
        SRM R0,[R8-32]
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        LRM R0,[R12+core_literal_779-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-32]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_find
        ADI SP,SP,8
        SRM R0,[R8-36]
        JMP core_if_end_784
core_else_783:
core_if_end_784:
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_787
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_770
        JMP core_if_end_788
core_else_787:
core_if_end_788:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        LRM R0,[R0]
        SRM R0,[R8-28]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_789
        LRM R0,[R8-60]
        LRM R1,[R8-28]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_793
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        LRM R1,[R8-48]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_793
        LRM R0,[R8-36]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        LRM R1,[R8-52]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_793:
        SNZ R0
        JMP core_else_791
        ADI R0,RZR,0
        JMP core_return_770
        JMP core_if_end_792
core_else_791:
core_if_end_792:
        ADI R0,RZR,183
        JMP core_return_770
        JMP core_if_end_790
core_else_789:
core_if_end_790:
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        LRM R0,[R12+core_literal_130-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-60]
        LRM R1,[R8-28]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_794
        LRM R0,[R12+core_literal_779-8000h]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_796
        ADI R0,RZR,5
        JMP core_return_770
        JMP core_if_end_797
core_else_796:
core_if_end_797:
        LRM R0,[R8-60]
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_798
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_799
core_else_798:
core_if_end_799:
        ADI R0,RZR,0
        SRM R0,[R8-40]
core_while_800:
        LRM R0,[R8-40]
        LRM R1,[R12+core_literal_29-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_801
        LRM R0,[R8-44]
        PSH R0
        LRM R0,[R8-40]
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_804
        LRM R0,[R8-36]
        LRM R0,[R0]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_804:
        SNZ R0
        JMP core_else_802
        ADI R0,RZR,2
        JMP core_return_770
        JMP core_if_end_803
core_else_802:
core_if_end_803:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        LRM R1,[R8-48]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_807
        LRM R0,[R8-36]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        LRM R1,[R8-52]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
core_bool_end_807:
        SNZ R0
        JMP core_else_805
        JMP core_while_end_801
        JMP core_if_end_806
core_else_805:
core_if_end_806:
        LRM R0,[R8-40]
        ADI R0,R0,1
        SRM R0,[R8-40]
        JMP core_while_800
core_while_end_801:
        LRM R0,[R8-60]
        PSH R0
        LRM R0,[R8-44]
        PSH R0
        LRM R0,[R8-40]
        PSH R0
        LRM R0,[R8-16]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_copy_one
        ADI SP,SP,20
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_808
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_809
core_else_808:
core_if_end_809:
        LRM R0,[R12+core_literal_780-8000h]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_resolve
        ADI SP,SP,8
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_810
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_811
core_else_810:
core_if_end_811:
        LRM R0,[R12+core_literal_204-8000h]
        LRM R0,[R0]
        PSH R0
        Call core_unlink_file
        ADI SP,SP,4
        JMP core_return_770
        JMP core_if_end_795
core_else_794:
core_if_end_795:
        LRM R0,[R12+core_literal_779-8000h]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_812
        LRM R0,[R8-32]
        SRM R0,[R8-4]
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        ADI R0,R0,32
        LRM R0,[R0]
        SRM R0,[R8-8]
core_while_814:
        LRM R0,[R8-4]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_815
        LRM R0,[R8-4]
        LRM R1,[R8-12]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_816
        ADI R0,RZR,5
        JMP core_return_770
        JMP core_if_end_817
core_else_816:
core_if_end_817:
        LRM R0,[R8-8]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_818
        ADI R0,RZR,13
        JMP core_return_770
        JMP core_if_end_819
core_else_818:
core_if_end_819:
        LRM R0,[R8-4]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_822
        LRM R0,[R8-36]
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_822
        LRM R0,[R8-36]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,46
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_822:
        SNZ R0
        JMP core_else_820
        ADI R0,RZR,13
        JMP core_return_770
        JMP core_if_end_821
core_else_820:
core_if_end_821:
        LRM R0,[R8-36]
        ADI R0,R0,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        SRM R0,[R8-4]
        LRM R0,[R8-8]
        ADI R0,R0,-1
        SRM R0,[R8-8]
        JMP core_while_814
core_while_end_815:
        JMP core_if_end_813
core_else_812:
core_if_end_813:
        LRM R0,[R8-44]
        LRM R1,[R8-32]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_823
        LRM R0,[R8-48]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_825
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_826
core_else_825:
core_if_end_826:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-52]
        ADD R0,R0,R1
        PSH R0
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-48]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-60]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        JMP core_return_770
        JMP core_if_end_824
core_else_823:
core_if_end_824:
        LRM R0,[R8-32]
        PSH R0
        Call core_free_entry
        ADI SP,SP,4
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_827
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_770
        JMP core_if_end_828
core_else_827:
core_if_end_828:
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        SRM R0,[R8-20]
        LRM R0,[R8-36]
        LRM R1,[R12+core_literal_106-8000h]
        SUB R0,R0,R1
        SRM R0,[R8-24]
        LRM R0,[R8-36]
        PSH R0
        LRM R0,[R12+core_literal_779-8000h]
        PSH R0
        ADI R0,RZR,32
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-36]
        PSH R0
        LRM R0,[R12+core_literal_664-8000h]
        PSH R0
        ADI R0,RZR,11
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-20]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-60]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_829
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_830
core_else_829:
core_if_end_830:
        LRM R0,[R12+core_literal_779-8000h]
        ADI R0,R0,44
        LRM R0,[R0]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_831
        LRM R0,[R8-12]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_dir_entry
        ADI SP,SP,8
        SRM R0,[R8-36]
        LRM R0,[R8-36]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_833
        LRM R0,[R12+core_literal_85-8000h]
        LRM R0,[R0]
        JMP core_return_770
        JMP core_if_end_834
core_else_833:
core_if_end_834:
        LRM R0,[R8-36]
        ADI R0,R0,104
        PSH R0
        LRM R0,[R8-32]
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R12+core_literal_41-8000h]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-60]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_835
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_836
core_else_835:
core_if_end_836:
        JMP core_if_end_832
core_else_831:
core_if_end_832:
        LRM R0,[R8-48]
        PSH R0
        Call core_read_dir_sector
        ADI SP,SP,4
        SRM R0,[R8-64]
        LRM R0,[R8-64]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_837
        LRM R0,[R8-64]
        JMP core_return_770
        JMP core_if_end_838
core_else_837:
core_if_end_838:
        LRM R0,[R12+core_literal_106-8000h]
        LRM R1,[R8-52]
        ADD R0,R0,R1
        ADI R1,RZR,229
        SRM R1,[R0]
        LRM R0,[R8-48]
        PSH R0
        LRM R0,[R12+core_literal_106-8000h]
        PSH R0
        LRM R0,[R8-60]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        JMP core_return_770
        ADI R0,RZR,0
core_return_770:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_sysfat:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-8
        LRM R0,[R12+core_literal_840-8000h]
        PSH R0
        LRM R0,[R8+8]
        PSH R0
        LRM R0,[R8+8]
        ADI R1,RZR,2
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-4]
        LRM R0,[R8-4]
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-4]
        ADI R0,R0,4
        LRM R0,[R0]
        ADI R1,RZR,256
        Call core_mul
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        SRM R0,[R8-8]
        LRM R0,[R8+8]
        ADI R1,RZR,1
        ANL R0,R0,R1
        SNZ R0
        JMP core_else_841
        LRM R0,[R8-8]
        ADI R1,RZR,4
        ADI R1,RZR,16
        Call core_div
        JMP core_return_839
        JMP core_if_end_842
core_else_841:
core_if_end_842:
        LRM R0,[R8-8]
        ADI R1,RZR,4095
        ANL R0,R0,R1
        JMP core_return_839
        ADI R0,RZR,0
core_return_839:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
core_sys_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-28
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_844
        ADI R0,RZR,87
        JMP core_return_843
        JMP core_if_end_845
core_else_844:
core_if_end_845:
        LRM R0,[R12+core_literal_243-8000h]
        LRM R0,[R0]
        PSH R0
        Call core_drive_value
        ADI SP,SP,4
        SRM R0,[R8-28]
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_846
        ADI R0,RZR,15
        JMP core_return_843
        JMP core_if_end_847
core_else_846:
core_if_end_847:
        LRM R0,[R8-28]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_848
        ADI R0,RZR,5
        JMP core_return_843
        JMP core_if_end_849
core_else_848:
core_if_end_849:
        LRM R0,[R8-28]
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_850
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_851
core_else_850:
core_if_end_851:
        LRM R0,[R12+core_literal_46-8000h]
        LRM R0,[R0]
        SRM R0,[R8-20]
        LRM R0,[R8-20]
        ADI R0,R0,8
        LRM R0,[R0]
        SRM R0,[R8-16]
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_bool_end_854
        LRM R0,[R8-16]
        ADI R1,RZR,162
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_854:
        SNZ R0
        JMP core_else_852
        ADI R0,RZR,13
        JMP core_return_843
        JMP core_if_end_853
core_else_852:
core_if_end_853:
        ADI R0,RZR,0
        PSH R0
        LRM R0,[R12+core_literal_855-8000h]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_856
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_857
core_else_856:
core_if_end_857:
        LRM R0,[R12+core_literal_855-8000h]
        PSH R0
        Call core_le32
        ADI SP,SP,4
        LRM R1,[R12+core_literal_861-8000h]
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SRZ R0
        JMP core_bool_end_860
        LRM R0,[R12+core_literal_855-8000h]
        ADI R0,R0,16
        PSH R0
        Call core_le32
        ADI SP,SP,4
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
core_bool_end_860:
        SNZ R0
        JMP core_else_858
        ADI R0,RZR,13
        JMP core_return_843
        JMP core_if_end_859
core_else_858:
core_if_end_859:
        LRM R0,[R8-16]
        PSH R0
        LRM R0,[R12+core_literal_862-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_863
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_864
core_else_863:
core_if_end_864:
        LRM R0,[R8-16]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_865
        LRM R0,[R8-20]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,-162
        SRM R0,[R8-8]
        LRM R0,[R8-8]
        ADI R1,RZR,1
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_867
        ADI R0,RZR,112
        JMP core_return_843
        JMP core_if_end_868
core_else_867:
core_if_end_868:
        LRM R0,[R12+core_literal_840-8000h]
        PSH R0
        LRM R0,[R12+core_literal_75-8000h]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,16
        LRM R0,[R0]
        ADI R1,RZR,512
        Call core_mul
        PSH R0
        Call core_memcopy
        ADI SP,SP,12
        LRM R0,[R8-8]
        ADI R0,R0,2
        SRM R0,[R8-4]
core_while_869:
        LRM R0,[R8-4]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,32
        LRM R0,[R0]
        ADI R0,R0,2
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_870
        LRM R0,[R8-4]
        PSH R0
        Call core_sysfat
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_871
        ADI R0,RZR,112
        JMP core_return_843
        JMP core_if_end_872
core_else_871:
core_if_end_872:
        LRM R0,[R8-4]
        ADI R0,R0,1
        SRM R0,[R8-4]
        JMP core_while_869
core_while_end_870:
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-4]
core_while_873:
        LRM R0,[R8-4]
        ADI R1,RZR,2
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_874
        LRM R0,[R8-4]
        PSH R0
        Call core_sysfat
        ADI SP,SP,4
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_875
        LRM R0,[R8-20]
        ADI R0,R0,28
        LRM R0,[R0]
        LRM R1,[R8-4]
        ADD R0,R0,R1
        ADI R0,R0,-2
        SRM R0,[R8-12]
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_877
        LRM R0,[R8-12]
        ADI R0,R0,162
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        JMP core_if_end_878
core_else_877:
core_if_end_878:
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_879
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_880
core_else_879:
core_if_end_880:
        JMP core_if_end_876
core_else_875:
core_if_end_876:
        LRM R0,[R8-4]
        ADI R0,R0,-1
        SRM R0,[R8-4]
        JMP core_while_873
core_while_end_874:
        LRM R0,[R8-20]
        ADI R0,R0,20
        LRM R0,[R0]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,24
        LRM R0,[R0]
        ADI R1,RZR,16
        Call core_div
        ADD R1,R0,RZR
        POP R0
        ADD R0,R0,R1
        ADI R0,R0,-1
        SRM R0,[R8-12]
core_while_881:
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R8-20]
        ADI R0,R0,12
        LRM R0,[R0]
        ADD R1,R0,RZR
        POP R0
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_882
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_883
        LRM R0,[R8-12]
        ADI R0,R0,162
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        JMP core_if_end_884
core_else_883:
core_if_end_884:
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_885
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_886
core_else_885:
core_if_end_886:
        LRM R0,[R8-12]
        ADI R0,R0,-1
        SRM R0,[R8-12]
        JMP core_while_881
core_while_end_882:
        LRM R0,[R12+core_literal_862-8000h]
        ADI R0,R0,76
        PSH R0
        LRM R0,[R12+core_literal_862-8000h]
        ADI R0,R0,76
        PSH R0
        Call core_le16
        ADI SP,SP,4
        ADI R0,R0,-162
        PSH R0
        Call core_set16
        ADI SP,SP,8
        LRM R0,[R12+core_literal_862-8000h]
        ADI R0,R0,112
        PSH R0
        ADI R0,RZR,162
        PSH R0
        Call core_set32
        ADI SP,SP,8
        ADI R0,RZR,162
        PSH R0
        LRM R0,[R12+core_literal_862-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_887
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_888
core_else_887:
core_if_end_888:
        JMP core_if_end_866
core_else_865:
core_if_end_866:
        ADI R0,RZR,1
        SRM R0,[R8-12]
core_while_889:
        LRM R0,[R8-12]
        ADI R1,RZR,162
        SUB R1,R0,R1
        ADI R0,RZR,0
        LRM R2,[R12+core_literal_10-8000h]
        ANL R1,R1,R2
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_while_end_890
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        ADI R0,RZR,0
        PSH R0
        ADI R0,RZR,0
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_891
        LRM R0,[R8-12]
        PSH R0
        LRM R0,[R12+core_literal_289-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        JMP core_if_end_892
core_else_891:
core_if_end_892:
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SRZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_893
        LRM R0,[R8-24]
        JMP core_return_843
        JMP core_if_end_894
core_else_893:
core_if_end_894:
        LRM R0,[R8-12]
        ADI R0,R0,1
        SRM R0,[R8-12]
        JMP core_while_889
core_while_end_890:
        ADI R0,RZR,0
        PSH R0
        LRM R0,[R12+core_literal_855-8000h]
        PSH R0
        LRM R0,[R8-28]
        PSH R0
        ADI R0,RZR,1
        PSH R0
        Call core_block
        ADI SP,SP,16
        SRM R0,[R8-24]
        LRM R0,[R8-20]
        ADI R0,R0,4
        ADI R1,RZR,0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_40-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R12+core_literal_41-8000h]
        PSH R0
        ADI R0,RZR,1
        SUB R0,RZR,R0
        ADD R1,R0,RZR
        POP R0
        SRM R1,[R0]
        LRM R0,[R8-24]
        ADI R1,RZR,0
        SUB R1,R0,R1
        ADI R0,RZR,0
        SNZ R1
        ADI R0,RZR,1
        SNZ R0
        JMP core_else_895
        LRM R0,[R12+core_literal_898-8000h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_newline
        JMP core_if_end_896
core_else_895:
core_if_end_896:
        LRM R0,[R8-24]
        JMP core_return_843
        ADI R0,RZR,0
core_return_843:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub
        INCLUDE arithmetic.inc
        INCLUDE blockio.inc
        INCLUDE answer.inc
core_literal_2:
        DD 16646152
core_literal_10:
        DD 2147483648
core_literal_28:
        DD 65535
core_literal_29:
        DD 65536
core_literal_38:
        DD 327680
core_literal_39:
        DD 81904
core_literal_40:
        DD 81908
core_literal_41:
        DD 81912
core_literal_46:
        DD 81888
core_literal_50:
        DD 196608
core_literal_75:
        DD 212992
core_literal_85:
        DD 81892
core_literal_106:
        DD 262144
core_literal_130:
        DD 83968
core_literal_151:
        DD 81920
core_literal_203:
        DD 81868
core_literal_204:
        DD 81860
core_literal_205:
        DD 81856
core_literal_206:
        DD 81864
core_literal_240:
        DD 20000h+core_string_239
core_literal_243:
        DD 87040
core_literal_247:
        DD 20000h+core_string_246
core_literal_253:
        DD 81840
core_literal_257:
        DD 20000h+core_string_256
core_literal_266:
        DD 86016
core_literal_272:
        DD 20000h+core_string_271
core_literal_274:
        DD 20000h+core_string_273
core_literal_289:
        DD 270336
core_literal_341:
        DD 20000h+core_string_340
core_literal_345:
        DD 20000h+core_string_344
core_literal_349:
        DD 20000h+core_string_348
core_literal_353:
        DD 20000h+core_string_352
core_literal_357:
        DD 20000h+core_string_356
core_literal_361:
        DD 20000h+core_string_360
core_literal_365:
        DD 20000h+core_string_364
core_literal_369:
        DD 20000h+core_string_368
core_literal_373:
        DD 20000h+core_string_372
core_literal_377:
        DD 20000h+core_string_376
core_literal_381:
        DD 20000h+core_string_380
core_literal_383:
        DD 20000h+core_string_382
core_literal_391:
        DD 20000h+core_string_390
core_literal_395:
        DD 20000h+core_string_394
core_literal_399:
        DD 20000h+core_string_398
core_literal_403:
        DD 20000h+core_string_402
core_literal_407:
        DD 20000h+core_string_406
core_literal_411:
        DD 20000h+core_string_410
core_literal_415:
        DD 20000h+core_string_414
core_literal_419:
        DD 20000h+core_string_418
core_literal_421:
        DD 20000h+core_string_420
core_literal_441:
        DD 84992
core_literal_552:
        DD 458752
core_literal_566:
        DD 425984
core_literal_577:
        DD 20000h+core_string_576
core_literal_594:
        DD 20000h+core_string_593
core_literal_608:
        DD 20000h+core_string_607
core_literal_632:
        DD 352256
core_literal_633:
        DD 352768
core_literal_636:
        DD 409600
core_literal_655:
        DD 20000h+core_string_654
core_literal_657:
        DD 20000h+core_string_656
core_literal_664:
        DD 353280
core_literal_669:
        DD 393216
core_literal_676:
        DD 286720
core_literal_721:
        DD 20000h+core_string_720
core_literal_732:
        DD 475136
core_literal_750:
        DD 20000h+core_string_749
core_literal_752:
        DD 20000h+core_string_751
core_literal_779:
        DD 786432
core_literal_780:
        DD 786944
core_literal_840:
        DD 798720
core_literal_855:
        DD 794624
core_literal_861:
        DD 827343435
core_literal_862:
        DD 790528
core_literal_898:
        DD 20000h+core_string_897
core_string_239:
        DD 46,0
core_string_246:
        DD 47,66,0
core_string_256:
        DD 32,68,105,114,101,99,116,111,114,121,32,111,102,32,0
core_string_271:
        DD 32,32,60,68,73,82,62,0
core_string_273:
        DD 32,32,0
core_string_340:
        DD 70,105,108,101,32,111,114,32,112,97,116,104,32,110,111,116,32,102,111,117,110,100,0
core_string_344:
        DD 65,99,99,101,115,115,32,100,101,110,105,101,100,0
core_string_348:
        DD 73,110,118,97,108,105,100,32,100,114,105,118,101,32,115,112,101,99,105,102,105,99,97,116,105,111,110,0
core_string_352:
        DD 67,97,110,110,111,116,32,114,101,109,111,118,101,32,99,117,114,114,101,110,116,32,100,105,114,101,99,116,111,114,121,0
core_string_356:
        DD 87,114,105,116,101,32,112,114,111,116,101,99,116,32,101,114,114,111,114,0
core_string_360:
        DD 68,114,105,118,101,32,110,111,116,32,114,101,97,100,121,0
core_string_364:
        DD 73,110,115,117,102,102,105,99,105,101,110,116,32,100,105,115,107,32,115,112,97,99,101,0
core_string_368:
        DD 73,110,118,97,108,105,100,32,112,97,114,97,109,101,116,101,114,0
core_string_372:
        DD 68,117,112,108,105,99,97,116,101,32,102,105,108,101,32,110,97,109,101,32,111,114,32,102,105,108,101,32,110,111,116,32,102,111,117,110,100,0
core_string_376:
        DD 70,105,108,101,32,99,97,110,110,111,116,32,98,101,32,99,111,112,105,101,100,32,111,110,116,111,32,105,116,115,101,108,102,0
core_string_380:
        DD 80,97,116,104,32,116,111,111,32,108,111,110,103,0
core_string_382:
        DD 73,110,118,97,108,105,100,32,100,105,115,107,32,111,114,32,100,97,116,97,32,101,114,114,111,114,0
core_string_390:
        DD 47,63,0
core_string_394:
        DD 68,73,82,32,91,112,97,116,104,93,32,91,47,66,93,0
core_string_398:
        DD 77,75,68,73,82,32,112,97,116,104,0
core_string_402:
        DD 82,77,68,73,82,32,112,97,116,104,0
core_string_406:
        DD 68,82,73,86,69,32,91,48,124,49,124,50,124,51,93,0
core_string_410:
        DD 67,79,80,89,32,115,111,117,114,99,101,32,100,101,115,116,105,110,97,116,105,111,110,32,91,47,89,93,0
core_string_414:
        DD 77,79,86,69,32,115,111,117,114,99,101,32,100,101,115,116,105,110,97,116,105,111,110,0
core_string_418:
        DD 68,69,76,69,84,69,32,112,97,116,104,32,91,47,80,93,0
core_string_420:
        DD 83,89,83,32,49,58,124,50,58,124,51,58,0
core_string_576:
        DD 47,80,0
core_string_593:
        DD 65,108,108,32,102,105,108,101,115,32,105,110,32,100,105,114,101,99,116,111,114,121,32,119,105,108,108,32,98,101,32,100,101,108,101,116,101,100,33,32,65,114,101,32,121,111,117,32,115,117,114,101,32,40,89,47,78,41,63,32,0
core_string_607:
        DD 44,32,68,101,108,101,116,101,32,40,89,47,78,41,63,32,0
core_string_654:
        DD 79,118,101,114,119,114,105,116,101,32,0
core_string_656:
        DD 32,40,89,101,115,47,78,111,41,63,32,0
core_string_720:
        DD 47,89,0
core_string_749:
        DD 32,32,32,32,32,32,32,32,0
core_string_751:
        DD 32,102,105,108,101,40,115,41,32,99,111,112,105,101,100,0
core_string_897:
        DD 83,121,115,116,101,109,32,116,114,97,110,115,102,101,114,114,101,100,0
