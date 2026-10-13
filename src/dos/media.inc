; Refresh DOS caches/current paths using the board's per-drive media generation.
; Called after core_select has set SELECT. Preserves R8/R9 and shell registers.
core_media_refresh:
        Const R1,13fe0h
        LRM R1,[R1]
        LRM R0,[R1]
        ADD R0,R0,R0
        ADD R0,R0,R0
        Const R7,0fe0120h
        ADD R7,R7,R0
        LRM R0,[R7]
        LRM R2,[R1+40]
        SUB R2,R0,R2
        SNZ R2
        JMP core_media_same
        SRM R0,[R1+40]
        SRM RZR,[R1+4]
        SRM RZR,[R1+36]
        ADI R0,RZR,92
        SRM R0,[R1+64]
        SRM RZR,[R1+68]
        Const R1,13ff4h
        ADI R0,RZR,-1
        SRM R0,[R1]
        SRM R0,[R1+4]
core_media_same:
        ADI R0,RZR,0
        JMP return_sub

; R8=handle slot; returns R0=1 if current, otherwise expires the slot (R0=0).
; Does not clobber the slot-scan R9 or file frame R13.
file_slot_current:
        LRM R0,[R8+8]
        ADD R0,R0,R0
        ADD R0,R0,R0
        Const R7,0fe0120h
        ADD R7,R7,R0
        LRM R0,[R7]
        LRM R1,[R8+76]
        SUB R0,R0,R1
        SRZ R0
        JMP file_slot_expired
        ADI R0,RZR,1
        JMP return_sub
file_slot_expired:
        SRM RZR,[R8]
        ADI R0,RZR,0
        JMP return_sub
