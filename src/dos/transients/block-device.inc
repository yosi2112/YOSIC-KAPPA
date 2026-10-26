; Internal block-device boundary (static library, not a public syscall ABI).
; R2 -> 128-byte descriptor in D0000h..DFF7Fh; request at +64.
; +0 kind (0 FD; all others unsupported), +4 unit, +8 profile,
; +12/+16 capacity in physical sectors (uint64 lo/hi), +20 physical bytes,
; +24 logical bytes (512), +28 capabilities (read/write/format = 7).
; +32/+36 requested physical LBA (uint64), +40 op (0 read / 1 write),
; +44 word-byte buffer. HD kinds return 50 before any board access.
; bd_open: input kind/unit/profile already set. bd_transfer uses +32..44.
; bd_identify: READ ID then BPB validation, using the +44 scratch buffer.
bd_open:
        PSH R8
        ADD R8,R2,RZR
        TrCall bd_validate
        SRZ R0
        JMP bd_open_return
        LRM R0,[R8+8]
        ADI R1,RZR,2880
        ADI R3,RZR,512
        SRZ R0
        ADI R1,RZR,1232
        SRZ R0
        ADI R3,RZR,1024
        SRM R1,[R8+12]
        SRM RZR,[R8+16]
        SRM R3,[R8+20]
        ADI R1,RZR,512
        SRM R1,[R8+24]
        ADI R1,RZR,7
        SRM R1,[R8+28]
        ADI R0,RZR,0
bd_open_return:
        POP R8
        JMP tr_return
bd_validate:
        TrConst R1,80000000h
        TrConst R0,0d0000h
        SUB R0,R8,R0
        ANL R0,R0,R1
        SRZ R0
        JMP bd_bad
        TrConst R0,0dff81h
        SUB R0,R8,R0
        ANL R0,R0,R1
        SNZ R0
        JMP bd_bad
        ADI R0,RZR,3
        ANL R0,R8,R0
        SRZ R0
        JMP bd_bad
        LRM R0,[R8]
        SRZ R0
        JMP bd_unsupported
        LRM R0,[R8+4]
        ADI R3,RZR,3
        ANL R3,R0,R3
        SUB R0,R0,R3
        SRZ R0
        JMP bd_bad
        LRM R0,[R8+8]
        ADI R3,RZR,1
        ANL R3,R0,R3
        SUB R0,R0,R3
        SRZ R0
        JMP bd_bad
        JMP tr_return
bd_transfer:
        PSH R8
        ADD R8,R2,RZR
        TrCall bd_open
        SRZ R0
        JMP bd_open_return
        LRM R0,[R8+36]
        SRZ R0
        JMP bd_transfer_bad
        LRM R0,[R8+32]
        TrConst R1,80000000h
        ANL R2,R0,R1
        SRZ R2
        JMP bd_transfer_bad
        LRM R2,[R8+12]
        SUB R2,R0,R2
        ANL R2,R2,R1
        SNZ R2
        JMP bd_transfer_bad
        PSH R0
        LRM R1,[R8+44]
        ADI R2,RZR,3
        ANL R2,R1,R2
        SRZ R2
        JMP bd_buffer_bad
        LRM R2,[R8+20]
        ADD R2,R2,R2
        ADD R2,R2,R2
        ADD R2,R2,R1
        SUB R2,R8,R2
        TrConst R3,80000000h
        ANL R2,R2,R3
        SRZ R2
        JMP bd_overlap
        JMP bd_buffer_ok
bd_overlap:
        ADI R2,R8,128
        SUB R2,R1,R2
        ANL R2,R2,R3
        SRZ R2
        JMP bd_buffer_bad
bd_buffer_ok:
        POP R0
        LRM R1,[R8+40]
        ADI R2,RZR,1
        ANL R2,R1,R2
        SUB R2,R1,R2
        SRZ R2
        JMP bd_transfer_bad
        SRM R1,[R8+64]
        LRM R1,[R8+4]
        SRM R1,[R8+68]
        LRM R1,[R8+8]
        SRM R1,[R8+84]
        ADI R2,RZR,18
        SRZ R1
        ADI R2,RZR,8
        ADD R1,R2,R2
        PSH R2
        TrCall tr_div
        SRM R0,[R8+72]
        ADD R0,R1,RZR
        POP R1
        TrCall tr_div
        SRM R0,[R8+76]
        ADI R1,R1,1
        SRM R1,[R8+80]
        LRM R0,[R8+44]
        SRM R0,[R8+88]
        ADI R2,R8,64
        TrCall fdc_request
        JMP bd_open_return
bd_buffer_bad:
        ADI SP,SP,4
bd_transfer_bad:
        ADI R0,RZR,87
        JMP bd_open_return
bd_bad:
        ADI R0,RZR,87
        JMP tr_return
bd_unsupported:
        ADI R0,RZR,50
        JMP tr_return
bd_identify:
        PSH R8
        PSH R9
        ADD R8,R2,RZR
        TrCall bd_open
        SRZ R0
        JMP bd_identify_return
        ADI R0,RZR,4
        SRM R0,[R8+64]
        LRM R0,[R8+4]
        SRM R0,[R8+68]
        SRM RZR,[R8+72]
        SRM RZR,[R8+76]
        ADI R0,RZR,1
        SRM R0,[R8+80]
        LRM R0,[R8+8]
        SRM R0,[R8+84]
        LRM R0,[R8+44]
        SRM R0,[R8+88]
        ADI R2,R8,64
        TrCall fdc_request
        SRZ R0
        JMP bd_identify_return
        SRM RZR,[R8+32]
        SRM RZR,[R8+36]
        SRM RZR,[R8+40]
        ADD R2,R8,RZR
        TrCall bd_transfer
        SRZ R0
        JMP bd_identify_return
        LRM R9,[R8+44]
        LRM R0,[R9+44]
        LRM R1,[R9+48]
        REPT 8
        ADD R1,R1,R1
        ENDM
        ADD R0,R0,R1
        LRM R1,[R8+20]
        SUB R0,R0,R1
        SRZ R0
        JMP bd_identify_bad
        LRM R0,[R8+8]
        ADI R1,RZR,18
        SRZ R0
        ADI R1,RZR,8
        LRM R0,[R9+96]
        SUB R0,R0,R1
        LRM R1,[R9+100]
        ORR R0,R0,R1
        LRM R1,[R9+104]
        ADI R1,R1,-2
        ORR R0,R0,R1
        LRM R1,[R9+108]
        ORR R0,R0,R1
        SRZ R0
        JMP bd_identify_bad
        LRM R0,[R9+76]
        LRM R1,[R9+80]
        REPT 8
        ADD R1,R1,R1
        ENDM
        ADD R0,R0,R1
        LRM R1,[R8+12]
        SUB R0,R0,R1
        SRZ R0
        JMP bd_identify_bad
        JMP bd_identify_return
bd_identify_bad:
        ADI R0,RZR,23
bd_identify_return:
        POP R9
        POP R8
        JMP tr_return
