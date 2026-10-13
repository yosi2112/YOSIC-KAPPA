; R1=function, R2=argument, R7=aligned return offset in the loaded payload.
; Non-reentrant frame 4D00h..4D38h. R5 is consumed by transition gates.
syscall_entry_jump:
        DD 18000000h+(((syscall_entry-0fff8h)/4)&1fffffh)
syscall_entry:
        LSEG DS,RZR
        LSEG SS,RZR
        LSEG ES,RZR
        SRM R1,[RZR+4d00h]
        SRM R2,[RZR+4d04h]
        SRM R3,[RZR+4d08h]
        SRM R4,[RZR+4d0ch]
        SRM R6,[RZR+4d10h]
        SRM R7,[RZR+4d14h]
        SRM R8,[RZR+4d18h]
        SRM R9,[RZR+4d1ch]
        SRM R10,[RZR+4d20h]
        SRM R11,[RZR+4d24h]
        SRM R12,[RZR+4d28h]
        SRM R13,[RZR+4d2ch]
        SRM SP,[RZR+4d30h]
        MediaLock
        LRM R7,[RZR+4d14h]
        LRM SP,[RZR+4c28h]
        Const R10,0fe0000h
        ADI R11,RZR,256
        Const R12,28000h
        ; EXIT needs no resume address and passes R2 directly back to DOS.
        SRZ R1
        JMP syscall_check_return
        ADD R0,R2,RZR
        JMP exec_return
syscall_check_return:
        ADI R0,RZR,3
        ANL R0,R7,R0
        SRZ R0
        JMP syscall_bad_return
        Const R0,80000000h
        ANL R3,R7,R0
        SRZ R3
        JMP syscall_bad_return
        LRM R3,[RZR+4c1ch]
        SUB R3,R7,R3
        ANL R3,R3,R0
        SNZ R3
        JMP syscall_bad_return
        SRM RZR,[RZR+4d34h]
        ADI R0,R1,-1
        SNZ R0
        JMP syscall_putchar
        ADI R0,R1,-2
        SNZ R0
        JMP syscall_puts
        ADI R0,R1,-3
        SNZ R0
        JMP syscall_getchar
        ADI R0,R1,-4
        SNZ R0
        JMP syscall_getdrive
        ADI R0,R1,-5
        SNZ R0
        JMP syscall_setdrive
        ADI R0,R1,-20h
        Const R3,80000000h
        ANL R4,R0,R3
        SRZ R4
        JMP syscall_command
        ADI R4,R0,-4
        ANL R4,R4,R3
        SNZ R4
        JMP syscall_command
        JMP file_dispatch
syscall_command:
        ; Map 10h..17h to filesystem command numbers 1,2,3,5,6,7,8,9.
        ADI R0,R1,-10h
        Const R3,80000000h
        ANL R4,R0,R3
        SRZ R4
        JMP syscall_unknown
        ADI R4,R0,-8
        ANL R4,R4,R3
        SNZ R4
        JMP syscall_unknown
        ADI R0,R0,1
        ADI R4,R0,-4
        ANL R4,R4,R3
        SNZ R4
        ADI R0,R0,1
        SRM R0,[RZR+4d38h]
        ADI R4,R0,-1
        SNZ R4
        JMP syscall_command_text
        ADI R4,R0,-9
        SNZ R4
        JMP syscall_command_text
        Call file_has_open
        SRZ R0
        JMP syscall_denied
syscall_command_text:
        ADI R3,RZR,127
        Call syscall_text
        SRZ R0
        JMP syscall_result
        LRM R0,[RZR+4d38h]
        PSH R0
        LRM R0,[RZR+4d04h]
        PSH R0
        Call core_fs_command
        ADI SP,SP,8
        JMP syscall_result
syscall_putchar:
        ADI R0,RZR,255
        ANL R0,R2,R0
        SUB R0,R2,R0
        SRZ R0
        JMP syscall_parameter
        SRM R2,[R10+8]
        JMP syscall_return
syscall_puts:
        ADI R3,RZR,4095
        Call syscall_text
        SRZ R0
        JMP syscall_result
        LRM R0,[RZR+4d04h]
        PSH R0
        Call core_puts
        ADI SP,SP,4
        JMP syscall_result
syscall_getchar:
        LRM R0,[R10]
        ADI R3,RZR,1
        ANL R0,R0,R3
        SNZ R0
        JMP syscall_no_character
        LRM R0,[R10+4]
        SRM R0,[RZR+4d04h]
        JMP syscall_return
syscall_no_character:
        ADI R0,RZR,-1
        SRM R0,[RZR+4d04h]
        JMP syscall_return
syscall_getdrive:
        Const R0,13ff0h
        LRM R0,[R0]
        SRM R0,[RZR+4d04h]
        JMP syscall_return
syscall_setdrive:
        PSH R2
        Call core_select
        ADI SP,SP,4
        SRZ R0
        JMP syscall_result
        LRM R0,[RZR+4d04h]
        Const R3,13ff0h
        SRM R0,[R3]
        JMP syscall_return
syscall_unknown:
        ADI R0,RZR,1
        JMP syscall_result
syscall_denied:
        ADI R0,RZR,5
        JMP syscall_result
syscall_parameter:
        ADI R0,RZR,87
syscall_result:
        SRM R0,[RZR+4d34h]
syscall_return:
        MediaUnlock
        ; Reuse the launch trampoline while keeping entry FFF0/FFF4 intact.
        LRM R0,[RZR+4d14h]
        ADI R0,R0,4
        ADI R1,RZR,4
        Call core_div
        Const R1,18000000h
        ORR R0,R0,R1
        Const R1,8fffch
        SRM R0,[R1]
        LRM R1,[RZR+4d00h]
        LRM R2,[RZR+4d04h]
        LRM R3,[RZR+4d08h]
        LRM R4,[RZR+4d0ch]
        LRM R6,[RZR+4d10h]
        LRM R7,[RZR+4d14h]
        LRM R8,[RZR+4d18h]
        LRM R9,[RZR+4d1ch]
        LRM R10,[RZR+4d20h]
        LRM R11,[RZR+4d24h]
        LRM R12,[RZR+4d28h]
        LRM R13,[RZR+4d2ch]
        LRM SP,[RZR+4d30h]
        LRM R0,[RZR+4d34h]
        ADI R5,RZR,8
        JMP 0fff4h
syscall_bad_return:
        ADI R0,RZR,87
        JMP exec_return

; Check the whole string before I/O. Call clobbers R2, so use saved pointer.
; R3=max length; R0=status. A character is one word containing 0..255.
syscall_text:
        LRM R8,[RZR+4d04h]
        ADD R9,R3,RZR
        ADI R0,RZR,3
        ANL R0,R8,R0
        SRZ R0
        JMP syscall_text_bad
        Const R4,80000000h
        ANL R0,R8,R4
        SRZ R0
        JMP syscall_text_bad
        Const R0,80000h
        SUB R0,R8,R0
        ANL R0,R0,R4
        SRZ R0
        JMP syscall_text_bad
        Const R3,8fff0h
        SUB R0,R8,R3
        ANL R0,R0,R4
        SNZ R0
        JMP syscall_text_work
        JMP syscall_text_scan
syscall_text_work:
        Const R0,0d0000h
        SUB R0,R8,R0
        ANL R0,R0,R4
        SRZ R0
        JMP syscall_text_bad
        Const R3,0f0000h
        SUB R0,R8,R3
        ANL R0,R0,R4
        SNZ R0
        JMP syscall_text_args
        JMP syscall_text_scan
syscall_text_args:
        Const R0,0f1000h
        SUB R0,R8,R0
        ANL R0,R0,R4
        SRZ R0
        JMP syscall_text_bad
        Const R3,0f1200h
        SUB R0,R8,R3
        ANL R0,R0,R4
        SNZ R0
        JMP syscall_text_bad
syscall_text_scan:
        SUB R0,R8,R3
        ANL R0,R0,R4
        SNZ R0
        JMP syscall_text_bad
        LRM R0,[R8]
        SNZ R0
        JMP syscall_text_ok
        ADI R1,RZR,255
        ANL R1,R0,R1
        SUB R1,R0,R1
        SRZ R1
        JMP syscall_text_bad
        SNZ R9
        JMP syscall_text_bad
        ADI R9,R9,-1
        ADI R8,R8,4
        JMP syscall_text_scan
syscall_text_ok:
        ADI R0,RZR,0
        JMP return_sub
syscall_text_bad:
        ADI R0,RZR,87
        JMP return_sub
