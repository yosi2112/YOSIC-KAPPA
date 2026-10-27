; Recursive descent. Expressions leave a signed, wrapped 16-bit value in R2.
; All helper calls preserve R6/R8..R13; nonlocal diagnostics restore initial SP.
d_program:
        DBegin
        TrCall d_next
        DExpect T_PROGRAM
        DExpect 59
d_declarations:
        DGet R2,d_token
        DEq R2,T_VAR,d_program_var
        DEq R2,T_FUNCTION,d_program_function
        DExpect T_BEGIN
        DGet R3,d_size
        ADI R2,RZR,rt_main_jump-rt_base
        TrCall d_patch
        TrCall d_statements
        DExpect T_END
        DExpect 46
        DExpect 0
        DRun rt_finish
        JMP d_return
d_program_var:
        TrCall d_variables
        JMP d_declarations
d_program_function:
        TrCall d_function
        JMP d_declarations
d_variables:
        DBegin
        DExpect T_VAR
d_variables_loop:
        DGet R8,d_scope
        DEq R8,0,d_variable_global
        DGet R9,d_locals
        DEq R9,33,d_limit
        ADI R9,R9,1
        DPut R9,d_locals
        ADD R3,R9,R9
        ADD R3,R3,R3
        SUB R3,RZR,R3
        ADI R2,RZR,3
        JMP d_variable_add
d_variable_global:
        DGet R9,d_globals
        ADD R3,R9,R9
        ADD R3,R3,R3
        ADI R9,R9,1
        DPut R9,d_globals
        ADI R2,RZR,1
d_variable_add:
        TrCall d_add
        TrCall d_next
        DGet R2,d_token
        DNe R2,44,d_variable_done
        TrCall d_next
        JMP d_variables_loop
d_variable_done:
        DExpect 59
        JMP d_return

d_function:
        DBegin
        DExpect T_FUNCTION
        DGet R3,d_size
        ADI R2,RZR,2
        TrCall d_add
        ADD R8,R2,RZR
        DPut R8,d_scope
        DGet R6,d_symbols
        TrCall d_next
        ; RESULT occupies the first local frame word; not a language keyword.
        DGet R9,d_text
        DGet R10,d_length
        DGet R11,d_token
        DAddr R2,d_result_name
        DPut R2,d_text
        DSet d_length,6
        DSet d_token,T_ID
        ADI R2,RZR,3
        ADI R3,RZR,-4
        TrCall d_add
        DPut R9,d_text
        DPut R10,d_length
        DPut R11,d_token
        DSet d_locals,1
        DSet d_params,0
        DGet R2,d_token
        DNe R2,40,d_function_params_done
        TrCall d_next
d_function_params:
        DGet R9,d_params
        DEq R9,16,d_limit
        ADI R9,R9,1
        DPut R9,d_params
        ADI R2,RZR,3
        ADD R3,R9,RZR
        TrCall d_add
        TrCall d_next
        DGet R2,d_token
        DNe R2,44,d_function_params_close
        TrCall d_next
        JMP d_function_params
d_function_params_close:
        DExpect 41
d_function_params_done:
        DGet R9,d_params
        SRM R9,[R8+16]
        ; Parameter index i becomes frame offset (argc-i+2)*4.
        ADI R10,R8,48
        ADD R11,R9,RZR
d_function_param_offsets:
        DEq R11,0,d_function_vars
        ADI R2,R11,1
        ADD R2,R2,R2
        ADD R2,R2,R2
        SRM R2,[R10+12]
        ADI R10,R10,24
        ADI R11,R11,-1
        JMP d_function_param_offsets
d_function_vars:
        DExpect 59
d_function_var_loop:
        DGet R2,d_token
        DNe R2,T_VAR,d_function_body
        TrCall d_variables
        JMP d_function_var_loop
d_function_body:
        DExpect T_BEGIN
        DRun rt_guard
        DSeq d_ops_0,2 ; PSH R10 / ADD R10,SP,RZR
        DGet R2,d_locals
        ADD R2,R2,R2
        ADD R2,R2,R2
        SUB R2,RZR,R2
        TrConst R3,044000000h+(14<<21)+(14<<16)
        TrConst R4,0ffffh
        ANL R2,R2,R4
        ORR R2,R2,R3
        TrCall d_emit
        TrCall d_statements
        DExpect T_END
        DExpect 59
        DSeq d_ops_1,4 ; LRM R2,[R10-4] / ADD SP,R10,RZR / POP R10 / POP R7
        DGet R2,d_params
        ADD R2,R2,R2
        ADD R2,R2,R2
        TrConst R3,044000000h+(14<<21)+(14<<16)
        ORR R2,R2,R3
        TrCall d_emit
        DRunTail rt_return_register
        DPut R6,d_symbols
        DSet d_scope,0
        JMP d_return

d_statements:
        DBegin
        TrCall d_statement
d_statements_more:
        DGet R2,d_token
        DNe R2,59,d_statements_done
        TrCall d_next
        TrCall d_statement
        JMP d_statements_more
d_statements_done:
        DNe R2,T_END,d_syntax
        JMP d_return

d_statement:
        DBegin
        ADI R6,RZR,0
        DGet R8,d_token
        DEq R8,T_BEGIN,d_statement_block
        DEq R8,T_IF,d_statement_if
        DEq R8,T_WHILE,d_statement_while
        DEq R8,T_ID,d_statement_id
        DEq R8,T_WRITE,d_statement_builtin
        DEq R8,T_WRITELN,d_statement_builtin
        DEq R8,T_GET,d_statement_builtin
        DEq R8,T_CONTINUE,d_statement_builtin
        DEq R8,T_BREAK,d_statement_builtin
        ADI R2,RZR,T_FOPEN
        DLt R8,R2,d_statement_empty
        ADI R2,RZR,T_IORESULT+1
        DLt R8,R2,d_statement_builtin
d_statement_empty:
        ; Empty statements are valid only at actual statement delimiters.
        DEq R8,59,d_statement_done
        DEq R8,T_END,d_statement_done
        DEq R8,T_ELSE,d_statement_done
        JMP d_syntax
d_statement_block:
        TrCall d_enter
        ADI R6,RZR,1
        TrCall d_next
        TrCall d_statements
        DExpect T_END
        JMP d_statement_done
d_statement_id:
        TrCall d_lookup
        DEq R2,0,d_undefined
        ADD R9,R2,RZR
        LRM R8,[R9+8]
        DEq R8,2,d_statement_call
        TrCall d_next
        DExpect T_ASSIGN
        TrCall d_expression
        TrCall d_store_symbol
        JMP d_statement_done
d_statement_call:
        ADD R2,R9,RZR
        TrCall d_user_call
        JMP d_statement_done
d_statement_builtin:
        TrCall d_builtin
        JMP d_statement_done
d_statement_if:
        TrCall d_enter
        ADI R6,RZR,1
        TrCall d_next
        TrCall d_expression
        DExpect T_THEN
        DSeq d_ops_2,1 ; SNZ R2
        ADI R2,RZR,0
        TrCall d_jump
        ADD R8,R2,RZR
        TrCall d_statement
        DGet R2,d_token
        DNe R2,T_ELSE,d_if_without_else
        TrCall d_next
        ADI R2,RZR,0
        TrCall d_jump
        ADD R9,R2,RZR
        ADD R2,R8,RZR
        DGet R3,d_size
        TrCall d_patch
        TrCall d_statement
        ADD R2,R9,RZR
        DGet R3,d_size
        TrCall d_patch
        JMP d_statement_done
d_if_without_else:
        ADD R2,R8,RZR
        DGet R3,d_size
        TrCall d_patch
        JMP d_statement_done
d_statement_while:
        TrCall d_enter
        ADI R6,RZR,1
        DGet R8,d_loop
        DGet R9,d_breaks
        ; R8 anchors the native loop stack. Nested loops save/restore it.
        DSeq d_ops_3,2 ; PSH R8 / ADD R8,SP,RZR
        DGet R10,d_size
        DPut R10,d_loop
        DSet d_breaks,0
        TrCall d_next
        TrCall d_expression
        DExpect T_DO
        DSeq d_ops_2,1 ; SNZ R2
        ADI R2,RZR,0
        TrCall d_emit
        DPut R2,d_breaks
        TrCall d_statement
        ADD R2,R10,RZR
        TrCall d_jump
        DGet R10,d_breaks
d_while_patch:
        DEq R10,0,d_while_done
        TrConst R2,90000h
        ADD R2,R2,R10
        LRM R11,[R2]
        ADD R2,R10,RZR
        DGet R3,d_size
        TrCall d_patch
        ADD R10,R11,RZR
        JMP d_while_patch
d_while_done:
        DSeq d_ops_4,1 ; POP R8
        DPut R8,d_loop
        DPut R9,d_breaks
d_statement_done:
        DEq R6,0,d_return
        TrCall d_leave
        JMP d_return

; Symbol held in R9 survives expression emission. R2 result is in generated code.
d_store_symbol:
        DBegin
        LRM R8,[R9+8]
        LRM R2,[R9+12]
        DEq R8,1,d_store_global
        TrConst R3,08000000h+(2<<21)+(10<<16)
        JMP d_symbol_memory
d_store_global:
        TrConst R3,08000000h+(2<<21)+(11<<16)
        JMP d_symbol_memory
d_load_symbol:
        DBegin
        LRM R8,[R2+8]
        LRM R2,[R2+12]
        DEq R8,1,d_load_global
        TrConst R3,04000000h+(2<<21)+(10<<16)
        JMP d_symbol_memory
d_load_global:
        TrConst R3,04000000h+(2<<21)+(11<<16)
d_symbol_memory:
        TrConst R4,0ffffh
        ANL R2,R2,R4
        ORR R2,R2,R3
        TrCall d_emit
        JMP d_return

d_expression:
        DBegin
        TrCall d_sum
        DGet R8,d_token
        DEq R8,61,d_expression_compare
        DEq R8,60,d_expression_compare
        DEq R8,62,d_expression_compare
        DEq R8,T_LE,d_expression_compare
        DEq R8,T_GE,d_expression_compare
        DEq R8,T_NE,d_expression_compare
        JMP d_return
d_expression_compare:
        DSeq d_ops_5,1 ; PSH R2
        TrCall d_next
        TrCall d_sum
        DSeq d_ops_6,1 ; POP R3
        ADD R2,R8,RZR
        ADI R3,RZR,4
        TrCall d_constant
        DRun rt_compare
        JMP d_return
d_sum:
        DBegin
        TrCall d_term
d_sum_more:
        DGet R8,d_token
        DEq R8,43,d_sum_op
        DEq R8,45,d_sum_op
        JMP d_return
d_sum_op:
        DSeq d_ops_5,1 ; PSH R2
        TrCall d_next
        TrCall d_term
        DSeq d_ops_6,1 ; POP R3
        DEq R8,45,d_sum_sub
        DSeq d_ops_7,1 ; ADD R2,R3,R2
        JMP d_sum_wrap
d_sum_sub:
        DSeq d_ops_8,1 ; SUB R2,R3,R2
d_sum_wrap:
        DRun rt_wrap
        JMP d_sum_more
d_term:
        DBegin
        TrCall d_factor
d_term_more:
        DGet R8,d_token
        DEq R8,42,d_term_op
        DEq R8,47,d_term_op
        DEq R8,37,d_term_op
        JMP d_return
d_term_op:
        DSeq d_ops_5,1 ; PSH R2
        TrCall d_next
        TrCall d_factor
        DSeq d_ops_6,1 ; POP R3
        DEq R8,42,d_term_mul
        DRun rt_divide
        DNe R8,37,d_term_more
        DSeq d_ops_9,1 ; ADD R2,R3,RZR
        JMP d_term_more
d_term_mul:
        DRun rt_multiply
        JMP d_term_more
d_factor:
        DBegin
        ADI R8,RZR,0
        DGet R2,d_token
        DNe R2,45,d_factor_kind
        ADI R8,RZR,1
        TrCall d_next
d_factor_kind:
        DGet R2,d_token
        DEq R2,T_INT,d_factor_number
        DEq R2,T_ID,d_factor_id
        DEq R2,40,d_factor_parens
        DEq R2,T_GET,d_factor_builtin
        DEq R2,T_WRITE,d_factor_builtin
        DEq R2,T_WRITELN,d_factor_builtin
        DEq R2,T_BREAK,d_factor_builtin
        DEq R2,T_CONTINUE,d_factor_builtin
        ADI R3,RZR,T_FOPEN
        DLt R2,R3,d_factor_invalid
        ADI R3,RZR,T_IORESULT+1
        DLt R2,R3,d_factor_builtin
d_factor_invalid:
        JMP d_syntax
d_factor_builtin:
        TrCall d_builtin
        JMP d_factor_sign
d_factor_number:
        DGet R2,d_value
        ADI R3,RZR,2
        TrCall d_constant
        DRun rt_wrap
        TrCall d_next
        JMP d_factor_sign
d_factor_id:
        TrCall d_lookup
        DEq R2,0,d_undefined
        LRM R3,[R2+8]
        DEq R3,2,d_factor_call
        TrCall d_load_symbol
        TrCall d_next
        JMP d_factor_sign
d_factor_call:
        TrCall d_user_call
        JMP d_factor_sign
d_factor_parens:
        TrCall d_enter
        TrCall d_next
        TrCall d_expression
        DExpect 41
        TrCall d_leave
d_factor_sign:
        DEq R8,0,d_factor_done
        DSeq d_ops_10,1 ; SUB R2,RZR,R2
        DRun rt_wrap
d_factor_done:
        JMP d_return

; R2 function record. Arguments are evaluated and pushed left-to-right.
d_user_call:
        DBegin
        ADD R8,R2,RZR
        TrCall d_enter
        ADI R9,RZR,0
        TrCall d_next
        DGet R2,d_token
        DNe R2,40,d_user_call_emit
        TrCall d_next
d_user_call_args:
        TrCall d_expression
        DSeq d_ops_5,1 ; PSH R2
        ADI R9,R9,1
        DEq R9,17,d_arity
        DGet R2,d_token
        DNe R2,44,d_user_call_close
        TrCall d_next
        JMP d_user_call_args
d_user_call_close:
        DExpect 41
d_user_call_emit:
        LRM R2,[R8+16]
        SUB R0,R2,R9
        SRZ R0
        JMP d_arity
        LRM R2,[R8+12]
        TrCall d_call
        TrCall d_leave
        JMP d_return

d_builtin:
        DBegin
        DGet R8,d_token
        ADI R2,RZR,T_FOPEN
        DLt R8,R2,d_builtin_original
        TrCall d_file_builtin
        JMP d_return
d_builtin_original:
        TrCall d_next
        DEq R8,T_GET,d_builtin_get
        DEq R8,T_CONTINUE,d_builtin_continue
        DEq R8,T_BREAK,d_builtin_break
        DGet R2,d_token
        DNe R2,40,d_builtin_write_done
        TrCall d_enter
        TrCall d_next
d_builtin_write_args:
        DGet R2,d_token
        DEq R2,T_STR,d_builtin_string
        TrCall d_expression
        DRun rt_putint
        JMP d_builtin_write_more
d_builtin_string:
        DGet R9,d_text
        DGet R10,d_length
        ADI R2,RZR,0
        TrCall d_jump
        ADD R11,R2,RZR
        DGet R6,d_size
d_builtin_string_emit:
        DEq R10,0,d_builtin_string_end
        LRM R2,[R9]
        TrCall d_emit
        ADI R9,R9,4
        ADI R10,R10,-1
        JMP d_builtin_string_emit
d_builtin_string_end:
        ADI R2,RZR,0
        TrCall d_emit
        ADD R2,R11,RZR
        DGet R3,d_size
        TrCall d_patch
        TrConst R2,80000h
        ADD R2,R2,R6
        ADI R3,RZR,2
        TrCall d_constant
        DRun rt_putstr
        TrCall d_next
d_builtin_write_more:
        DGet R2,d_token
        DNe R2,44,d_builtin_write_close
        TrCall d_next
        JMP d_builtin_write_args
d_builtin_write_close:
        DExpect 41
        TrCall d_leave
d_builtin_write_done:
        DNe R8,T_WRITELN,d_builtin_zero
        DRun rt_newline
d_builtin_zero:
        DSeq d_ops_11,1 ; ADI R2,RZR,0
        JMP d_return
d_builtin_get:
        DRun rt_getint
        JMP d_return
d_builtin_continue:
        DGet R2,d_loop
        DEq R2,0,d_outside_loop
        DSeq d_ops_12,1 ; ADD SP,R8,RZR
        DGet R2,d_loop
        TrCall d_jump
        JMP d_return
d_builtin_break:
        DGet R2,d_loop
        DEq R2,0,d_outside_loop
        DSeq d_ops_12,1 ; ADD SP,R8,RZR
        DGet R8,d_size
        DGet R2,d_breaks
        TrCall d_emit
        DPut R8,d_breaks
        JMP d_return
