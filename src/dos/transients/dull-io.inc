; CLI, bounded source loading, streaming serialization, and normal MOVE /Y.
d_start:
        TrInit
        TrConst R13,D_STATE
        ADD R8,R13,RZR
        ADI R9,RZR,512
d_start_zero:
        SRM RZR,[R8]
        ADI R8,R8,4
        ADI R9,R9,-1
        SRZ R9
        JMP d_start_zero
        DPut SP,d_initialsp
        DSet d_line,1
        DSet d_tokline,1
        TrCall d_paths
        TrCall d_load
        TrCall d_runtime
        TrCall d_program
        TrCall d_save
        DSay d_success
        ADI R2,RZR,0
        TrCall tr_exit

d_paths:
        DBegin
        TrConst R2,0f1000h
        TrConst R3,D_INPUT
        ADI R4,RZR,64
        TrCall tr_arg
        DNe R0,0,d_parameter
        TrConst R8,D_INPUT
        TrCall d_validate_path
        TrConst R8,D_OUTPUT
        SRM RZR,[R8]
        ; d_validate_path saved tr_arg's next cursor before scanning the path.
        DGet R2,d_transfer
d_path_arguments:
        TrConst R3,D_MOVE
        ADI R4,RZR,64
        TrCall tr_arg
        DEq R0,1,d_path_defaults
        DNe R0,0,d_parameter
        DEq R3,0,d_parameter
        DPut R2,d_transfer
        TrConst R8,D_MOVE
        LRM R2,[R8]
        DEq R2,47,d_path_option
        TrConst R8,D_OUTPUT
        LRM R2,[R8]
        DNe R2,0,d_parameter
        TrConst R2,D_MOVE
        TrConst R3,D_OUTPUT
        TrCall d_copy_string
        DGet R2,d_transfer
        JMP d_path_arguments
d_path_option:
        DGet R2,d_mode
        DNe R2,0,d_parameter
        LRM R2,[R8+8]
        DNe R2,0,d_parameter
        LRM R2,[R8+4]
        TrCall d_upper
        DEq R2,65,d_path_asm
        DNe R2,76,d_parameter
        DSet d_mode,2
        DGet R2,d_transfer
        JMP d_path_arguments
d_path_asm:
        DSet d_mode,1
        DGet R2,d_transfer
        JMP d_path_arguments
d_path_defaults:
        TrConst R8,D_OUTPUT
        LRM R2,[R8]
        DNe R2,0,d_path_output_ready
        TrConst R2,D_INPUT
        TrConst R3,D_OUTPUT
        TrCall d_copy_string
        TrConst R8,D_OUTPUT
        TrCall d_leaf
        ADD R9,R2,RZR
d_path_find_extension:
        LRM R2,[R9]
        DEq R2,0,d_path_add_extension
        DEq R2,46,d_path_add_extension
        ADI R9,R9,4
        JMP d_path_find_extension
d_path_add_extension:
        TrConst R2,D_OUTPUT+57*4
        DLt R9,R2,d_path_extension_room
        JMP d_parameter
d_path_extension_room:
        DGet R2,d_mode
        DAddr R3,d_yke_ext
        DEq R2,0,d_path_set_extension
        DAddr R3,d_asm_ext
        DEq R2,1,d_path_set_extension
        DAddr R3,d_lst_ext
d_path_set_extension:
        ADD R2,R3,RZR
        ADD R3,R9,RZR
        TrCall d_copy_string
d_path_output_ready:
        TrConst R8,D_OUTPUT
        TrCall d_validate_path
        ; Output names are short enough for two paths and /Y in a 127-byte MOVE.
        TrConst R8,D_OUTPUT
        ADI R9,RZR,0
d_path_output_length:
        LRM R2,[R8]
        DEq R2,0,d_path_output_length_done
        ADI R8,R8,4
        ADI R9,R9,1
        JMP d_path_output_length
d_path_output_length_done:
        ADI R2,RZR,61
        DLt R9,R2,d_path_compare
        JMP d_parameter
d_path_compare:
        ; Conservative same-leaf exclusion covers relative/absolute aliases
        ; without inspecting DOS private current-directory data.
        TrConst R8,D_INPUT
        TrCall d_leaf
        ADD R10,R2,RZR
        TrConst R8,D_OUTPUT
        TrCall d_leaf
        ADD R9,R2,RZR
        TrCall d_same_leaf
        DEq R2,1,d_parameter
        TrConst R2,D_OUTPUT
        TrConst R3,D_TEMP
        TrCall d_copy_string
        TrConst R8,D_TEMP
        TrCall d_leaf
        ADD R9,R2,RZR
d_path_temp_ext:
        LRM R2,[R9]
        DEq R2,0,d_path_temp_suffix
        DEq R2,46,d_path_temp_suffix
        ADI R9,R9,4
        JMP d_path_temp_ext
d_path_temp_suffix:
        ADD R3,R9,RZR
        DAddr R2,d_temp_ext
        TrCall d_copy_string
        TrConst R8,D_TEMP
        TrCall d_leaf
        ADD R9,R2,RZR
        TrCall d_same_leaf
        DEq R2,1,d_parameter
        TrConst R8,D_OUTPUT
        TrCall d_leaf
        ADD R10,R2,RZR
        TrCall d_same_leaf
        DEq R2,1,d_parameter
        JMP d_return

; R8 path -> R2 leaf. Reject empty, '.'/'..', wildcard, whitespace, quotes,
; reserved delimiters. DOS validates actual 8.3 lengths and components on OPEN.
; R2 command cursor is stored before validation on the first call only.
d_validate_path:
        DBegin
        DPut R2,d_transfer
        ADD R9,R8,RZR
        ADI R10,RZR,0
d_validate_loop:
        LRM R2,[R8]
        DEq R2,0,d_validate_done
        DEq R2,47,d_parameter
        DEq R2,34,d_parameter
        DEq R2,42,d_parameter
        DEq R2,63,d_parameter
        ADI R3,RZR,33
        DLt R2,R3,d_parameter
        TrCall d_upper
        SRM R2,[R8]
        ADI R8,R8,4
        ADI R10,R10,1
        DEq R10,64,d_parameter
        JMP d_validate_loop
d_validate_done:
        DEq R10,0,d_parameter
        ADD R8,R9,RZR
        TrCall d_leaf
        LRM R3,[R2]
        DEq R3,0,d_parameter
        DEq R3,46,d_parameter
        JMP d_return
d_leaf:
        ADD R2,R8,RZR
d_leaf_loop:
        LRM R3,[R8]
        DEq R3,0,tr_return
        DEq R3,92,d_leaf_found
        DEq R3,58,d_leaf_found
        ADI R8,R8,4
        JMP d_leaf_loop
d_leaf_found:
        ADI R8,R8,4
        ADD R2,R8,RZR
        JMP d_leaf_loop
; R9/R10 NUL-terminated leaf strings -> R2 equality. Preserves both pointers.
d_same_leaf:
        DBegin
        ADD R8,R9,RZR
        ADD R11,R10,RZR
d_same_leaf_loop:
        LRM R2,[R8]
        LRM R3,[R11]
        SUB R0,R2,R3
        SRZ R0
        JMP d_same_leaf_no
        DEq R2,0,d_same_leaf_yes
        ADI R8,R8,4
        ADI R11,R11,4
        JMP d_same_leaf_loop
d_same_leaf_yes:
        ADI R2,RZR,1
        JMP d_return
d_same_leaf_no:
        ADI R2,RZR,0
        JMP d_return
d_copy_string:
        LRM R4,[R2]
        SRM R4,[R3]
        ADI R2,R2,4
        ADI R3,R3,4
        DEq R4,0,tr_return
        JMP d_copy_string

d_load:
        DBegin
        TrConst R2,D_INPUT
        ADI R3,RZR,0
        TrCall tr_open
        DNe R0,0,d_io_error
        DPut R2,d_handle
        TrConst R3,D_SOURCE
        TrConst R4,8192
        TrCall tr_read
        DNe R0,0,d_io_error
        ADD R8,R2,RZR
        ; READ may be short; accumulate until EOF or the exact limit.
d_load_more:
        TrConst R2,8192
        SUB R4,R2,R8
        DEq R4,0,d_load_probe
        ADD R3,R8,R8
        ADD R3,R3,R3
        TrConst R2,D_SOURCE
        ADD R3,R3,R2
        DGet R2,d_handle
        TrCall tr_read
        DNe R0,0,d_io_error
        DEq R2,0,d_load_closed
        ADD R8,R8,R2
        JMP d_load_more
d_load_probe:
        DGet R2,d_handle
        TrConst R3,D_BUFFER
        ADI R4,RZR,1
        TrCall tr_read
        DNe R0,0,d_io_error
        DNe R2,0,d_limit
d_load_closed:
        DGet R2,d_handle
        DSet d_handle,0
        TrCall tr_close
        DNe R0,0,d_io_error
        TrConst R9,D_SOURCE
        DPut R9,d_pos
d_load_validate:
        DEq R8,0,d_load_valid
        LRM R2,[R9]
        DEq R2,0,d_syntax
        DEq R2,26,d_load_valid
        DNe R2,10,d_load_valid_next
        DGet R2,d_tokline
        ADI R2,R2,1
        DPut R2,d_tokline
d_load_valid_next:
        ADI R9,R9,4
        ADI R8,R8,-1
        JMP d_load_validate
d_load_valid:
        DPut R9,d_end
        JMP d_return

; Serialization begins only after complete successful parsing/relocation.
; Probe destination validity and temp absence before creating any file.
d_save:
        DBegin
        TrConst R2,D_OUTPUT
        ADI R3,RZR,0
        TrCall tr_open
        DEq R0,3,d_save_temp_probe
        DNe R0,0,d_io_error
        TrCall tr_close
        DNe R0,0,d_io_error
d_save_temp_probe:
        TrConst R2,D_TEMP
        ADI R3,RZR,0
        TrCall tr_open
        DEq R0,3,d_save_create
        DNe R0,0,d_io_error
        TrCall tr_close
        JMP d_collision
d_save_create:
        TrConst R2,D_TEMP
        ADI R3,RZR,1
        TrCall tr_open
        DNe R0,0,d_io_error
        DPut R2,d_handle
        DSet d_owned,1
        DSet d_outputcount,0
        DGet R8,d_mode
        DNe R8,0,d_save_text
        ; YKE1 version=1, size, entry=0, additive checksum, reserved=0.
        TrConst R2,031454b59h
        TrCall d_outword
        ADI R2,RZR,1
        TrCall d_outword
        DGet R2,d_size
        TrCall d_outword
        ADI R2,RZR,0
        TrCall d_outword
        TrConst R8,90000h
        DGet R9,d_size
        ADI R10,RZR,0
d_save_checksum:
        DEq R9,0,d_save_header_end
        LRM R2,[R8]
        ADD R10,R10,R2
        ADI R8,R8,4
        ADI R9,R9,-4
        JMP d_save_checksum
d_save_header_end:
        ADD R2,R10,RZR
        TrCall d_outword
        ADI R2,RZR,0
        TrCall d_outword
        TrConst R8,90000h
        DGet R9,d_size
d_save_payload:
        DEq R9,0,d_save_close
        LRM R2,[R8]
        TrCall d_outword
        ADI R8,R8,4
        ADI R9,R9,-4
        JMP d_save_payload
d_save_text:
        DNe R8,1,d_save_listing
        DAddr R2,d_asm_header
        TrCall d_outstring
        JMP d_save_text_words
d_save_listing:
        DAddr R2,d_lst_header
        TrCall d_outstring
d_save_text_words:
        ADI R8,RZR,0
        TrConst R9,90000h
        DSet d_listingrun,0
        DSet d_listingline,0
d_save_text_loop:
        DGet R2,d_size
        SUB R0,R8,R2
        SNZ R0
        JMP d_save_close
        DGet R2,d_mode
        DEq R2,2,d_save_listing_word
        DAddr R2,d_asm_dd
        TrCall d_outstring
        LRM R2,[R9]
        TrCall d_outhex
        ADI R2,RZR,104
        TrCall d_outbyte
        JMP d_save_text_line_end
d_save_listing_word:
        ; Run lookup is monotonic and never reads beyond the recorded run table.
        DGet R10,d_listingrun
        DGet R11,d_runs
d_save_listing_run:
        SUB R0,R10,R11
        SNZ R0
        JMP d_save_listing_ready
        ADD R2,R10,R10
        ADD R2,R2,R2
        ADD R2,R2,R2
        TrConst R3,D_LINES
        ADD R2,R2,R3
        LRM R3,[R2]
        DLt R8,R3,d_save_listing_ready
        LRM R2,[R2+4]
        DPut R2,d_listingline
        ADI R10,R10,1
        DPut R10,d_listingrun
        JMP d_save_listing_run
d_save_listing_ready:
        ADD R2,R8,RZR
        TrCall d_outhex
        ADI R2,RZR,32
        TrCall d_outbyte
        LRM R2,[R9]
        TrCall d_outhex
        ADI R2,RZR,32
        TrCall d_outbyte
        DGet R2,d_listingline
        TrCall d_outdecimal
d_save_text_line_end:
        ADI R2,RZR,13
        TrCall d_outbyte
        ADI R2,RZR,10
        TrCall d_outbyte
        ADI R8,R8,4
        ADI R9,R9,4
        JMP d_save_text_loop
d_save_close:
        TrCall d_flush
        DGet R2,d_handle
        DSet d_handle,0
        TrCall tr_close
        DNe R0,0,d_io_error
        TrConst R2,D_TEMP
        TrConst R3,D_MOVE
        TrCall d_copy_string
        ADI R3,R3,-4
        ADI R4,RZR,32
        SRM R4,[R3]
        ADI R3,R3,4
        TrConst R2,D_OUTPUT
        TrCall d_copy_string
        ADI R3,R3,-4
        DAddr R2,d_move_option
        TrCall d_copy_string
        TrConst R2,D_MOVE
        TrCall d_promote
        DNe R0,0,d_io_error
        DSet d_owned,0
        JMP d_return
d_promote:
        DosCall DOS_MOVE
        JMP tr_return

; Word -> four little-endian bytes. tr_div has fixed bounded work.
d_outword:
        DBegin
        ADD R8,R2,RZR
        ADI R9,RZR,4
d_outword_loop:
        ADD R0,R8,RZR
        ADI R1,RZR,256
        TrCall tr_div
        ADD R8,R0,RZR
        ADD R2,R1,RZR
        TrCall d_outbyte
        ADI R9,R9,-1
        DNe R9,0,d_outword_loop
        JMP d_return
d_outbyte:
        DBegin
        DGet R8,d_outputcount
        ADD R3,R8,R8
        ADD R3,R3,R3
        TrConst R4,D_BUFFER
        ADD R3,R3,R4
        SRM R2,[R3]
        ADI R8,R8,1
        DPut R8,d_outputcount
        DNe R8,2048,d_return
        TrCall d_flush
        JMP d_return
d_flush:
        DBegin
        DGet R4,d_outputcount
        DEq R4,0,d_return
        ADD R8,R4,RZR
        DGet R2,d_handle
        TrConst R3,D_BUFFER
        TrCall tr_write
        DNe R0,0,d_io_error
        SUB R0,R2,R8
        SRZ R0
        JMP d_short_write
        DSet d_outputcount,0
        JMP d_return
d_outstring:
        DBegin
        ADD R8,R2,RZR
d_outstring_loop:
        LRM R2,[R8]
        DEq R2,0,d_return
        TrCall d_outbyte
        ADI R8,R8,4
        JMP d_outstring_loop
; Eight hex digits. Leading zero makes DD ...h valid even for A..F.
d_outhex:
        DBegin
        ADD R8,R2,RZR
        ADI R9,RZR,8
d_outhex_div:
        ADD R0,R8,RZR
        ADI R1,RZR,16
        TrCall tr_div
        ADD R8,R0,RZR
        ADI R2,R1,48
        ADI R3,RZR,58
        DLt R2,R3,d_outhex_push
        ADI R2,R2,7
d_outhex_push:
        PSH R2
        ADI R9,R9,-1
        DNe R9,0,d_outhex_div
        ADI R9,RZR,8
d_outhex_pop:
        POP R2
        TrCall d_outbyte
        ADI R9,R9,-1
        DNe R9,0,d_outhex_pop
        JMP d_return
d_outdecimal:
        DBegin
        TrConst R3,D_NUMBER
        TrCall tr_uint
        TrConst R2,D_NUMBER
        TrCall d_outstring
        JMP d_return

d_parameter:
        DAddr R2,d_parameter_message
        JMP d_fatal
d_syntax:
        DAddr R2,d_syntax_message
        JMP d_fatal
d_reserved:
        DAddr R2,d_reserved_message
        JMP d_fatal
d_string_error:
        DAddr R2,d_string_message
        JMP d_fatal
d_limit:
        DAddr R2,d_limit_message
        JMP d_fatal
d_duplicate:
        DAddr R2,d_duplicate_message
        JMP d_fatal
d_undefined:
        DAddr R2,d_undefined_message
        JMP d_fatal
d_arity:
        DAddr R2,d_arity_message
        JMP d_fatal
d_outside_loop:
        DAddr R2,d_loop_message
        JMP d_fatal
d_collision:
        DAddr R2,d_collision_message
        JMP d_fatal
d_short_write:
        ADI R0,RZR,112
d_io_error:
        DPut R0,d_savederror
        DAddr R2,d_io_message
d_fatal:
        DPut R2,d_outputptr
        DGet SP,d_initialsp
        DGet R2,d_handle
        DEq R2,0,d_fatal_print
        DSet d_handle,0
        TrCall tr_close
d_fatal_print:
        DSay d_error_prefix
        DGet R2,d_tokline
        TrConst R3,D_NUMBER
        TrCall tr_uint
        TrConst R2,D_NUMBER
        TrCall tr_puts
        DSay d_colon
        DGet R2,d_outputptr
        TrCall tr_puts
        DGet R2,d_savederror
        DEq R2,0,d_fatal_end
        TrCall tr_error
d_fatal_end:
        DSay d_crlf
        DGet R2,d_owned
        DEq R2,0,d_fatal_exit
        DSay d_temp_message
        TrConst R2,D_TEMP
        TrCall tr_puts
        DSay d_crlf
d_fatal_exit:
        ADI R2,RZR,1
        TrCall tr_exit

d_yke_ext: DD 46,89,75,69,0
d_asm_ext: DD 46,65,83,77,0
d_lst_ext: DD 46,76,83,84,0
d_temp_ext: DD 46,36,36,36,0
d_move_option: DD 32,47,89,0
d_success: DD 68,85,76,76,58,32,99,111,109,112,105,108,101,100,13,10,0
d_error_prefix: DD 68,85,76,76,32,108,105,110,101,32,0
d_colon: DD 58,32,0
d_crlf: DD 13,10,0
d_syntax_message: DD 115,121,110,116,97,120,32,101,114,114,111,114,0
d_reserved_message: DD 114,101,115,101,114,118,101,100,32,105,100,101,110,116,105,102,105,101,114,0
d_parameter_message: DD 105,110,118,97,108,105,100,32,112,97,114,97,109,101,116,101,114,0
d_string_message: DD 117,110,116,101,114,109,105,110,97,116,101,100,32,115,116,114,105,110,103,0
d_limit_message: DD 99,97,112,97,99,105,116,121,32,108,105,109,105,116,0
d_duplicate_message: DD 100,117,112,108,105,99,97,116,101,32,105,100,101,110,116,105,102,105,101,114,0
d_undefined_message: DD 117,110,100,101,102,105,110,101,100,32,105,100,101,110,116,105,102,105,101,114,0
d_arity_message: DD 97,114,103,117,109,101,110,116,32,99,111,117,110,116,0
d_loop_message: DD 66,82,69,65,75,47,67,79,78,84,73,78,85,69,32,111,117,116,115,105,100,101,32,87,72,73,76,69,0
d_collision_message: DD 116,101,109,112,111,114,97,114,121,32,102,105,108,101,32,101,120,105,115,116,115,0
d_io_message: DD 73,47,79,32,101,114,114,111,114,32,0
d_temp_message: DD 84,101,109,112,111,114,97,114,121,32,102,105,108,101,32,107,101,112,116,58,32,0
d_asm_header: DD 59,32,80,76,47,68,85,76,76,32,110,97,116,105,118,101,32,75,65,80,80,65,13,10,67,80,85,32,89,79,83,73,67,45,75,65,80,80,65,13,10,0
d_asm_dd: DD 68,68,32,48,0
d_lst_header: DD 59,32,111,102,102,115,101,116,32,119,111,114,100,32,115,111,117,114,99,101,45,108,105,110,101,13,10,0
