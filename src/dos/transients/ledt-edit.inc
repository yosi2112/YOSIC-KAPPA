; Parse up to four numeric fields, omitted fields, . / # / +/-n, then [?]op.
ed_parse:
        EdBegin
        EdSet ed_a,0
        EdSet ed_b,0
        EdSet ed_c,0
        EdSet ed_repeat,0
        EdSet ed_confirm,0
        TrConst R8,ED_CMD_BUFFER
        ADI R9,R13,ed_a
        ADI R10,RZR,1
ed_parse_field:
        TrCall ed_spaces
        LRM R2,[R8]
        EdEq R2,46,ed_parse_current
        EdEq R2,35,ed_parse_end
        EdEq R2,43,ed_parse_relative
        EdEq R2,45,ed_parse_relative
        ADI R3,RZR,48
        EdLt R2,R3,ed_parse_after
        ADI R3,RZR,58
        EdLt R2,R3,ed_parse_absolute
        JMP ed_parse_after
ed_parse_current:
        EdGet R6,ed_current
        ADI R8,R8,4
        JMP ed_parse_store
ed_parse_end:
        EdGet R6,ed_count
        ADI R6,R6,1
        ADI R8,R8,4
        JMP ed_parse_store
ed_parse_relative:
        ADD R11,R2,RZR
        ADI R8,R8,4
        EdCall ed_number
        EdGet R2,ed_current
        EdEq R11,45,ed_parse_subtract
        ADD R6,R2,R6
        JMP ed_parse_positive
ed_parse_subtract:
        SUB R6,R2,R6
ed_parse_positive:
        ADI R2,RZR,1
        EdLt R6,R2,ed_bad
        JMP ed_parse_store
ed_parse_absolute:
        EdCall ed_number
        EdEq R6,0,ed_bad
ed_parse_store:
        SRM R6,[R9]
ed_parse_after:
        TrCall ed_spaces
        LRM R2,[R8]
        EdNe R2,44,ed_parse_opcode
        ADI R2,RZR,4
        EdLt R10,R2,ed_parse_comma
        JMP ed_bad
ed_parse_comma:
        ADI R8,R8,4
        ADI R9,R9,4
        ADI R10,R10,1
        JMP ed_parse_field
ed_parse_opcode:
        EdPut R10,ed_fields
        EdNe R2,63,ed_parse_letter
        EdSet ed_confirm,1
        ADI R8,R8,4
        LRM R2,[R8]
ed_parse_letter:
        ADI R3,RZR,97
        EdLt R2,R3,ed_parse_letter_ready
        ADI R3,RZR,123
        EdLt R2,R3,ed_parse_upper
        JMP ed_bad
ed_parse_upper:
        ADI R2,R2,-32
ed_parse_letter_ready:
        EdPut R2,ed_op
        EdEq R2,0,ed_parse_tail
        ADI R8,R8,4
ed_parse_tail:
        EdPut R8,ed_tail
        JMP ed_ok
; Cursor R8 advances. R6 uint <=65535, R8/R9/R10/R11 otherwise preserved.
ed_number:
        ADI R6,RZR,0
        ADI R5,RZR,0
ed_number_next:
        LRM R2,[R8]
        ADI R3,RZR,48
        EdLt R2,R3,ed_number_done
        ADI R3,RZR,58
        EdLt R2,R3,ed_number_digit
        JMP ed_number_done
ed_number_digit:
        ADD R0,R6,R6
        ADD R6,R0,R0
        ADD R6,R6,R6
        ADD R6,R6,R0
        ADI R2,R2,-48
        ADD R6,R6,R2
        TrConst R3,65536
        EdLt R6,R3,ed_number_advance
        ADI R0,RZR,87
        JMP tr_return
ed_number_advance:
        ADI R8,R8,4
        ADI R5,RZR,1
        JMP ed_number_next
ed_number_done:
        ADI R0,RZR,0
        SNZ R5
        ADI R0,RZR,87
        JMP tr_return
ed_spaces:
        LRM R2,[R8]
        EdEq R2,32,ed_space_next
        EdEq R2,9,ed_space_next
        JMP tr_return
ed_space_next:
        ADI R8,R8,4
        JMP ed_spaces

ed_command:
        EdBegin
        EdCall ed_parse
        EdGet R8,ed_op
        EdEq R8,83,ed_search_command
        EdEq R8,82,ed_search_command
        EdEq R8,84,ed_transfer_command
        EdGet R8,ed_tail
        TrCall ed_spaces
        LRM R2,[R8]
        EdNe R2,0,ed_bad
        EdGet R2,ed_confirm
        EdNe R2,0,ed_bad
        EdGet R8,ed_op
        EdEq R8,67,ed_copy_command
        EdEq R8,77,ed_copy_command
        EdGet R2,ed_fields
        ADI R3,RZR,3
        EdLt R2,R3,ed_command_small
        JMP ed_bad
ed_command_small:
        EdEq R8,76,ed_list_command
        EdEq R8,80,ed_list_command
        EdEq R8,68,ed_delete_command
        EdGet R2,ed_fields
        EdNe R2,1,ed_bad
        EdEq R8,73,ed_insert_command
        EdEq R8,0,ed_edit_command
        EdEq R8,65,ed_append_command
        EdEq R8,87,ed_write_command
        EdGet R2,ed_a
        EdNe R2,0,ed_bad
        EdEq R8,69,ed_end_command
        EdEq R8,81,ed_quit_command
        JMP ed_bad

; Validate inclusive ed_a/ed_b; '#' as last means actual last line.
ed_range:
        EdBegin
        EdGet R8,ed_a
        EdGet R9,ed_b
        EdGet R10,ed_count
        ADI R2,RZR,1
        EdLt R8,R2,ed_bad
        EdLt R10,R8,ed_bad
        EdLt R9,R8,ed_bad
        EdLt R10,R9,ed_range_clamp
        JMP ed_ok
ed_range_clamp:
        ADI R2,R10,1
        EdLt R2,R9,ed_bad
        EdPut R10,ed_b
        JMP ed_ok
ed_single:
        EdBegin
        EdGet R2,ed_a
        EdNe R2,0,ed_single_valid
        EdGet R2,ed_current
        EdPut R2,ed_a
ed_single_valid:
        EdGet R3,ed_count
        ADI R3,R3,1
        EdLt R3,R2,ed_single_clamp
        JMP ed_ok
ed_single_clamp:
        EdPut R3,ed_a
        JMP ed_ok

ed_list_command:
        EdGet R2,ed_a
        EdNe R2,0,ed_list_first
        EdGet R2,ed_current
        EdGet R3,ed_op
        EdEq R3,80,ed_page_first
        ADI R2,R2,-11
        ADI R3,RZR,1
        EdLt R2,R3,ed_list_one
        JMP ed_list_first
ed_list_one:
        ADI R2,RZR,1
        JMP ed_list_first
ed_page_first:
        ADI R2,R2,1
ed_list_first:
        EdPut R2,ed_a
        EdGet R3,ed_b
        EdNe R3,0,ed_list_last
        ADI R3,R2,22
ed_list_last:
        EdGet R4,ed_count
        EdLt R4,R3,ed_list_end
        JMP ed_list_range
ed_list_end:
        ADD R3,R4,RZR
ed_list_range:
        EdPut R3,ed_b
        EdCall ed_range
        EdGet R8,ed_a
        EdGet R9,ed_b
ed_list_loop:
        ADD R2,R8,RZR
        TrCall ed_show
        ADI R8,R8,1
        EdLt R9,R8,ed_list_done
        JMP ed_list_loop
ed_list_done:
        EdGet R2,ed_op
        EdNe R2,80,ed_ok
        EdPut R9,ed_current
        JMP ed_ok

ed_delete_command:
        EdCall ed_single
        EdGet R2,ed_b
        EdNe R2,0,ed_delete_range
        EdGet R2,ed_a
        EdPut R2,ed_b
ed_delete_range:
        EdCall ed_range
        TrCall ed_delete
        JMP ed_return

ed_insert_command:
        EdCall ed_single
        EdGet R8,ed_a
ed_insert_loop:
        ADD R2,R8,RZR
        TrCall ed_line_prompt
        TrConst R2,ED_LINE
        ADI R4,RZR,255
        TrCall ed_readline
        ADD R9,R0,RZR
        EdEq R9,995,ed_ok
        ADD R0,R9,RZR
        SRZ R0
        JMP ed_return
        EdPut R2,ed_line_len
        EdNe R2,1,ed_insert_text
        TrConst R3,ED_LINE
        LRM R3,[R3]
        EdEq R3,26,ed_insert_done
ed_insert_text:
        ADD R2,R8,RZR
        EdCall ed_insert
        ADI R8,R8,1
        JMP ed_insert_loop
ed_insert_done:
        EdPut R8,ed_current
        JMP ed_ok
ed_edit_command:
        EdGet R2,ed_a
        EdNe R2,0,ed_edit_selected
        EdGet R2,ed_current
        ADI R2,R2,1
ed_edit_selected:
        EdPut R2,ed_a
        EdPut R2,ed_b
        EdCall ed_range
        EdGet R8,ed_a
        EdPut R8,ed_current
        ADD R2,R8,RZR
        TrCall ed_show
        ADD R2,R8,RZR
        TrCall ed_line_prompt
        TrConst R2,ED_LINE
        ADI R4,RZR,255
        TrCall ed_readline
        ADD R9,R0,RZR
        EdEq R9,995,ed_ok
        ADD R0,R9,RZR
        SRZ R0
        JMP ed_return
        EdEq R2,0,ed_ok
        EdPut R2,ed_line_len
        ADD R2,R8,RZR
        TrCall ed_replace_line
        JMP ed_return

; Replace single R2 using ED_LINE. Preflight size before deleting any text.
ed_replace_line:
        EdBegin
        ADD R11,R2,RZR
        TrCall ed_ptr
        LRM R8,[R2]
        EdGet R9,ed_line_len
        SUB R9,R9,R8
        EdGet R8,ed_used
        ADD R8,R8,R9
        TrConst R9,12289
        EdLt R8,R9,ed_replace_space
        JMP ed_full
ed_replace_space:
        EdGet R8,ed_a
        EdGet R9,ed_b
        EdPut R11,ed_a
        EdPut R11,ed_b
        TrCall ed_delete
        ADD R2,R11,RZR
        TrCall ed_insert
        EdPut R8,ed_a
        EdPut R9,ed_b
        JMP ed_return

ed_line_prompt:
        EdBegin
        TrCall tr_error
        EdSay ed_colon
        JMP ed_ok
ed_show:
        EdBegin
        ADD R8,R2,RZR
        TrCall tr_error
        EdSay ed_colon
        EdGet R3,ed_current
        SUB R0,R8,R3
        ADI R2,RZR,32
        SNZ R0
        ADI R2,RZR,42
        TrCall tr_putchar
        ADD R2,R8,RZR
        TrCall ed_ptr
        LRM R9,[R2]
        ADI R8,R2,4
ed_show_byte:
        EdEq R9,0,ed_show_done
        LRM R2,[R8]
        TrCall tr_putchar
        ADI R8,R8,4
        ADI R9,R9,-1
        JMP ed_show_byte
ed_show_done:
        EdSay ed_nl
        JMP ed_ok
ed_colon: DD 58,0

; C/M range defaults current/current. Destination mandatory, outside range.
ed_copy_command:
        EdGet R2,ed_fields
        ADI R3,RZR,3
        EdLt R2,R3,ed_bad
        EdGet R2,ed_op
        EdNe R2,77,ed_copy_fields
        EdGet R2,ed_fields
        EdNe R2,3,ed_bad
ed_copy_fields:
        EdCall ed_single
        EdGet R2,ed_b
        EdNe R2,0,ed_copy_range
        EdGet R2,ed_current
        EdPut R2,ed_b
ed_copy_range:
        EdCall ed_range
        EdGet R8,ed_a
        EdGet R9,ed_b
        EdGet R10,ed_c
        EdEq R10,0,ed_bad
        EdGet R2,ed_count
        ADI R2,R2,1
        EdLt R2,R10,ed_bad
        EdLt R10,R8,ed_copy_destination
        EdLt R9,R10,ed_copy_destination
        JMP ed_bad
ed_copy_destination:
        EdGet R2,ed_op
        EdEq R2,77,ed_move_lines
        EdGet R11,ed_repeat
        EdNe R11,0,ed_copy_repeat
        ADI R11,RZR,1
ed_copy_repeat:
        ; Bound repeat multiplication with iterative preflight before mutation.
        SUB R6,R9,R8
        ADI R6,R6,1
        ADD R2,R8,RZR
        TrCall ed_ptr
        PSH R2
        ADI R2,R9,1
        TrCall ed_ptr
        POP R3
        SUB R2,R2,R3
        EdPut R2,ed_replace_old
        EdGet R8,ed_used
        EdGet R9,ed_count
        ADD R5,R11,RZR
ed_copy_preflight:
        EdGet R2,ed_replace_old
        ADD R0,R2,RZR
        ADI R1,RZR,4
        PSH R5
        TrCall tr_div
        POP R5
        ADD R8,R8,R0
        ADD R9,R9,R6
        TrConst R3,12289
        EdLt R8,R3,ed_copy_check_index
        JMP ed_full
ed_copy_check_index:
        ADI R3,RZR,1792
        EdLt R9,R3,ed_copy_preflight_next
        JMP ed_full
ed_copy_preflight_next:
        ADI R5,R5,-1
        EdNe R5,0,ed_copy_preflight
        EdPut R8,ed_used
        ; Open a single gap for all copies. Preserve offsets across move.
        ADD R2,R10,RZR
        TrCall ed_ptr
        ADD R8,R2,RZR
        EdGet R2,ed_count
        ADI R2,R2,1
        TrCall ed_ptr
        SUB R0,R2,R8
        ADI R1,RZR,4
        TrCall tr_div
        ADD R4,R0,RZR
        EdGet R2,ed_replace_old
        ADI R9,RZR,0
        ADD R5,R11,RZR
ed_copy_gap_size:
        ADD R9,R9,R2
        ADI R5,R5,-1
        EdNe R5,0,ed_copy_gap_size
        ADD R2,R8,R9
        ADD R3,R8,RZR
        TrCall ed_memmove
        EdGet R2,ed_a
        TrCall ed_ptr
        EdGet R3,ed_a
        EdLt R10,R3,ed_copy_adjust_source
        JMP ed_copy_source_ready
ed_copy_adjust_source:
        ADD R2,R2,R9
ed_copy_source_ready:
        ADD R9,R2,RZR
ed_copy_block:
        EdGet R0,ed_replace_old
        ADI R1,RZR,4
        TrCall tr_div
        ADD R4,R0,RZR
        ADD R2,R8,RZR
        ADD R3,R9,RZR
        TrCall ed_memmove
        EdGet R0,ed_replace_old
        ADD R8,R8,R0
        ADI R11,R11,-1
        EdNe R11,0,ed_copy_block
        EdPut R10,ed_current
        TrCall ed_reindex
        JMP ed_ok

; Rotate word ranges using three reversals; M needs no spare text capacity.
ed_move_lines:
        SUB R11,R9,R8
        ADI R11,R11,1
        ADD R2,R8,RZR
        TrCall ed_ptr
        ADD R8,R2,RZR
        ADI R2,R9,1
        TrCall ed_ptr
        ADD R9,R2,RZR
        ADD R2,R10,RZR
        TrCall ed_ptr
        ADD R6,R2,RZR
        EdGet R3,ed_a
        EdLt R10,R3,ed_move_before
        ADD R2,R8,RZR
        ADD R3,R9,RZR
        TrCall ed_reverse
        ADD R2,R9,RZR
        ADD R3,R6,RZR
        TrCall ed_reverse
        ADD R2,R8,RZR
        ADD R3,R6,RZR
        TrCall ed_reverse
        SUB R10,R10,R11
        JMP ed_move_index
ed_move_before:
        ADD R2,R6,RZR
        ADD R3,R8,RZR
        TrCall ed_reverse
        ADD R2,R8,RZR
        ADD R3,R9,RZR
        TrCall ed_reverse
        ADD R2,R6,RZR
        ADD R3,R9,RZR
        TrCall ed_reverse
ed_move_index:
        EdPut R10,ed_current
        TrCall ed_reindex
        JMP ed_ok
ed_reverse:
        EdBegin
        ADD R8,R2,RZR
        ADI R9,R3,-4
ed_reverse_loop:
        EdLt R8,R9,ed_reverse_swap
        JMP ed_ok
ed_reverse_swap:
        LRM R2,[R8]
        LRM R3,[R9]
        SRM R3,[R8]
        SRM R2,[R9]
        ADI R8,R8,4
        ADI R9,R9,-4
        JMP ed_reverse_loop

ed_search_command:
        EdGet R2,ed_fields
        ADI R3,RZR,3
        EdLt R2,R3,ed_search_fields
        JMP ed_bad
ed_search_fields:
        EdGet R8,ed_tail
        ADD R9,R8,RZR
        ADI R6,RZR,0
ed_pattern_end:
        LRM R2,[R9]
        EdEq R2,0,ed_pattern_ready
        EdEq R2,26,ed_pattern_delimiter
        ADI R9,R9,4
        ADI R6,R6,1
        JMP ed_pattern_end
ed_pattern_delimiter:
        SRM RZR,[R9]
        ADI R9,R9,4
ed_pattern_ready:
        EdPut R9,ed_replace_ptr
        EdEq R6,0,ed_pattern_previous
        EdPut R6,ed_find_len
        TrConst R2,ED_SEARCH
        ADD R3,R8,RZR
        ADI R4,R6,1
        TrCall ed_memmove
ed_pattern_previous:
        EdGet R6,ed_find_len
        EdEq R6,0,ed_bad
        EdGet R8,ed_op
        EdEq R8,83,ed_pattern_search_only
        EdGet R9,ed_replace_ptr
        ADI R6,RZR,0
ed_replacement_length:
        LRM R2,[R9]
        EdEq R2,0,ed_replacement_ready
        EdEq R2,26,ed_replacement_terminator
        ADI R9,R9,4
        ADI R6,R6,1
        JMP ed_replacement_length
ed_replacement_terminator:
        SRM RZR,[R9]
ed_replacement_ready:
        EdPut R6,ed_replace_len
ed_pattern_search_only:
        EdGet R2,ed_a
        EdNe R2,0,ed_search_first
        EdGet R2,ed_current
        ADI R2,R2,1
        EdGet R3,ed_count
        EdLt R3,R2,ed_search_no_more
ed_search_first:
        EdPut R2,ed_a
        EdGet R3,ed_b
        EdNe R3,0,ed_search_last
        EdGet R3,ed_count
ed_search_last:
        EdPut R3,ed_b
        EdCall ed_range
        EdSet ed_found,0
        EdGet R8,ed_a
ed_search_line:
        ADD R2,R8,RZR
        TrCall ed_ptr
        LRM R9,[R2]
        ADI R3,R2,4
        TrConst R2,ED_LINE
        ADD R4,R9,RZR
        TrCall ed_memmove
        ADI R10,RZR,0
ed_search_position:
        EdGet R6,ed_find_len
        ADD R2,R10,R6
        EdLt R9,R2,ed_search_next_line
        ADD R11,R10,R10
        ADD R11,R11,R11
        TrConst R2,ED_LINE
        ADD R11,R11,R2
        ADD R3,R11,RZR
        TrConst R4,ED_SEARCH
        ADD R5,R6,RZR
ed_match_byte:
        LRM R2,[R3]
        LRM R7,[R4]
        SUB R0,R2,R7
        SRZ R0
        JMP ed_no_match
        ADI R3,R3,4
        ADI R4,R4,4
        ADI R5,R5,-1
        EdNe R5,0,ed_match_byte
        ADD R2,R8,RZR
        TrCall ed_show
        EdGet R2,ed_confirm
        EdEq R2,0,ed_match_accept
        EdSay ed_ok_message
        ADI R3,RZR,1
        TrCall ed_yes
        EdEq R2,0,ed_match_reject
ed_match_accept:
        EdGet R2,ed_op
        EdEq R2,83,ed_search_success
        EdGet R2,ed_replace_len
        SUB R3,R2,R6
        ADD R3,R9,R3
        ADI R4,RZR,255
        EdLt R3,R4,ed_replace_length_ok
        JMP ed_bad
ed_replace_length_ok:
        EdGet R4,ed_used
        ADD R4,R4,R3
        SUB R4,R4,R9
        TrConst R5,12289
        EdLt R4,R5,ed_replace_capacity_ok
        JMP ed_full
ed_replace_capacity_ok:
        EdPut R3,ed_line_len
        ; Move old line suffix around the replacement in the scratch line.
        ADD R3,R6,R6
        ADD R3,R3,R3
        ADD R3,R11,R3
        ADD R0,R2,R2
        ADD R0,R0,R0
        ADD R2,R11,R0
        SUB R4,R9,R10
        SUB R4,R4,R6
        TrCall ed_memmove
        ADD R2,R11,RZR
        EdGet R3,ed_replace_ptr
        EdGet R4,ed_replace_len
        TrCall ed_memmove
        ADD R2,R8,RZR
        EdCall ed_replace_line
        EdGet R9,ed_line_len
        EdGet R2,ed_replace_len
        ADD R10,R10,R2
        EdSet ed_found,1
        ADD R2,R8,RZR
        TrCall ed_show
        JMP ed_search_position
ed_search_success:
        EdPut R8,ed_current
        JMP ed_ok
ed_match_reject:
        EdGet R2,ed_op
        EdEq R2,83,ed_search_next_line
        ADD R10,R10,R6
        JMP ed_search_position
ed_no_match:
        ADI R10,R10,1
        JMP ed_search_position
ed_search_next_line:
        ADI R8,R8,1
        EdGet R2,ed_b
        EdLt R2,R8,ed_search_complete
        JMP ed_search_line
ed_search_complete:
        EdGet R2,ed_found
        EdNe R2,0,ed_ok
ed_search_no_more:
        EdSay ed_not_found
        JMP ed_ok

; Read a whole answer, so CRLF and multi-character replies cannot leak to '*'.
; R3=accept empty (search yes, quit no), result R2=boolean.
ed_yes:
        EdBegin
        ADD R8,R3,RZR
        TrConst R2,ED_OUTPUT
        ADI R4,RZR,128
        TrCall ed_readline
        SRZ R0
        JMP ed_answer_no
        EdNe R2,0,ed_answer_key
        ADD R2,R8,RZR
        JMP ed_ok
ed_answer_key:
        TrConst R2,ED_OUTPUT
        LRM R2,[R2]
        EdEq R2,89,ed_answer_yes
        EdEq R2,121,ed_answer_yes
ed_answer_no:
        ADI R2,RZR,0
        JMP ed_ok
ed_answer_yes:
        ADI R2,RZR,1
        JMP ed_ok
ed_ok_message: DD 79,46,75,46,63,32,0
ed_not_found: DD 78,111,116,32,102,111,117,110,100,13,10,0
