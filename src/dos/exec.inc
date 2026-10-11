; Independent YKE1 loader. The existing core.inc remains a canonical input.
; Scratch 4C00h..4C38h; packed staging 90000h; application CS=8.
; Every FAT chain and the entire header/payload are validated before launch.
ExecArg MACRO value
        Const R0,value
        PSH R0
        ENDM

exec_command:
        PSH R6
        SRM RZR,[RZR+4c30h]
        Const R0,13fe4h
        SRM RZR,[R0]
        LRM R8,[SP+8]
        ADI R9,RZR,6000h
        ADI R3,RZR,0
        ADI R4,RZR,0
        ADI R5,RZR,0
exec_token:
        LRM R1,[R8]
        SNZ R1
        JMP exec_token_end
        ADI R2,R1,-34
        SNZ R2
        JMP exec_quote
        SRZ R5
        JMP exec_char
        ADI R2,R1,-32
        SNZ R2
        JMP exec_token_end
        ADI R2,R1,-9
        SNZ R2
        JMP exec_token_end
exec_char:
        Upper
        SRM R1,[R9]
        ADI R9,R9,4
        ADI R3,R3,1
        ADI R2,R1,-92
        SNZ R2
        ADI R4,RZR,0
        ADI R2,R1,-58
        SNZ R2
        ADI R4,RZR,0
        ADI R2,R1,-46
        SNZ R2
        ADI R4,RZR,1
        ADI R8,R8,4
        JMP exec_token
exec_quote:
        ADI R2,RZR,1
        XOR R5,R5,R2
        ADI R8,R8,4
        JMP exec_token
exec_token_end:
        SRZ R5
        JMP exec_parameter
        SNZ R3
        JMP exec_parameter
        SRZ R4
        JMP exec_extension
        ADI R1,RZR,46
        SRM R1,[R9]
        ADI R1,RZR,89
        SRM R1,[R9+4]
        ADI R1,RZR,75
        SRM R1,[R9+8]
        ADI R1,RZR,69
        SRM R1,[R9+12]
        ADI R9,R9,16
exec_extension:
        ; Require .YKE, including on an explicitly named file.
        LRM R1,[R9-16]
        ADI R1,R1,-46
        SRZ R1
        JMP exec_format
        LRM R1,[R9-12]
        ADI R1,R1,-89
        SRZ R1
        JMP exec_format
        LRM R1,[R9-8]
        ADI R1,R1,-75
        SRZ R1
        JMP exec_format
        LRM R1,[R9-4]
        ADI R1,R1,-69
        SRZ R1
        JMP exec_format
        SRM RZR,[R9]
exec_tail_space:
        LRM R1,[R8]
        ADI R2,R1,-32
        SNZ R2
        JMP exec_tail_next
        ADI R2,R1,-9
        SNZ R2
        JMP exec_tail_next
        SRM R8,[RZR+4c00h]
        JMP exec_resolve
exec_tail_next:
        ADI R8,R8,4
        JMP exec_tail_space
exec_resolve:
        ExecArg 6000h
        ExecArg 0
        Call core_resolve
        ADI SP,SP,8
        SRZ R0
        JMP exec_error
        Const R1,13fcch
        LRM R0,[R1]
        SRZ R0
        JMP exec_denied
        Const R1,13fc4h
        LRM R1,[R1]
        LRM R0,[R1+44]
        ADI R2,RZR,24
        ANL R0,R0,R2
        SRZ R0
        JMP exec_denied
        ADI R0,R1,112
        PSH R0
        Call core_le32
        ADI SP,SP,4
        SRM R0,[RZR+4c04h]
        ; Check signed bit before comparisons; file size is 28..65552.
        Const R1,80000000h
        ANL R2,R0,R1
        SRZ R2
        JMP exec_format
        ADI R2,R0,-28
        ANL R2,R2,R1
        SRZ R2
        JMP exec_format
        Const R2,65552
        SUB R2,R2,R0
        ANL R2,R2,R1
        SRZ R2
        JMP exec_format
        ADI R1,RZR,3
        ANL R1,R0,R1
        SRZ R1
        JMP exec_format
        ADI R0,R0,511
        ADI R1,RZR,512
        Call core_div
        SRM R0,[RZR+4c08h]
        Const R1,13fc4h
        LRM R1,[R1]
        ADI R0,R1,104
        PSH R0
        Call core_le16
        ADI SP,SP,4
        PSH R0
        LRM R0,[RZR+4c04h]
        PSH R0
        ExecArg 64000h
        Call core_chain_load
        ADI SP,SP,12
        Const R1,80000000h
        ANL R1,R0,R1
        SRZ R1
        JMP exec_disk_error
        LRM R1,[RZR+4c08h]
        SUB R1,R0,R1
        SRZ R1
        JMP exec_data_error
        SRM RZR,[RZR+4c0ch]
        Const R0,90000h
        SRM R0,[RZR+4c10h]
exec_sector:
        LRM R0,[RZR+4c0ch]
        ADD R0,R0,R0
        ADD R0,R0,R0
        Const R1,64000h
        ADD R1,R1,R0
        LRM R0,[R1]
        ADI R0,R0,-2
        Const R1,13fe0h
        LRM R1,[R1]
        LRM R2,[R1+28]
        ADD R0,R0,R2
        PSH R0
        LRM R1,[R1]
        SRM R1,[RZR+4c38h]
        ExecArg 30000h
        LRM R0,[RZR+4c38h]
        PSH R0
        ExecArg 0
        Call core_block
        ADI SP,SP,16
        SRZ R0
        JMP exec_error
        Const R0,30000h
        SRM R0,[RZR+4c14h]
        ADI R0,RZR,128
        SRM R0,[RZR+4c18h]
exec_pack:
        LRM R0,[RZR+4c14h]
        PSH R0
        Call core_le32
        ADI SP,SP,4
        LRM R1,[RZR+4c10h]
        SRM R0,[R1]
        ADI R1,R1,4
        SRM R1,[RZR+4c10h]
        Const R0,90000h
        LRM R2,[RZR+4c04h]
        ADD R0,R0,R2
        SUB R0,R0,R1
        SNZ R0
        JMP exec_header
        LRM R0,[RZR+4c14h]
        ADI R0,R0,16
        SRM R0,[RZR+4c14h]
        LRM R0,[RZR+4c18h]
        ADI R0,R0,-1
        SRM R0,[RZR+4c18h]
        SRZ R0
        JMP exec_pack
        LRM R0,[RZR+4c0ch]
        ADI R0,R0,1
        SRM R0,[RZR+4c0ch]
        JMP exec_sector
exec_header:
        Const R8,90000h
        LRM R0,[R8]
        Const R1,31454b59h
        SUB R0,R0,R1
        SRZ R0
        JMP exec_format
        LRM R0,[R8+4]
        ADI R0,R0,-1
        SRZ R0
        JMP exec_format
        LRM R0,[R8+8]
        LRM R1,[RZR+4c04h]
        ADI R1,R1,-24
        SUB R2,R0,R1
        SRZ R2
        JMP exec_format
        SRM R0,[RZR+4c1ch]
        LRM R1,[R8+12]
        ADI R2,RZR,3
        ANL R2,R1,R2
        SRZ R2
        JMP exec_format
        Const R2,80000000h
        ANL R3,R1,R2
        SRZ R3
        JMP exec_format
        SUB R3,R1,R0
        ANL R3,R3,R2
        SNZ R3
        JMP exec_format
        SRM R1,[RZR+4c20h]
        LRM R0,[R8+20]
        SRZ R0
        JMP exec_format
        ; Verify checksum first; no partial application is ever launched.
        ADI R9,R8,24
        ADI R3,RZR,0
        LRM R4,[RZR+4c1ch]
exec_checksum:
        LRM R0,[R9]
        ADD R3,R3,R0
        ADI R9,R9,4
        ADI R4,R4,-4
        SRZ R4
        JMP exec_checksum
        LRM R0,[R8+16]
        SUB R0,R0,R3
        SRZ R0
        JMP exec_format
        Const R9,80000h
        Const R4,65528
exec_zero:
        SRM RZR,[R9]
        ADI R9,R9,4
        ADI R4,R4,-4
        SRZ R4
        JMP exec_zero
        Const R8,90018h
        Const R9,80000h
        LRM R4,[RZR+4c1ch]
exec_copy:
        LRM R0,[R8]
        SRM R0,[R9]
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R4,R4,-4
        SRZ R4
        JMP exec_copy
        LRM R8,[RZR+4c00h]
        Const R9,0f1000h
        ADI R2,RZR,0
exec_args:
        LRM R0,[R8]
        SRM R0,[R9]
        SNZ R0
        JMP exec_launch
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R2,R2,1
        JMP exec_args
exec_launch:
        ; Application return gate uses R5=2; retain R0 as exit status.
        Const R1,80050000h
        Const R9,2fff8h
        SRM R1,[R9]
        Const R9,8fff8h
        SRM R1,[R9]
        ; Dynamic JMP from PC=FFFCh to the validated entry offset.
        ; Divide by four via core_div on a positive offset.
        ; PC wraps modulo 64KiB: (entry+4)/4 reaches entry from FFFC.
        LRM R0,[RZR+4c20h]
        ADI R0,R0,4
        ADI R1,RZR,4
        PSH R2
        Call core_div
        POP R2
        Const R1,18000000h
        ORR R0,R0,R1
        Const R9,8fffch
        SRM R0,[R9]
        LRM R0,[R12+exec_return_jump-8000h]
        Const R9,2fffch
        SRM R0,[R9]
        SRM SP,[RZR+4c28h]
        Const SP,0f0000h
        Const R1,0f1000h
        Const R9,13ff0h
        LRM R3,[R9]
        Const R4,0fff8h
        ADI R5,RZR,8
        JMP 0fff8h
exec_return_jump:
        DD 18000000h+(((exec_return-0fffch)/4)&1fffffh)
exec_return:
        ; Restore kernel segments, stack and persistent shell registers.
        LSEG DS,RZR
        LSEG SS,RZR
        LSEG ES,RZR
        LRM SP,[RZR+4c28h]
        SRM R0,[RZR+4c30h]
        Const R10,0fe0000h
        ADI R11,RZR,256
        Const R12,28000h
        Const R1,13ff4h
        ADI R0,RZR,-1
        SRM R0,[R1]
        SRM R0,[R1+4]
        JMP exec_done
exec_parameter:
        ADI R0,RZR,87
        JMP exec_error
exec_denied:
        ADI R0,RZR,5
        JMP exec_error
exec_data_error:
        ADI R0,RZR,13
        JMP exec_error
exec_disk_error:
        Const R1,13fe4h
        LRM R0,[R1]
        SNZ R0
        ADI R0,RZR,13
        JMP exec_error
exec_format:
        PutText "Invalid YKE executable"
        NewLine
        ADI R0,RZR,11
        SRM R0,[RZR+4c30h]
        JMP exec_done
exec_error:
        SRM R0,[RZR+4c30h]
        PSH R0
        Call core_error_message
        ADI SP,SP,4
exec_done:
        LRM R0,[RZR+4c30h]
        Const R1,13f00h
        SRM R0,[R1]
        POP R6
        JMP return_sub
