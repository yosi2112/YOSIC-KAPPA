; Standalone generated runtime. R13=D0400, word 0 is GET's CRLF flag.
; +4 IORESULT, +12 one-byte transfer buffer, +32..95 eight (active,EOF)
; records keyed by DOS handle 1..8, +128..383 OPEN's 64-word path buffer.
; File calls preserve R6/R8..R13; R0..R5/R7 are volatile like other helpers.
; No runtime state or pointer refers back to the compiler's address space.
rt_ioresult:
        LRM R2,[R13+4]
        JMP rt_return
; R2 handle -> R0 status, R3 owned record or zero. No status side effects.
rt_file_check:
        ADI R0,R2,-1
        RtConst R1,80000000h
        ANL R1,R0,R1
        SRZ R1
        JMP rt_file_invalid
        ADI R1,R2,-9
        RtConst R3,80000000h
        ANL R1,R1,R3
        SNZ R1
        JMP rt_file_invalid
        ADD R3,R0,R0
        ADD R3,R3,R3
        ADD R3,R3,R3
        ADI R3,R3,32
        ADD R3,R3,R13
        LRM R0,[R3]
        SNZ R0
        JMP rt_file_invalid
        ADI R0,RZR,0
        JMP rt_return
rt_file_invalid:
        ADI R3,RZR,0
        ADI R0,RZR,6
        JMP rt_return
; R0 status, R9 checked record. Error 6 retires runtime ownership too.
rt_file_status:
        SRM R0,[R13+4]
        ADI R1,R0,-6
        SRZ R1
        JMP rt_file_status_done
        SRZ R9
        SRM RZR,[R9]
rt_file_status_done:
        ADD R2,R0,RZR
        JMP rt_return
rt_fwrite_begin:
        RtCall rt_file_check
        SRM R0,[R13+4]
        ADD R2,R0,RZR
        JMP rt_return
rt_feof:
        RtCall rt_file_check
        SRZ R0
        JMP rt_feof_bad
        LRM R2,[R3+4]
        JMP rt_return
rt_feof_bad:
        SRM R0,[R13+4]
        ADI R2,RZR,0
        JMP rt_return

; R2 literal path (word bytes), R3 mode expression. Validate before OPEN so
; an invalid mode cannot create or truncate any file.
rt_fopen:
        PSH R6
        PSH R8
        PSH R9
        ADD R8,R2,RZR
        ADD R6,R3,RZR
        RtConst R1,80000000h
        ANL R0,R6,R1
        SRZ R0
        JMP rt_fopen_mode_error
        ADI R0,R6,-3
        ANL R0,R0,R1
        SNZ R0
        JMP rt_fopen_mode_error
        ADI R9,R13,128
rt_fopen_copy:
        LRM R0,[R8]
        SRM R0,[R9]
        ADI R8,R8,4
        ADI R9,R9,4
        SRZ R0
        JMP rt_fopen_copy
        ADI R2,R13,128
        ADD R3,R6,RZR
        RtSys DOS_OPEN
        SRM R0,[R13+4]
        SRZ R0
        JMP rt_fopen_failure
        ADI R0,R2,-1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADI R0,R0,32
        ADD R9,R13,R0
        ADI R0,RZR,1
        SRM R0,[R9]
        SRM RZR,[R9+4]
        JMP rt_file_pop
rt_fopen_mode_error:
        ADI R0,RZR,87
        SRM R0,[R13+4]
rt_fopen_failure:
        ADI R2,RZR,0
        JMP rt_file_pop

rt_fclose:
        PSH R6
        PSH R8
        PSH R9
        ADD R8,R2,RZR
        RtCall rt_file_check
        ADD R9,R3,RZR
        SRZ R0
        JMP rt_fclose_result
        ADD R2,R8,RZR
        RtSys DOS_CLOSE
        SRZ R0
        JMP rt_fclose_result
        SRM RZR,[R9]
        SRM RZR,[R9+4]
rt_fclose_result:
        RtCall rt_file_status
        JMP rt_file_pop
rt_fgetc:
        PSH R6
        PSH R8
        PSH R9
        ADD R8,R2,RZR
        RtCall rt_file_check
        ADD R9,R3,RZR
        SRZ R0
        JMP rt_fgetc_error
        ADD R2,R8,RZR
        ADI R3,R13,12
        ADI R4,RZR,1
        RtSys DOS_READ
        SRZ R0
        JMP rt_fgetc_error
        SRM RZR,[R13+4]
        SNZ R2
        JMP rt_fgetc_eof
        LRM R2,[R13+12]
        SRM RZR,[R9+4]
        JMP rt_file_pop
rt_fgetc_eof:
        ADI R0,RZR,1
        SRM R0,[R9+4]
        ADI R2,RZR,-1
        JMP rt_file_pop
rt_fgetc_error:
        RtCall rt_file_status
        ADI R2,RZR,-1
        JMP rt_file_pop
; R2 handle, R3 unsigned byte expression. Check the full signed16 value.
rt_fputc:
        PSH R6
        PSH R8
        PSH R9
        ADD R8,R2,RZR
        ADD R6,R3,RZR
        RtCall rt_file_check
        ADD R9,R3,RZR
        SRZ R0
        JMP rt_fputc_result
        ADI R1,RZR,255
        ANL R1,R6,R1
        SUB R1,R6,R1
        SRZ R1
        JMP rt_fputc_range
        SRM R6,[R13+12]
        ADD R2,R8,RZR
        ADI R3,R13,12
        ADI R4,RZR,1
        RtSys DOS_WRITE
        SRZ R0
        JMP rt_fputc_result
        ADI R1,R2,-1
        SNZ R1
        JMP rt_fputc_ok
        ; A successful short WRITE is still a failed byte emission.
        ADI R0,RZR,112
        JMP rt_fputc_result
rt_fputc_ok:
        SRM RZR,[R9+4]
        JMP rt_fputc_result
rt_fputc_range:
        ADI R0,RZR,87
rt_fputc_result:
        RtCall rt_file_status
rt_file_pop:
        POP R9
        POP R8
        POP R6
        JMP rt_return

; R6 handle, R2 character. Kept separate from console output routines.
rt_filechar:
        ADD R3,R2,RZR
        ADD R2,R6,RZR
        JMP rt_fputc
rt_fputstr:
        PSH R8
        ADD R8,R2,RZR
rt_fputstr_next:
        LRM R2,[R8]
        SNZ R2
        JMP rt_fputstr_end
        RtCall rt_filechar
        SRZ R2
        JMP rt_fputstr_end
        ADI R8,R8,4
        JMP rt_fputstr_next
rt_fputstr_end:
        POP R8
        JMP rt_return
rt_fnewline:
        ADI R2,RZR,13
        RtCall rt_filechar
        SRZ R2
        JMP rt_return
        ADI R2,RZR,10
        JMP rt_filechar
rt_fputint:
        PSH R8
        PSH R9
        PSH R10
        ADD R8,R2,RZR
        ADI R10,RZR,0
        RtConst R0,80000000h
        ANL R0,R8,R0
        SRZ R0
        JMP rt_fputint_negative
        JMP rt_fputint_places
rt_fputint_negative:
        SUB R8,RZR,R8
        ADI R2,RZR,45
        RtCall rt_filechar
        SRZ R2
        JMP rt_fputint_end
rt_fputint_places:
        RtConst R9,80000h+rt_places-rt_base
rt_fputint_place:
        LRM R4,[R9]
        ADI R5,RZR,0
rt_fputint_subtract:
        SUB R0,R8,R4
        RtConst R1,80000000h
        ANL R1,R0,R1
        SRZ R1
        JMP rt_fputint_digit
        ADD R8,R0,RZR
        ADI R5,R5,1
        JMP rt_fputint_subtract
rt_fputint_digit:
        SNZ R5
        JMP rt_fputint_leading
        ADI R10,RZR,1
rt_fputint_leading:
        ADI R0,R4,-1
        SNZ R0
        ADI R10,RZR,1
        SRZ R10
        JMP rt_fputint_emit
        JMP rt_fputint_next
rt_fputint_emit:
        ADI R2,R5,48
        RtCall rt_filechar
        SRZ R2
        JMP rt_fputint_end
rt_fputint_next:
        ADI R9,R9,4
        LRM R0,[R9]
        SRZ R0
        JMP rt_fputint_place
        ADI R2,RZR,0
rt_fputint_end:
        POP R10
        POP R9
        POP R8
        JMP rt_return
; Normal/fatal exit closes owned handles without overwriting IORESULT or the
; caller's registers. DOS EXIT provides the final release if CLOSE fails.
rt_file_cleanup:
        PSH R6
        PSH R8
        PSH R9
        ADI R6,RZR,1
        ADI R9,R13,32
rt_file_cleanup_next:
        LRM R0,[R9]
        SRZ R0
        JMP rt_file_cleanup_close
        JMP rt_file_cleanup_step
rt_file_cleanup_close:
        ADD R2,R6,RZR
        RtSys DOS_CLOSE
        SRM RZR,[R9]
        SRM RZR,[R9+4]
rt_file_cleanup_step:
        ADI R9,R9,8
        ADI R6,R6,1
        ADI R0,R6,-9
        SNZ R0
        JMP rt_file_pop
        JMP rt_file_cleanup_next
