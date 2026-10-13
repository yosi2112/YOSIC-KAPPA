; Per-drive current paths occupy STATES + drive*512 + 64 (64 word chars).
; core_fs_command calls this with one argument-tail pointer on the stack.
core_chdir_help:
        PutText "CHDIR [path]"
        JMP core_if_end_417
core_chdir_command:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-12
        LRM R0,[R8+8]
        PSH R0
        Call core_tokens
        ADI SP,SP,4
        SNZ R0
        JMP core_chdir_current
        ADI R0,R0,-1
        SRZ R0
        JMP core_chdir_parameter
        Const R0,15400h
        LRM R0,[R0]
        SRM R0,[R8-4]
        LRM R1,[R0]
        SNZ R1
        JMP core_chdir_parameter
        ADI R1,R1,-47
        SRZ R1
        JMP core_chdir_path
        JMP core_chdir_parameter
core_chdir_path:
        LRM R1,[R0+4]
        ADI R1,R1,-58
        SRZ R1
        JMP core_chdir_resolve
        LRM R1,[R0+8]
        SRZ R1
        JMP core_chdir_resolve
        LRM R0,[R0]
        ADI R0,R0,-48
        JMP core_chdir_query
core_chdir_current:
        Const R0,13ff0h
        LRM R0,[R0]
core_chdir_query:
        PSH R0
        Call core_select
        ADI SP,SP,4
        SRZ R0
        JMP core_chdir_return
        Const R0,13fe0h
        LRM R0,[R0]
        LRM R0,[R0]
        ADI R0,R0,48
        PSH R0
        Call core_putc
        ADI SP,SP,4
        PutChar 58
        Const R0,13fe0h
        LRM R0,[R0]
        ADI R0,R0,64
        PSH R0
        Call core_puts
        ADI SP,SP,4
        Call core_newline
        ADI R0,RZR,0
        JMP core_chdir_return
core_chdir_resolve:
        LRM R0,[R8-4]
        PSH R0
        PSH RZR
        Call core_resolve
        ADI SP,SP,8
        SRZ R0
        JMP core_chdir_return
        Const R0,13fcch
        LRM R0,[R0]
        SRZ R0
        JMP core_chdir_commit
        Const R0,13fc4h
        LRM R0,[R0]
        LRM R0,[R0+44]
        ADI R1,RZR,16
        ANL R0,R0,R1
        SNZ R0
        JMP core_chdir_denied
        ; Validate the entire directory chain before changing the stored path.
        Const R0,13fc8h
        LRM R0,[R0]
        SRM R0,[R8-8]
        Const R0,13fe0h
        LRM R0,[R0]
        LRM R0,[R0+32]
        SRM R0,[R8-12]
core_chdir_chain:
        LRM R0,[R8-12]
        SNZ R0
        JMP core_chdir_data_error
        ADI R0,R0,-1
        SRM R0,[R8-12]
        LRM R0,[R8-8]
        PSH R0
        Call core_fatget
        ADI SP,SP,4
        Const R1,13fe4h
        LRM R1,[R1]
        SRZ R1
        JMP core_chdir_data_error
        ADI R1,R0,-2
        Const R2,80000000h
        ANL R1,R1,R2
        SRZ R1
        JMP core_chdir_data_error
        ADI R1,R0,-0ff0h
        ANL R1,R1,R2
        SNZ R1
        JMP core_chdir_check_end
        SRM R0,[R8-8]
        JMP core_chdir_chain
core_chdir_check_end:
        ADI R0,R0,-0ff8h
        ANL R0,R0,R2
        SRZ R0
        JMP core_chdir_data_error
core_chdir_commit:
        Const R0,13fe0h
        LRM R0,[R0]
        ADI R0,R0,64
        PSH R0
        Const R0,14000h
        PSH R0
        Call core_copytext
        ADI SP,SP,8
        JMP core_chdir_return
core_chdir_denied:
        ADI R0,RZR,5
        JMP core_chdir_return
core_chdir_parameter:
        ADI R0,RZR,87
        JMP core_chdir_return
core_chdir_data_error:
        ADI R0,RZR,13
core_chdir_return:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub

; True if resolved PATHBUF names the selected drive's current dir/ancestor.
; Components, not mere string prefixes, determine ancestry (A != AB).
core_cwd_busy:
        Const R0,14000h
        Const R1,13fe0h
        LRM R1,[R1]
        ADI R1,R1,64
core_cwd_compare:
        LRM R2,[R0]
        SNZ R2
        JMP core_cwd_boundary
        LRM R7,[R1]
        SUB R7,R7,R2
        SRZ R7
        JMP core_cwd_free
        ADI R0,R0,4
        ADI R1,R1,4
        JMP core_cwd_compare
core_cwd_boundary:
        LRM R0,[R1]
        SNZ R0
        JMP core_cwd_used
        ADI R0,R0,-92
        SNZ R0
        JMP core_cwd_used
core_cwd_free:
        ADI R0,RZR,0
        JMP return_sub
core_cwd_used:
        ADI R0,RZR,1
        JMP return_sub
