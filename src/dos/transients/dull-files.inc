; File builtins are ordinary signed16 expressions. Literals remain immutable
; word bytes in generated code; OPEN copies its path into owned runtime RAM.
d_file_builtin:
        DBegin
        DGet R8,d_token
        TrCall d_next
        DEq R8,T_IORESULT,d_file_result
        TrCall d_enter
        DExpect 40
        DEq R8,T_FOPEN,d_file_open
        TrCall d_expression
        DEq R8,T_FWRITE,d_file_write
        DEq R8,T_FWRITELN,d_file_write
        DNe R8,T_FPUTC,d_file_one
        DSeq d_ops_5,1 ; PSH handle
        DExpect 44
        TrCall d_expression
        DSeq d_file_value,2 ; ADD R3,R2,RZR / POP R2
        DExpect 41
        DRun rt_fputc
        JMP d_file_done
d_file_one:
        DExpect 41
        DEq R8,T_FCLOSE,d_file_close
        DEq R8,T_FGETC,d_file_get
        DRun rt_feof
        JMP d_file_done
d_file_close:
        DRun rt_fclose
        JMP d_file_done
d_file_get:
        DRun rt_fgetc
        JMP d_file_done
d_file_open:
        DGet R2,d_token
        DNe R2,T_STR,d_syntax
        DGet R2,d_length
        ADI R3,RZR,64
        DLt R2,R3,d_file_path_ok
        JMP d_parameter
d_file_path_ok:
        TrCall d_file_literal
        DSeq d_ops_5,1 ; PSH path
        DExpect 44
        TrCall d_expression
        DSeq d_file_value,2 ; ADD R3,R2,RZR / POP R2
        DExpect 41
        DRun rt_fopen
        JMP d_file_done
d_file_result:
        DRun rt_ioresult
        JMP d_return
d_file_done:
        TrCall d_leave
        JMP d_return

; R6 holds this call's handle, with the enclosing call's value on the stack.
; A linked list of native branches skips every remaining argument after error.
d_file_write:
        DSeq d_file_context,2 ; PSH R6 / ADD R6,R2,RZR
        ADI R11,RZR,0
        DRun rt_fwrite_begin
        TrCall d_file_failure
        ADD R11,R2,RZR
d_file_write_more:
        DGet R2,d_token
        DEq R2,41,d_file_write_close
        DExpect 44
        DGet R2,d_token
        DEq R2,T_STR,d_file_write_string
        TrCall d_expression
        DRun rt_fputint
        JMP d_file_write_check
d_file_write_string:
        TrCall d_file_literal
        DRun rt_fputstr
d_file_write_check:
        TrCall d_file_failure
        ADD R11,R2,RZR
        JMP d_file_write_more
d_file_write_close:
        DExpect 41
        DNe R8,T_FWRITELN,d_file_write_patches
        DRun rt_fnewline
d_file_write_patches:
        DEq R11,0,d_file_write_end
        TrConst R2,90000h
        ADD R2,R2,R11
        LRM R9,[R2]
        ADD R2,R11,RZR
        DGet R3,d_size
        TrCall d_patch
        ADD R11,R9,RZR
        JMP d_file_write_patches
d_file_write_end:
        DSeq d_file_restore,1 ; POP R6
        JMP d_file_done
d_file_failure:
        DBegin
        DEmit 010400000h ; SRZ R2
        DGet R9,d_size
        ADD R2,R11,RZR
        TrCall d_emit
        ; DBegin preserves R11: return the new chain head through R2.
        ADD R2,R9,RZR
        JMP d_return

; Emit a literal behind a native jump, then load its physical address into R2.
d_file_literal:
        DBegin
        DGet R8,d_text
        DGet R9,d_length
        ADI R2,RZR,0
        TrCall d_jump
        ADD R10,R2,RZR
        DGet R11,d_size
d_file_literal_loop:
        DEq R9,0,d_file_literal_end
        LRM R2,[R8]
        TrCall d_emit
        ADI R8,R8,4
        ADI R9,R9,-1
        JMP d_file_literal_loop
d_file_literal_end:
        ADI R2,RZR,0
        TrCall d_emit
        ADD R2,R10,RZR
        DGet R3,d_size
        TrCall d_patch
        TrConst R2,80000h
        ADD R2,R2,R11
        ADI R3,RZR,2
        TrCall d_constant
        TrCall d_next
        JMP d_return
d_file_value: DD 040627800h,020400000h
d_file_context: DD 00cc00000h,040c27800h
d_file_restore: DD 020c00000h
