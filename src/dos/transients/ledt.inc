; D0000..DBFFF: 12288 record words; DC000..DDBFF: 1791 indices + end.
; DDC00..DDFFF: pending source line; DE000..DE7FF: input cache (512).
; DE800..DEFFF: output / transfer byte buffer (512).
; DF000..DF1FF: command (127); DF200..DF5FF: edit line (254 + NUL).
; DF600/700/800: original/BAK/temp paths (63 bytes + NUL each).
; DF900..DFAFF: MOVE command; DFB00..DFCFF: last search (127 + NUL).
; DFD00..DFFFF: state. E0000..EFFFF is the balanced application stack.
ED_TEXT EQU 0d0000h
ED_INDEX EQU 0dc000h
ED_PENDING EQU 0ddc00h
ED_INPUT EQU 0de000h
ED_OUTPUT EQU 0de800h
ED_CMD_BUFFER EQU 0df000h
ED_LINE EQU 0df200h
ED_PATH EQU 0df600h
ED_BAK EQU 0df700h
ED_TEMP EQU 0df800h
ED_MOVE EQU 0df900h
ED_SEARCH EQU 0dfb00h
ed_used EQU 0
ed_count EQU 4
ed_current EQU 8
ed_in EQU 12
ed_out EQU 16
ed_binary EQU 20
ed_existed EQU 24
ed_owned EQU 28
ed_eof EQU 32
ed_cache_count EQU 36
ed_cache_ptr EQU 40
ed_skip_lf EQU 44
ed_pending_len EQU 48
ed_read_error EQU 52
ed_write_error EQU 56
ed_a EQU 60
ed_b EQU 64
ed_c EQU 68
ed_repeat EQU 72
ed_fields EQU 76
ed_op EQU 80
ed_confirm EQU 84
ed_tail EQU 88
ed_line_len EQU 92
ed_limit EQU 96
ed_num_read EQU 100
ed_transfer EQU 104
ed_transfer_in EQU 108
ed_transfer_at_end EQU 112
ed_transfer_skip EQU 116
ed_find_len EQU 120
ed_replace_ptr EQU 124
ed_replace_len EQU 128
ed_found EQU 132
ed_scan_line EQU 136
ed_scan_pos EQU 140
ed_target EQU 144
ed_console_lf EQU 148
ed_save_stage EQU 152
ed_editing EQU 156
ed_replace_old EQU 160
ed_transfer_cached EQU 164
ed_transfer_pointer EQU 168

EdAddr MACRO reg,label
        TrConst reg,80000h+label
        ENDM
EdGet MACRO reg,field
        LRM reg,[R13+field]
        ENDM
EdPut MACRO reg,field
        SRM reg,[R13+field]
        ENDM
EdSet MACRO field,value
        ADI R0,RZR,value
        EdPut R0,field
        ENDM
; Comparisons destroy R0/R1 only. Operands must not be R0/R1.
EdEq MACRO reg,value,target
        TrConst R0,value
        SUB R0,reg,R0
        SNZ R0
        JMP target
        ENDM
EdNe MACRO reg,value,target
        TrConst R0,value
        SUB R0,reg,R0
        SRZ R0
        JMP target
        ENDM
EdLt MACRO left,right,target
        SUB R0,left,right
        TrConst R1,80000000h
        ANL R0,R0,R1
        SRZ R0
        JMP target
        ENDM
EdBegin MACRO
        PSH R6
        PSH R8
        PSH R9
        PSH R10
        PSH R11
        ENDM
EdCall MACRO target
        TrCall target
        SRZ R0
        JMP ed_return
        ENDM
EdSay MACRO label
        EdAddr R2,label
        TrCall tr_puts
        ENDM
ed_return:
        POP R11
        POP R10
        POP R9
        POP R8
        POP R6
        JMP tr_return
ed_ok:
        ADI R0,RZR,0
        JMP ed_return
ed_bad:
        ADI R0,RZR,87
        JMP ed_return
ed_full:
        ADI R0,RZR,8
        JMP ed_return

ed_start:
        TrInit
        TrConst R13,0dfd00h
        ADD R8,R13,RZR
        ADI R9,RZR,192
ed_zero:
        SRM RZR,[R8]
        ADI R8,R8,4
        ADI R9,R9,-1
        SRZ R9
        JMP ed_zero
        EdSet ed_current,1
        EdSet ed_pending_len,-1
        TrCall ed_reindex
        TrCall ed_paths
        SRZ R0
        JMP ed_fatal
        TrConst R2,ED_PATH
        ADI R3,RZR,0
        TrCall tr_open
        SRZ R0
        JMP ed_new_check
        EdPut R2,ed_in
        EdSet ed_existed,1
        JMP ed_initial_read
ed_new_check:
        ADD R8,R0,RZR
        EdNe R8,3,ed_fatal_code
        EdSet ed_eof,1
        EdSay ed_new_message
ed_initial_read:
        EdSet ed_limit,0
        TrCall ed_append
        SRZ R0
        JMP ed_report
        JMP ed_prompt
ed_fatal_code:
        ADD R0,R8,RZR
ed_fatal:
        PSH R0
        TrCall ed_close_all
        EdSay ed_error_message
        POP R2
        PSH R2
        TrCall tr_error
        EdSay ed_nl
        POP R2
        TrCall tr_exit
ed_prompt:
        EdSay ed_star
        TrConst R2,ED_CMD_BUFFER
        ADI R4,RZR,128
        TrCall ed_readline
        SRZ R0
        JMP ed_report
        TrCall ed_command
        SRZ R0
        JMP ed_report
        JMP ed_prompt
ed_report:
        PSH R0
        EdSay ed_error_message
        POP R2
        TrCall tr_error
        EdSay ed_nl
        JMP ed_prompt

; Console input supports CR, LF, CRLF, backspace, literal Ctrl-Z and bytes.
; R2=destination, R4=capacity including NUL. Overflow drains without mutation.
ed_readline:
        EdBegin
        ADD R8,R2,RZR
        ADD R9,R4,RZR
        ADI R6,RZR,0
        ADI R10,RZR,0
ed_key:
        TrCall tr_getchar
        EdEq R2,-1,ed_key
        EdGet R3,ed_console_lf
        EdSet ed_console_lf,0
        EdEq R3,0,ed_key_test
        EdEq R2,10,ed_key
ed_key_test:
        EdEq R2,3,ed_key_cancel
        EdEq R2,13,ed_key_cr
        EdEq R2,10,ed_key_done
        EdEq R2,8,ed_key_back
        EdNe R10,0,ed_key
        ADI R3,R6,1
        EdLt R3,R9,ed_key_store
        ADI R10,RZR,87
        JMP ed_key
ed_key_store:
        SRM R2,[R8]
        ADI R8,R8,4
        ADI R6,R6,1
        TrCall tr_putchar
        JMP ed_key
ed_key_back:
        EdNe R10,0,ed_key
        EdEq R6,0,ed_key
        ADI R8,R8,-4
        ADI R6,R6,-1
        TrCall tr_putchar
        ADI R2,RZR,32
        TrCall tr_putchar
        ADI R2,RZR,8
        TrCall tr_putchar
        JMP ed_key
ed_key_cancel:
        ADI R10,RZR,995
        JMP ed_key_done
ed_key_cr:
        EdSet ed_console_lf,1
ed_key_done:
        SRM RZR,[R8]
        EdSay ed_nl
        ADD R2,R6,RZR
        ADD R0,R10,RZR
        JMP ed_return

; R2=line, result R2=record (also valid for count+1 sentinel).
ed_ptr:
        ADI R2,R2,-1
        ADD R2,R2,R2
        ADD R2,R2,R2
        TrConst R0,ED_INDEX
        ADD R2,R2,R0
        LRM R2,[R2]
        JMP tr_return
ed_reindex:
        EdBegin
        TrConst R8,ED_TEXT
        TrConst R9,ED_INDEX
        EdGet R10,ed_used
        ADD R10,R10,R10
        ADD R10,R10,R10
        ADD R10,R8,R10
        ADI R6,RZR,0
ed_index_next:
        SRM R8,[R9]
        ADI R9,R9,4
        EdLt R8,R10,ed_index_record
        EdPut R6,ed_count
        JMP ed_ok
ed_index_record:
        LRM R0,[R8]
        ADI R0,R0,1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R8,R8,R0
        ADI R6,R6,1
        JMP ed_index_next

; Overlap-safe word move, R2=destination, R3=source, R4=words.
ed_memmove:
        EdBegin
        ADD R8,R2,RZR
        ADD R9,R3,RZR
        ADD R6,R4,RZR
        EdLt R8,R9,ed_move_forward
        ADD R0,R6,R6
        ADD R0,R0,R0
        ADD R8,R8,R0
        ADD R9,R9,R0
ed_move_back:
        EdEq R6,0,ed_ok
        ADI R8,R8,-4
        ADI R9,R9,-4
        LRM R0,[R9]
        SRM R0,[R8]
        ADI R6,R6,-1
        JMP ed_move_back
ed_move_forward:
        EdEq R6,0,ed_ok
        LRM R0,[R9]
        SRM R0,[R8]
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R6,R6,-1
        JMP ed_move_forward

; Insert ED_LINE/ed_line_len before R2. Capacity checked before text writes.
ed_insert:
        EdBegin
        ADD R11,R2,RZR
        EdGet R8,ed_line_len
        ADI R8,R8,1
        EdGet R9,ed_used
        ADD R9,R9,R8
        TrConst R3,12289
        EdLt R9,R3,ed_insert_index
        JMP ed_full
ed_insert_index:
        EdGet R3,ed_count
        TrConst R4,1791
        EdLt R3,R4,ed_insert_space
        JMP ed_full
ed_insert_space:
        TrCall ed_ptr
        ADD R10,R2,RZR
        EdGet R4,ed_used
        ADD R4,R4,R4
        ADD R4,R4,R4
        TrConst R0,ED_TEXT
        ADD R4,R4,R0
        SUB R4,R4,R10
        ADD R0,R8,R8
        ADD R0,R0,R0
        ADD R2,R10,R0
        ADD R3,R10,RZR
        ; bytes -> words by division (no shift instruction dependency).
        PSH R2
        PSH R3
        ADD R0,R4,RZR
        ADI R1,RZR,4
        TrCall tr_div
        ADD R4,R0,RZR
        POP R3
        POP R2
        TrCall ed_memmove
        ADI R0,R8,-1
        SRM R0,[R10]
        ADI R2,R10,4
        TrConst R3,ED_LINE
        ADI R4,R8,-1
        TrCall ed_memmove
        EdPut R9,ed_used
        EdPut R11,ed_current
        TrCall ed_reindex
        JMP ed_ok

; Delete inclusive ed_a..ed_b (already validated), keep next line current.
ed_delete:
        EdBegin
        EdGet R2,ed_a
        TrCall ed_ptr
        ADD R8,R2,RZR
        EdGet R2,ed_b
        ADI R2,R2,1
        TrCall ed_ptr
        ADD R9,R2,RZR
        EdGet R2,ed_count
        ADI R2,R2,1
        TrCall ed_ptr
        SUB R0,R2,R9
        ADI R1,RZR,4
        TrCall tr_div
        ADD R4,R0,RZR
        ADD R2,R8,RZR
        ADD R3,R9,RZR
        TrCall ed_memmove
        SUB R0,R9,R8
        ADI R1,RZR,4
        TrCall tr_div
        EdGet R2,ed_used
        SUB R2,R2,R0
        EdPut R2,ed_used
        TrCall ed_reindex
        EdGet R2,ed_a
        EdGet R3,ed_count
        EdLt R3,R2,ed_delete_last
        JMP ed_delete_current
ed_delete_last:
        ADD R2,R3,RZR
        EdNe R2,0,ed_delete_current
        ADI R2,RZR,1
ed_delete_current:
        EdPut R2,ed_current
        JMP ed_ok

ed_star: DD 42,0
ed_nl: DD 13,10,0
ed_new_message: DD 78,101,119,32,102,105,108,101,13,10,0
ed_error_message: DD 76,69,68,84,32,101,114,114,111,114,32,0
