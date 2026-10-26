; Internal transient entry/return guard. No application notifications or ABI.
; Drive state +52 BPB fingerprint, +56 entry-valid; +60 unused.
; Compare boot bytes 0..111 and expected root CSV, preserving sticky RO.
; API writes refresh the CSV through core_media_written. Checksums cannot
; identify identical replacements or guarantee detection of collisions.
media_enter:
        PSH RZR
        JMP media_guard
media_leave:
        ADI R0,RZR,1
        PSH R0
media_guard:
        PSH R6
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-32
        LRM R0,[R8+8]
        SRM R0,[R8-8]
        ExecConst R0,13fe0h
        LRM R1,[R0]
        SRM R1,[R8-12]
        ExecConst R0,13ff4h
        LRM R1,[R0]
        SRM R1,[R8-16]
        LRM R1,[R0+4]
        SRM R1,[R8-20]
        SRM RZR,[R8-24]
        ExecConst R0,50000h
        SRM R0,[R8-4]
media_guard_drive:
        LRM R7,[R8-4]
        LRM R0,[R8-8]
        SNZ R0
        SRM RZR,[R7+56]
        LRM R0,[R7+4]
        SNZ R0
        JMP media_guard_next
        ExecConst R0,13fe0h
        SRM R7,[R0]
        ; Save software RO and detect a fresh mismatch even if already sticky.
        LRM R0,[R7+40]
        SRM R0,[R8-28]
        SRM RZR,[R7+40]
        PSH RZR
        ExecConst R0,44800h
        PSH R0
        LRM R0,[R7]
        PSH R0
        PSH RZR
        Call core_block
        ADI SP,SP,16
        SRZ R0
        JMP media_guard_changed
        ExecConst R1,44800h
        ADI R2,RZR,112
        ADI R3,RZR,0
media_guard_hash:
        ADD R4,R3,RZR
        REPT 5
        ADD R3,R3,R3
        ENDM
        ADD R3,R3,R4
        LRM R4,[R1]
        ADD R3,R3,R4
        ADI R1,R1,4
        ADI R2,R2,-1
        SRZ R2
        JMP media_guard_hash
        LRM R7,[R8-4]
        LRM R0,[R8-8]
        SNZ R0
        JMP media_guard_store
        LRM R0,[R7+56]
        SNZ R0
        JMP media_guard_csv
        LRM R0,[R7+52]
        SUB R0,R0,R3
        SRZ R0
        JMP media_guard_changed
        JMP media_guard_csv
media_guard_store:
        SRM R3,[R7+52]
        ADI R0,RZR,1
        SRM R0,[R7+56]
media_guard_csv:
        Call core_media_check
        SRZ R0
        JMP media_guard_changed
        LRM R7,[R8-4]
        LRM R0,[R7+40]
        SRZ R0
        JMP media_guard_changed
        LRM R0,[R8-28]
        SRM R0,[R7+40]
        JMP media_guard_next
media_guard_changed:
        LRM R7,[R8-4]
        ADI R0,RZR,1
        SRM R0,[R7+40]
        SRM RZR,[R7+4]
        SRM RZR,[R7+44]
        SRM RZR,[R7+56]
        ADI R0,RZR,92
        SRM R0,[R7+64]
        SRM RZR,[R7+68]
        LRM R0,[R7]
        LRM R1,[R8-16]
        SUB R0,R0,R1
        SRZ R0
        JMP media_guard_next
        ADI R0,RZR,1
        SRM R0,[R8-24]
media_guard_next:
        LRM R0,[R8-4]
        ADI R0,R0,512
        SRM R0,[R8-4]
        ExecConst R1,50800h
        SUB R0,R0,R1
        SRZ R0
        JMP media_guard_drive
        ExecConst R0,13fe0h
        LRM R1,[R8-12]
        SRM R1,[R0]
        ExecConst R0,13ff4h
        LRM R1,[R8-16]
        LRM R2,[R8-20]
        LRM R3,[R8-24]
        SRZ R3
        ADI R1,RZR,-1
        SRZ R3
        ADI R2,RZR,-1
        SRM R1,[R0]
        SRM R2,[R0+4]
        ADD SP,R8,RZR
        POP R8
        POP R6
        ADI SP,SP,4
        JMP return_sub
