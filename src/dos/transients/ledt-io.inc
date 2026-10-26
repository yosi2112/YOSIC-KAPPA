; Path handling and streaming I/O. Every file is a normal public DOS handle.
ed_paths:
        EdBegin
        TrConst R2,0f1000h
        TrConst R3,ED_PATH
        ADI R4,RZR,64
        EdCall tr_arg
        ADD R8,R2,RZR
        EdEq R3,0,ed_bad
        TrConst R3,ED_TEMP
        ADI R4,RZR,64
        TrCall tr_arg
        ADD R8,R2,RZR
        ADD R9,R0,RZR
        EdEq R9,1,ed_paths_names
        ADD R0,R9,RZR
        SRZ R0
        JMP ed_return
        TrConst R9,ED_TEMP
        LRM R3,[R9]
        EdNe R3,47,ed_bad
        LRM R2,[R9+4]
        TrCall ed_upper
        EdNe R2,66,ed_bad
        LRM R3,[R9+8]
        EdNe R3,0,ed_bad
        EdSet ed_binary,1
        ; No third token is permitted.
        ADD R2,R8,RZR
        TrConst R3,ED_TEMP
        ADI R4,RZR,64
        TrCall tr_arg
        ADD R9,R0,RZR
        EdNe R9,1,ed_bad
ed_paths_names:
        TrConst R8,ED_PATH
        ADD R10,R8,RZR
        ADI R11,RZR,0
ed_path_scan:
        LRM R2,[R8]
        EdEq R2,0,ed_path_end
        TrCall ed_upper
        SRM R2,[R8]
        EdEq R2,32,ed_bad
        EdEq R2,9,ed_bad
        EdEq R2,42,ed_bad
        EdEq R2,63,ed_bad
        EdEq R2,47,ed_path_slash
        EdEq R2,92,ed_path_leaf
        EdEq R2,58,ed_path_leaf
        EdNe R2,46,ed_path_next
        ADD R11,R8,RZR
        JMP ed_path_next
ed_path_slash:
        ADI R2,RZR,92
        SRM R2,[R8]
ed_path_leaf:
        ADI R10,R8,4
        ADI R11,RZR,0
ed_path_next:
        ADI R8,R8,4
        JMP ed_path_scan
ed_path_end:
        EdLt R10,R8,ed_path_has_leaf
        JMP ed_bad
ed_path_has_leaf:
        EdNe R11,0,ed_path_stem
        ADD R11,R8,RZR
ed_path_stem:
        TrConst R9,ED_PATH
        SUB R0,R11,R9
        ADI R1,RZR,4
        TrCall tr_div
        ADD R6,R0,RZR
        ADI R3,RZR,60
        EdLt R6,R3,ed_path_copy_names
        JMP ed_bad
ed_path_copy_names:
        TrConst R2,ED_BAK
        TrConst R3,ED_PATH
        ADD R4,R6,RZR
        TrCall ed_memmove
        TrConst R2,ED_TEMP
        TrConst R3,ED_PATH
        ADD R4,R6,RZR
        TrCall ed_memmove
        ADD R6,R6,R6
        ADD R6,R6,R6
        TrConst R2,ED_BAK
        ADD R2,R2,R6
        EdAddr R3,ed_bak_suffix
        ADI R4,RZR,5
        TrCall ed_memmove
        TrConst R2,ED_TEMP
        ADD R2,R2,R6
        EdAddr R3,ed_temp_suffix
        ADI R4,RZR,5
        TrCall ed_memmove
        TrConst R2,ED_PATH
        TrConst R3,ED_BAK
        TrCall ed_equal
        EdEq R2,1,ed_bad
        TrConst R2,ED_PATH
        TrConst R3,ED_TEMP
        TrCall ed_equal
        EdEq R2,1,ed_bad
        JMP ed_ok
ed_upper:
        ADI R3,RZR,97
        EdLt R2,R3,ed_upper_done
        ADI R3,RZR,123
        EdLt R2,R3,ed_upper_change
        JMP ed_upper_done
ed_upper_change:
        ADI R2,R2,-32
ed_upper_done:
        JMP tr_return
ed_equal:
        LRM R4,[R2]
        LRM R5,[R3]
        SUB R0,R4,R5
        SRZ R0
        JMP ed_equal_no
        EdEq R4,0,ed_equal_yes
        ADI R2,R2,4
        ADI R3,R3,4
        JMP ed_equal
ed_equal_yes:
        ADI R2,RZR,1
        JMP tr_return
ed_equal_no:
        ADI R2,RZR,0
        JMP tr_return

ed_append_command:
        EdGet R2,ed_a
        EdPut R2,ed_limit
        TrCall ed_append
        JMP ed_return
ed_append:
        EdBegin
        EdGet R8,ed_limit
ed_append_next:
        EdGet R2,ed_read_error
        EdNe R2,0,ed_io_saved_error
        EdGet R9,ed_pending_len
        EdNe R9,-1,ed_append_pending
        EdGet R2,ed_eof
        EdNe R2,0,ed_append_eof
        EdCall ed_source_line
        EdGet R9,ed_pending_len
        EdEq R9,-1,ed_append_eof
ed_append_pending:
        EdGet R10,ed_used
        ADD R10,R10,R9
        ADI R10,R10,1
        TrConst R3,9217
        EdLt R10,R3,ed_append_index
        JMP ed_ok
ed_append_index:
        EdGet R2,ed_count
        ADI R3,RZR,1344
        EdLt R2,R3,ed_append_insert
        JMP ed_ok
ed_append_insert:
        ADI R11,R2,1
        TrConst R2,ED_LINE
        TrConst R3,ED_PENDING
        ADD R4,R9,RZR
        TrCall ed_memmove
        EdPut R9,ed_line_len
        EdGet R6,ed_current
        ADD R2,R11,RZR
        EdCall ed_insert
        EdPut R6,ed_current
        EdSet ed_pending_len,-1
        EdEq R8,0,ed_append_next
        ADI R8,R8,-1
        EdNe R8,0,ed_append_next
        JMP ed_ok
ed_append_eof:
        EdSay ed_eof_message
        JMP ed_ok
ed_io_saved_error:
        ADD R0,R2,RZR
        JMP ed_return

; Read main stream a complete line, leaving it pending until A/E consumes it.
; On an I/O or overlong-line error E is disabled; Q leaves the source intact.
ed_source_line:
        EdBegin
        TrConst R8,ED_PENDING
        ADI R9,RZR,0
ed_source_byte:
        TrCall ed_getbyte
        SRZ R0
        JMP ed_source_error
        EdEq R2,-1,ed_source_eof
        EdGet R3,ed_skip_lf
        EdSet ed_skip_lf,0
        EdEq R3,0,ed_source_test
        EdEq R2,10,ed_source_byte
ed_source_test:
        EdEq R2,13,ed_source_cr
        EdEq R2,10,ed_source_done
        ADI R3,RZR,254
        EdLt R9,R3,ed_source_store
        ADI R0,RZR,87
        JMP ed_source_error
ed_source_store:
        SRM R2,[R8]
        ADI R8,R8,4
        ADI R9,R9,1
        JMP ed_source_byte
ed_source_cr:
        EdSet ed_skip_lf,1
        JMP ed_source_done
ed_source_eof:
        EdEq R9,0,ed_ok
ed_source_done:
        EdPut R9,ed_pending_len
        JMP ed_ok
ed_source_error:
        EdPut R0,ed_read_error
        JMP ed_return
ed_getbyte:
        EdBegin
        EdGet R2,ed_eof
        EdNe R2,0,ed_getbyte_eof
        EdGet R8,ed_cache_count
        EdNe R8,0,ed_getbyte_cached
        EdGet R2,ed_in
        TrConst R3,ED_INPUT
        ADI R4,RZR,512
        EdCall tr_read
        EdEq R2,0,ed_getbyte_eof
        ADD R8,R2,RZR
        TrConst R2,ED_INPUT
        EdPut R2,ed_cache_ptr
ed_getbyte_cached:
        EdGet R3,ed_cache_ptr
        LRM R2,[R3]
        ADI R3,R3,4
        EdPut R3,ed_cache_ptr
        ADI R8,R8,-1
        EdPut R8,ed_cache_count
        EdNe R2,26,ed_ok
        EdGet R3,ed_binary
        EdNe R3,0,ed_ok
ed_getbyte_eof:
        EdSet ed_eof,1
        ADI R2,RZR,-1
        JMP ed_ok

; T reads into the unused text tail, then rotates it before the destination.
; Until complete, used/count/current remain unchanged. Failure rolls it back.
ed_transfer_command:
        EdGet R2,ed_fields
        EdNe R2,1,ed_bad
        EdGet R2,ed_confirm
        EdNe R2,0,ed_bad
        EdCall ed_single
        EdGet R8,ed_tail
        TrCall ed_spaces
        LRM R2,[R8]
        EdEq R2,0,ed_bad
        ADD R2,R8,RZR
        ADI R3,RZR,0
        EdCall tr_open
        EdPut R2,ed_transfer_in
        EdGet R8,ed_used
        EdGet R9,ed_count
        EdGet R10,ed_current
        EdGet R11,ed_a
        EdSet ed_transfer_skip,0
        EdSet ed_transfer_at_end,0
        EdSet ed_transfer_cached,0
        TrCall ed_transfer_read
        PSH R0
        EdGet R2,ed_transfer_in
        TrCall tr_close
        ADD R6,R0,RZR
        EdSet ed_transfer_in,0
        POP R0
        SNZ R0
        ADD R0,R6,RZR
        SRZ R0
        JMP ed_transfer_rollback
        EdGet R2,ed_count
        SUB R6,R2,R9
        EdEq R6,0,ed_ok
        ; Reuse in-place M rotation with appended lines as source.
        ADI R9,R9,1
        EdPut R9,ed_a
        EdPut R2,ed_b
        EdPut R11,ed_c
        ADD R8,R9,RZR
        ADD R9,R2,RZR
        ADD R10,R11,RZR
        EdEq R8,0,ed_bad
        EdEq R10,0,ed_bad
        SUB R0,R10,R8
        SNZ R0
        JMP ed_transfer_same
        JMP ed_move_lines
ed_transfer_same:
        EdPut R10,ed_current
        JMP ed_ok
ed_transfer_rollback:
        EdPut R8,ed_used
        EdPut R9,ed_count
        EdPut R10,ed_current
        PSH R0
        TrCall ed_reindex
        POP R0
        JMP ed_return
ed_transfer_read:
        EdBegin
        TrConst R8,ED_LINE
        ADI R9,RZR,0
ed_transfer_byte:
        EdGet R2,ed_transfer_at_end
        EdNe R2,0,ed_transfer_eof_reached
        EdCall ed_transfer_getbyte
        EdEq R2,-1,ed_transfer_eof_reached
        EdNe R2,26,ed_transfer_skip_lf
        EdGet R3,ed_binary
        EdEq R3,0,ed_transfer_eof_reached
ed_transfer_skip_lf:
        EdGet R3,ed_transfer_skip
        EdSet ed_transfer_skip,0
        EdEq R3,0,ed_transfer_test
        EdEq R2,10,ed_transfer_byte
ed_transfer_test:
        EdEq R2,13,ed_transfer_cr
        EdEq R2,10,ed_transfer_line
        ADI R3,RZR,254
        EdLt R9,R3,ed_transfer_store
        JMP ed_bad
ed_transfer_store:
        SRM R2,[R8]
        ADI R8,R8,4
        ADI R9,R9,1
        JMP ed_transfer_byte

; T has its own 512-byte cache so the main stream and pending line survive.
ed_transfer_getbyte:
        EdBegin
        EdGet R8,ed_transfer_cached
        EdNe R8,0,ed_transfer_cached_byte
        EdGet R2,ed_transfer_in
        TrConst R3,ED_OUTPUT
        ADI R4,RZR,512
        EdCall tr_read
        EdEq R2,0,ed_transfer_cache_eof
        ADD R8,R2,RZR
        TrConst R2,ED_OUTPUT
        EdPut R2,ed_transfer_pointer
ed_transfer_cached_byte:
        EdGet R3,ed_transfer_pointer
        LRM R2,[R3]
        ADI R3,R3,4
        EdPut R3,ed_transfer_pointer
        ADI R8,R8,-1
        EdPut R8,ed_transfer_cached
        JMP ed_ok
ed_transfer_cache_eof:
        ADI R2,RZR,-1
        JMP ed_ok
ed_transfer_cr:
        EdSet ed_transfer_skip,1
        JMP ed_transfer_line
ed_transfer_eof_reached:
        EdSet ed_transfer_at_end,1
        EdEq R9,0,ed_ok
ed_transfer_line:
        EdPut R9,ed_line_len
        EdGet R2,ed_count
        ADI R2,R2,1
        EdCall ed_insert
        TrConst R8,ED_LINE
        ADI R9,RZR,0
        JMP ed_transfer_byte

; R2=path, success only if absent (DOS error 3). No collision is truncated.
ed_absent:
        EdBegin
        ADI R3,RZR,0
        TrCall tr_open
        ADD R8,R0,RZR
        EdEq R8,3,ed_ok
        EdNe R8,0,ed_absent_error
        EdCall tr_close
        ADI R8,RZR,80
ed_absent_error:
        ADD R0,R8,RZR
        JMP ed_return
ed_output_open:
        EdBegin
        EdGet R2,ed_write_error
        EdNe R2,0,ed_io_saved_error
        EdGet R2,ed_out
        EdNe R2,0,ed_ok
        TrConst R2,ED_TEMP
        EdCall ed_absent
        TrConst R2,ED_TEMP
        ADI R3,RZR,1
        EdCall tr_open
        EdPut R2,ed_out
        EdSet ed_owned,1
        JMP ed_ok

ed_write_command:
        EdGet R8,ed_a
        EdGet R9,ed_count
        EdEq R8,0,ed_write_default
        EdLt R9,R8,ed_write_default
        EdPut R8,ed_limit
        JMP ed_write_buffer
ed_write_default:
        ; Choose complete leading lines until <=25% of text remains.
        EdGet R6,ed_used
        ADI R8,RZR,0
        TrConst R10,ED_TEXT
        ADI R11,RZR,3073
ed_write_choose:
        EdLt R6,R11,ed_write_choose_index
        JMP ed_write_choose_more
ed_write_choose_index:
        SUB R2,R9,R8
        ADI R3,RZR,449
        EdLt R2,R3,ed_write_chosen
ed_write_choose_more:
        EdEq R8,0,ed_write_choose_line
        EdLt R8,R9,ed_write_choose_line
        JMP ed_write_chosen
ed_write_choose_line:
        LRM R2,[R10]
        ADI R2,R2,1
        SUB R6,R6,R2
        ADD R2,R2,R2
        ADD R2,R2,R2
        ADD R10,R10,R2
        ADI R8,R8,1
        JMP ed_write_choose
ed_write_chosen:
        EdPut R8,ed_limit
ed_write_buffer:
        TrCall ed_flush_lines
        JMP ed_return
; ed_limit leading lines. Batch complete records into <=512-byte writes.
; After a partial WRITE, remove only fully written records, retaining its tail.
ed_flush_lines:
        EdBegin
        EdGet R8,ed_limit
ed_flush_next:
        EdEq R8,0,ed_ok
        EdGet R2,ed_count
        EdEq R2,0,ed_ok
        EdCall ed_output_open
        TrConst R10,ED_TEXT
        ADI R9,RZR,0
        ADI R11,RZR,0
ed_flush_collect:
        EdLt R11,R8,ed_flush_collect_count
        JMP ed_flush_write
ed_flush_collect_count:
        EdGet R2,ed_count
        EdLt R11,R2,ed_flush_collect_size
        JMP ed_flush_write
ed_flush_collect_size:
        LRM R6,[R10]
        ADD R5,R9,R6
        ADI R5,R5,2
        ADI R3,RZR,513
        EdLt R5,R3,ed_flush_collect_line
        JMP ed_flush_write
ed_flush_collect_line:
        ADD R2,R9,R9
        ADD R2,R2,R2
        TrConst R0,ED_OUTPUT
        ADD R2,R2,R0
        ADI R3,R10,4
        ADD R4,R6,RZR
        TrCall ed_memmove
        ADD R2,R9,R6
        ADD R2,R2,R2
        ADD R2,R2,R2
        TrConst R0,ED_OUTPUT
        ADD R2,R2,R0
        ADI R0,RZR,13
        SRM R0,[R2]
        ADI R0,RZR,10
        SRM R0,[R2+4]
        ADD R9,R9,R6
        ADI R9,R9,2
        ADI R0,R6,1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R10,R10,R0
        ADI R11,R11,1
        JMP ed_flush_collect
ed_flush_write:
        EdPut R11,ed_target
        EdGet R2,ed_out
        TrConst R3,ED_OUTPUT
        ADD R4,R9,RZR
        TrCall tr_write
        SRZ R0
        JMP ed_flush_write_error
        SUB R0,R2,R9
        SRZ R0
        ADI R0,RZR,112
ed_flush_write_error:
        EdPut R0,ed_write_error
        ADD R6,R2,RZR
        TrConst R10,ED_TEXT
        ADI R11,RZR,0
ed_flush_completed:
        EdGet R2,ed_target
        EdLt R11,R2,ed_flush_completed_size
        JMP ed_flush_remove
ed_flush_completed_size:
        LRM R3,[R10]
        ADI R3,R3,2
        EdLt R6,R3,ed_flush_remove
        SUB R6,R6,R3
        ADI R0,R3,-1
        ADD R0,R0,R0
        ADD R0,R0,R0
        ADD R10,R10,R0
        ADI R11,R11,1
        JMP ed_flush_completed
ed_flush_remove:
        EdEq R11,0,ed_flush_status
        EdSet ed_a,1
        EdPut R11,ed_b
        EdCall ed_delete
        SUB R8,R8,R11
ed_flush_status:
        EdGet R0,ed_write_error
        SRZ R0
        JMP ed_return
        JMP ed_flush_next

ed_end_command:
        EdGet R2,ed_read_error
        EdNe R2,0,ed_io_saved_error
        EdGet R2,ed_write_error
        EdNe R2,0,ed_io_saved_error
        EdGet R2,ed_existed
        EdEq R2,0,ed_end_output
        TrConst R2,ED_BAK
        EdCall ed_absent
ed_end_output:
        EdCall ed_output_open
        EdGet R2,ed_count
        EdPut R2,ed_limit
        EdCall ed_flush_lines
ed_end_tail:
        EdGet R2,ed_pending_len
        EdNe R2,-1,ed_end_more
        EdGet R2,ed_eof
        EdNe R2,0,ed_end_close
ed_end_more:
        EdSet ed_limit,0
        EdCall ed_append
        EdGet R2,ed_count
        EdPut R2,ed_limit
        EdCall ed_flush_lines
        JMP ed_end_tail
ed_end_close:
        EdCall ed_close_all
        EdGet R2,ed_existed
        EdEq R2,0,ed_end_promote
        TrConst R2,ED_PATH
        TrConst R3,ED_BAK
        TrCall ed_rename
        SRZ R0
        JMP ed_end_first_rename_error
        EdSet ed_save_stage,1
ed_end_promote:
        TrConst R2,ED_TEMP
        TrConst R3,ED_PATH
        TrCall ed_rename
        SRZ R0
        JMP ed_end_restore
        EdSet ed_owned,0
        EdSet ed_save_stage,2
        EdSay ed_saved_message
        ADI R2,RZR,0
        TrCall tr_exit
ed_end_first_rename_error:
        PSH R0
        JMP ed_end_paths
ed_end_restore:
        PSH R0
        EdGet R2,ed_save_stage
        EdEq R2,0,ed_end_paths
        TrConst R2,ED_BAK
        TrConst R3,ED_PATH
        TrCall ed_rename
        SRZ R0
        JMP ed_end_paths
        EdSet ed_save_stage,0
ed_end_paths:
        EdSay ed_recover_message
        TrConst R2,ED_PATH
        TrCall tr_puts
        EdSay ed_nl
        TrConst R2,ED_BAK
        TrCall tr_puts
        EdSay ed_nl
        TrConst R2,ED_TEMP
        TrCall tr_puts
        EdSay ed_nl
        POP R0
        ; Closed output must not be re-opened/truncated after a failed rename.
        EdPut R0,ed_write_error
        JMP ed_return

; All handles must be closed before command API MOVE or DELETE.
ed_close_all:
        EdBegin
        ADI R8,RZR,0
        EdGet R2,ed_in
        EdEq R2,0,ed_close_output
        TrCall tr_close
        ADD R8,R0,RZR
        EdSet ed_in,0
ed_close_output:
        EdGet R2,ed_out
        EdEq R2,0,ed_close_transfer
        TrCall tr_close
        SNZ R8
        ADD R8,R0,RZR
        EdSet ed_out,0
ed_close_transfer:
        EdGet R2,ed_transfer_in
        EdEq R2,0,ed_close_done
        TrCall tr_close
        SNZ R8
        ADD R8,R0,RZR
        EdSet ed_transfer_in,0
ed_close_done:
        ADD R0,R8,RZR
        JMP ed_return
ed_quit_command:
        EdSay ed_quit_message
        ADI R3,RZR,0
        EdCall ed_yes
        EdEq R2,0,ed_ok
        EdCall ed_close_all
        EdGet R2,ed_owned
        EdEq R2,0,ed_quit_exit
        ; A complete recovery copy is kept following an incomplete promotion.
        EdGet R2,ed_write_error
        EdNe R2,0,ed_quit_keep
        TrConst R2,ED_TEMP
        TrCall ed_delete_file
        SRZ R0
        JMP ed_quit_keep
ed_quit_exit:
        ADI R2,RZR,0
        TrCall tr_exit
ed_quit_keep:
        EdSay ed_temp_kept
        TrConst R2,ED_TEMP
        TrCall tr_puts
        EdSay ed_nl
        JMP ed_quit_exit
; R2=source, R3=destination. <=63+1+63 command, no overwrite flag.
ed_rename:
        EdBegin
        ADD R8,R2,RZR
        ADD R9,R3,RZR
        TrConst R10,ED_MOVE
ed_move_source:
        LRM R2,[R8]
        EdEq R2,0,ed_move_space
        SRM R2,[R10]
        ADI R8,R8,4
        ADI R10,R10,4
        JMP ed_move_source
ed_move_space:
        ADI R2,RZR,32
        SRM R2,[R10]
        ADI R10,R10,4
ed_move_target:
        LRM R2,[R9]
        SRM R2,[R10]
        ADI R9,R9,4
        ADI R10,R10,4
        EdNe R2,0,ed_move_target
        TrConst R2,ED_MOVE
        DosCall DOS_MOVE
        JMP ed_return
ed_delete_file:
        DosCall DOS_DELETE
        JMP tr_return

ed_bak_suffix: DD 46,66,65,75,0
ed_temp_suffix: DD 46,36,36,36,0
ed_eof_message: DD 69,110,100,32,111,102,32,105,110,112,117,116,32,102,105,108,101,13,10,0
ed_saved_message: DD 83,97,118,101,100,13,10,0
ed_quit_message: DD 65,98,111,114,116,32,101,100,105,116,32,40,89,47,78,41,63,32,0
ed_recover_message: DD 82,101,99,111,118,101,114,121,32,112,97,116,104,115,58,13,10,0
ed_temp_kept: DD 84,101,109,112,111,114,97,114,121,32,102,105,108,101,32,107,101,112,116,58,32,0
