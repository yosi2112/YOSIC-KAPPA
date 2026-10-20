; CP/M-style directory checksum vector. No host media identity or exchange lock.
; State+40: sticky software read-only; state+44: checksum vector initialized.
; CSV: 50800h..50FFFh (128 word checksums/drive); scratch sector: 44800h.
; Checksums cover each 128-byte record of the FAT12 root directory.
core_media_check:
        PSH R8
        ADD R8,SP,RZR
        ADI SP,SP,-24
        Const R7,13fe0h
        LRM R7,[R7]
        SRM R7,[R8-4]
        LRM R0,[R7+4]
        SNZ R0
        JMP media_check_ok
        LRM R0,[R7+44]
        SRM R0,[R8-20]
        LRM R0,[R7]
        SRM R0,[R8-24]
        LRM R0,[R7+24]
        ADI R1,RZR,16
        Call core_div
        SRM R0,[R8-12]
        LRM R7,[R8-4]
        ADI R0,R7,2048
        SRM R0,[R8-16]
        SRM RZR,[R8-8]
media_check_loop:
        LRM R0,[R8-8]
        LRM R1,[R8-12]
        SUB R1,R0,R1
        SNZ R1
        JMP media_check_done
        LRM R7,[R8-4]
        LRM R1,[R7+20]
        ADD R0,R0,R1
        PSH R0
        Const R0,44800h
        PSH R0
        LRM R0,[R8-24]
        PSH R0
        PSH RZR
        Call core_block
        ADI SP,SP,16
        SRZ R0
        JMP media_check_return
        LRM R1,[R8-8]
        REPT 4
        ADD R1,R1,R1
        ENDM
        LRM R0,[R8-16]
        ADD R1,R1,R0
        Const R0,44800h
        LRM R6,[R8-20]
        LRM R7,[R8-4]
        Call media_checksums
        LRM R0,[R8-8]
        ADI R0,R0,1
        SRM R0,[R8-8]
        JMP media_check_loop
media_check_done:
        LRM R7,[R8-4]
        ADI R0,RZR,1
        SRM R0,[R7+44]
media_check_ok:
        ADI R0,RZR,0
media_check_return:
        ADD SP,R8,RZR
        POP R8
        JMP return_sub

; Successful root-directory writes update the CSV to avoid false media changes.
; R0=LBA, R1=word-byte buffer, R7=drive state. Preserves R8/R9.
core_media_written:
        PSH R3
        PSH R4
        PSH R9
        ADD R4,R1,RZR
        LRM R1,[R7+44]
        SNZ R1
        JMP media_written_done
        LRM R1,[R7+20]
        SUB R3,R0,R1
        Const R2,80000000h
        ANL R1,R3,R2
        SRZ R1
        JMP media_written_done
        LRM R0,[R7+24]
        ADI R1,RZR,16
        PSH R3
        PSH R4
        PSH R7
        Call core_div
        POP R7
        POP R4
        POP R3
        SUB R1,R3,R0
        Const R2,80000000h
        ANL R1,R1,R2
        SNZ R1
        JMP media_written_done
        REPT 4
        ADD R3,R3,R3
        ENDM
        ADI R1,R7,2048
        ADD R1,R1,R3
        ADD R0,R4,RZR
        ADI R6,RZR,0
        Call media_checksums
media_written_done:
        POP R9
        POP R4
        POP R3
        ADI R0,RZR,0
        JMP return_sub

; R0=buffer, R1=CSV, R6=0 establish/update or 1 compare, R7=state.
; CP/M-style 8-bit sum for each of four 128-byte directory records.
media_checksums:
        PSH R3
        PSH R4
        PSH R5
        PSH R6
        PSH R8
        PSH R9
        ADD R2,R6,RZR
        ADD R4,R0,RZR
        ADD R5,R1,RZR
        ADI R3,RZR,4
media_checksum_record:
        ADI R6,RZR,128
        ADI R8,RZR,0
media_checksum_byte:
        LRM R0,[R4]
        ADD R8,R8,R0
        ADI R4,R4,4
        ADI R6,R6,-1
        SRZ R6
        JMP media_checksum_byte
        ADI R0,RZR,255
        ANL R8,R8,R0
        SNZ R2
        JMP media_checksum_store
        LRM R9,[R5]
        SUB R9,R8,R9
        SNZ R9
        JMP media_checksum_next
        ADI R0,RZR,1
        SRM R0,[R7+40]
        Const R0,13ff8h
        ADI R1,RZR,-1
        SRM R1,[R0]
        JMP media_checksum_next
media_checksum_store:
        SRM R8,[R5]
media_checksum_next:
        ADI R5,R5,4
        ADI R3,R3,-1
        SRZ R3
        JMP media_checksum_record
        POP R9
        POP R8
        POP R6
        POP R5
        POP R4
        POP R3
        ADI R0,RZR,0
        JMP return_sub
