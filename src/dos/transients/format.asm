; FORMAT: native FD physical/FAT12 initialization. Data media, no IPL payload.
        CPU YOSIC-KAPPA
        JMP start
        INCLUDE common.inc
        INCLUDE fdc.inc
        INCLUDE block-device.inc
        INCLUDE format-fs.inc
start:
        TrInit
        TrConst R13,0df000h
        McSet 12,-1
        McSet 16,0
        McSet 20,0
        TrConst R0,0f1000h
        SRM R0,[R13+24]
        TrCall mc_arg
        McFail
        TrConst R2,0dc000h
        McAddr R3,tfmt_help
        TrCall mc_equal
        SNZ R0
        JMP fmt_help
        TrConst R2,0dc000h
        TrCall mc_drive
        McFail
        SRM R2,[R13]
fmt_args:
        TrCall mc_arg
        ADI R1,R0,-1
        SNZ R1
        JMP fmt_detect
        McFail
        TrConst R2,0dc000h
        McAddr R3,tfmt_yes
        TrCall mc_equal
        SNZ R0
        JMP fmt_yes
        TrConst R2,0dc000h
        McAddr R3,tfmt_quick
        TrCall mc_equal
        SNZ R0
        JMP fmt_quick
        TrConst R2,0dc000h
        McAddr R3,tfmt_1440
        TrCall mc_equal
        SNZ R0
        JMP fmt_1440
        TrConst R2,0dc000h
        McAddr R3,tfmt_1232
        TrCall mc_equal
        SNZ R0
        JMP fmt_1232
        TrConst R2,0dc000h
        McAddr R3,tfmt_help
        TrCall mc_equal
        SNZ R0
        JMP fmt_help
        ADI R0,RZR,87
        JMP mc_error
fmt_yes: McSet 16,1
        JMP fmt_args
fmt_quick: McSet 20,1
        JMP fmt_args
fmt_1440: McSet 12,0
        JMP fmt_args
fmt_1232: McSet 12,1
        JMP fmt_args
fmt_help:
        McAddr R2,help
        TrCall tr_puts
        ADI R2,RZR,0
        TrCall tr_exit
fmt_detect:
        TrCall mc_detect
        SRM R0,[R13+28]
        LRM R1,[R13+12]
        ADI R2,R1,1
        SNZ R2
        JMP fmt_need_detect
        LRM R2,[R13+20]
        SRZ R2
        JMP fmt_quick_check
        SRM R1,[R13+8]
        JMP fmt_ready
fmt_quick_check:
        McFail
        LRM R2,[R13+8]
        SUB R0,R1,R2
        SRZ R0
        JMP fmt_profile_error
        JMP fmt_ready
fmt_need_detect:
        McFail
fmt_ready:
        TrCall mc_descriptor
        McFail
        ; Non-destructive READ ID before the confirmation. FORMAT itself checks RO.
        McSet 32,0
        McSet 36,0
        McSet 40,1
        TrConst R0,0da000h
        SRM R0,[R13+44]
        ADI R0,RZR,4
        TrCall mc_request
        SRZ R0
        JMP fmt_probe_failed
        JMP fmt_confirm
fmt_probe_failed:
        ADI R1,R0,-23
        SRZ R1
        JMP mc_error
        ; A readable ID with the other N means incompatible geometry.
        LRM R0,[R13+8]
        ADI R1,RZR,1
        XOR R0,R0,R1
        SRM R0,[R13+8]
        ADI R0,RZR,4
        TrCall mc_request
        PSH R0
        LRM R0,[R13+8]
        ADI R1,RZR,1
        XOR R0,R0,R1
        SRM R0,[R13+8]
        POP R0
        SNZ R0
        JMP fmt_profile_error
        LRM R0,[R13+20]
        SRZ R0
        JMP fmt_profile_error
        LRM R0,[R13+12]
        ADI R0,R0,1
        SNZ R0
        JMP fmt_profile_error
        LRM R0,[R13+20]
        SRZ R0
        JMP fmt_confirm
        ; An explicit /F permits blank media; no assumptions from failed BPB.
fmt_confirm:
        McAddr R2,target
        TrCall tr_puts
        LRM R2,[R13]
        ADI R2,R2,48
        TrCall tr_putchar
        McAddr R2,mc_crlf
        TrCall tr_puts
        TrCall mc_confirm
        McFail
        LRM R0,[R13+20]
        SRZ R0
        JMP fmt_filesystem
fmt_track:
        TrCall mc_cancel
        McFail
        ADI R0,RZR,3
        TrCall mc_request
        McFail
        LRM R0,[R13+36]
        ADI R0,R0,1
        ADI R1,R0,-2
        SRM R0,[R13+36]
        SNZ R1
        JMP fmt_next_cylinder
        JMP fmt_track
fmt_next_cylinder:
        McSet 36,0
        LRM R0,[R13+32]
        ADI R0,R0,1
        SRM R0,[R13+32]
        LRM R1,[R13+8]
        ADI R2,RZR,80
        SRZ R1
        ADI R2,RZR,77
        SUB R1,R0,R2
        SRZ R1
        JMP fmt_track
fmt_filesystem:
        McSet 32,0
        McSet 36,0
        ADI R0,RZR,2
        TrCall mc_request
        McFail
        TrCall mc_descriptor
        McFail
        ; One physical-sector word-byte buffer. Metadata sectors only for /Q.
        McSet 48,0
fmt_metadata_loop:
        TrConst R9,0da000h
        LRM R6,[R8+20]
        ADD R2,R9,RZR
        ADD R3,R6,RZR
fmt_clear:
        SRM RZR,[R2]
        ADI R2,R2,4
        ADI R3,R3,-1
        SRZ R3
        JMP fmt_clear
        LRM R0,[R13+48]
        SNZ R0
        JMP fmt_boot
        ADI R1,R0,-1
        SNZ R1
        JMP fmt_fat
        LRM R1,[R13+8]
        ADI R2,RZR,10
        SRZ R1
        ADI R2,RZR,3
        SUB R1,R0,R2
        SNZ R1
        JMP fmt_fat
        JMP fmt_write
fmt_boot:
        ADI R0,RZR,235
        SRM R0,[R9+0]
        ADI R0,RZR,60
        SRM R0,[R9+4]
        ADI R0,RZR,144
        SRM R0,[R9+8]
        ADI R0,RZR,0
        SRM R0,[R9+44]
        ADI R0,RZR,2
        SRM R0,[R9+48]
        ADI R0,RZR,1
        SRM R0,[R9+52]
        ADI R0,RZR,1
        SRM R0,[R9+56]
        ADI R0,RZR,2
        SRM R0,[R9+64]
        ADI R0,RZR,224
        SRM R0,[R9+68]
        ADI R0,RZR,64
        SRM R0,[R9+76]
        ADI R0,RZR,11
        SRM R0,[R9+80]
        ADI R0,RZR,240
        SRM R0,[R9+84]
        ADI R0,RZR,9
        SRM R0,[R9+88]
        ADI R0,RZR,18
        SRM R0,[R9+96]
        ADI R0,RZR,2
        SRM R0,[R9+104]
        ADI R0,RZR,41
        SRM R0,[R9+152]
        ADI R0,RZR,85
        SRM R0,[R9+2040]
        ADI R0,RZR,170
        SRM R0,[R9+2044]
        ADI R0,RZR,75
        SRM R0,[R9+12]
        ADI R0,RZR,65
        SRM R0,[R9+16]
        ADI R0,RZR,80
        SRM R0,[R9+20]
        ADI R0,RZR,80
        SRM R0,[R9+24]
        ADI R0,RZR,65
        SRM R0,[R9+28]
        ADI R0,RZR,32
        SRM R0,[R9+32]
        ADI R0,RZR,32
        SRM R0,[R9+36]
        ADI R0,RZR,32
        SRM R0,[R9+40]
        LRM R0,[R13+8]
        SRZ R0
        JMP fmt_boot_1232
        JMP fmt_write
fmt_boot_1232:
        ADI R0,RZR,4
        SRM R0,[R9+48]
        ADI R0,RZR,192
        SRM R0,[R9+68]
        ADI R0,RZR,208
        SRM R0,[R9+76]
        ADI R0,RZR,4
        SRM R0,[R9+80]
        ADI R0,RZR,254
        SRM R0,[R9+84]
        ADI R0,RZR,2
        SRM R0,[R9+88]
        ADI R0,RZR,8
        SRM R0,[R9+96]
        JMP fmt_write
fmt_fat:
        LRM R0,[R13+8]
        ADI R1,RZR,240
        SRZ R0
        ADI R1,RZR,254
        SRM R1,[R9]
        ADI R1,RZR,255
        SRM R1,[R9+4]
        SRM R1,[R9+8]
fmt_write:
        LRM R0,[R13+48]
        SRM R0,[R8+32]
        ADI R0,RZR,1
        SRM R0,[R8+40]
        ADD R2,R8,RZR
        TrCall bd_transfer
        McFail
        LRM R0,[R13+48]
        ADI R0,R0,1
        SRM R0,[R13+48]
        LRM R1,[R13+8]
        ADI R2,RZR,33
        SRZ R1
        ADI R2,RZR,11
        SUB R0,R0,R2
        SRZ R0
        JMP fmt_metadata_loop
        JMP mc_success
fmt_profile_error:
        ADI R0,RZR,23
        JMP mc_error
tfmt_yes: DD 47,89,0
tfmt_quick: DD 47,81,0
tfmt_1440: DD 47,70,58,49,52,52,48,0
tfmt_1232: DD 47,70,58,49,50,51,50,0
tfmt_help: DD 47,63,0
target: DD 70,79,82,77,65,84,32,116,97,114,103,101,116,32,0
help: DD 70,79,82,77,65,84,32,100,114,105,118,101,58,32,91,47,70,58,49,52,52,48,124,49,50,51,50,93,32,91,47,81,93,32,91,47,89,93,13,10,0
        END
