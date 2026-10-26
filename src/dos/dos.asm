; YOSIC-DOS 1.0K: native CS=2 kernel and disk-loaded YKE1 applications.
; DS/SS stay zero. One input byte occupies one aligned RAM word.
; Persistent registers: R3 length, R4 write pointer, R5 rejected line,
; R6 pending CR, R10 physical console, R11 256, R12 code bias.
; R0-R2/R7-R9 are scratch. No CPU extensions or host command execution.
        CPU YOSIC-KAPPA
Buffer  EQU 4000h
PutChar MACRO value
        ADI R1,RZR,value
        SRM R1,[R10+8]
        ENDM
PutText MACRO text
        IRPC ch,text
        PutChar 'CH'
        ENDM
        ENDM
NewLine MACRO
        PutChar 13
        PutChar 10
        ENDM
; Fold only ASCII a-z. R1 is a byte, so bit 8 detects subtraction underflow.
; Clobbers R2/R7; leaves the original input buffer intact.
Upper   MACRO
        ADI R2,R1,-97
        ANL R7,R2,R11
        SRZ R7
        JMP folded
        ADI R2,R2,-26
        ANL R7,R2,R11
        SNZ R7
        JMP folded
        ADI R1,R1,-32
folded:
        ENDM
; R8 points to a candidate token. On match R9 points to its delimiter.
Match   MACRO word,target,dotted=0
        ADD R9,R8,RZR
        IRPC ch,word
        LRM R1,[R9]
        Upper
        ADI R2,R1,-'CH'
        SRZ R2
        JMP mismatch
        ADI R9,R9,4
        ENDM
        LRM R1,[R9]
        SNZ R1
        JMP target
        ADI R2,R1,-32
        SNZ R2
        JMP target
        ADI R2,R1,-9
        SNZ R2
        JMP target
        IF dotted
        ADI R2,R1,-46
        SNZ R2
        JMP target
        ENDIF
mismatch:
        ENDM

        INCLUDE runtime.inc
        JMP entry
return_sub:
        POP R2
        SRM R2,[R12+ret_slot-8000h]
ret_slot:
        JMP ret_slot

entry:
        ADI R10,RZR,0feh
        REPT 16
        ADD R10,R10,R10
        ENDM
        ADI R11,RZR,256
        Const R12,28000h
        Call core_init
        ADI R6,RZR,0
        PutText "YOSIC-DOS 1.0K"
        NewLine

new_line:
        Call submit_next
        SNZ R0
        JMP console_line
        ADI R8,RZR,Buffer
        JMP skip_space
console_line:
        ADI R3,RZR,0
        ADI R4,RZR,Buffer
        ADI R5,RZR,0
prompt:
        Call core_prompt_drive
        PutChar 58
        PutChar 62
poll:
        LRM R0,[R10]
        ADI R9,RZR,1
        ANL R2,R0,R9
        SRZ R2
        JMP receive
        ADI R9,RZR,4
        ANL R2,R0,R9
        SNZ R2
        JMP poll
        ORR R2,R3,R5
        SRZ R2
        JMP finish_line
; Batch input has reached EOF; retain the shell state at this address.
input_done:
        JMP input_done

receive:
        LRM R1,[R10+4]
        ADI R2,R1,-3
        SNZ R2
        JMP disk_reset
        ADI R2,R1,-10
        SNZ R2
        JMP line_feed
        ADI R6,RZR,0
        ADI R2,R1,-13
        SNZ R2
        JMP carriage_return
        ADI R2,R1,-8
        SNZ R2
        JMP backspace
        SNZ R1
        JMP invalid_byte
        SRM R1,[R10+8]
        SRZ R5
        JMP poll
        ADI R2,R3,-127
        SNZ R2
        JMP overflow
        SRM R1,[R4]
        ADI R4,R4,4
        ADI R3,R3,1
        JMP poll
invalid_byte:
        ADI R5,RZR,2
        JMP poll
overflow:
        ADI R5,RZR,1
        JMP poll
backspace:
        SRZ R5
        JMP poll
        SNZ R3
        JMP poll
        ADI R3,R3,-1
        ADI R4,R4,-4
        PutChar 8
        PutChar 32
        PutChar 8
        JMP poll
line_feed:
        SRZ R6
        JMP skip_lf
        JMP finish_line
skip_lf:
        ADI R6,RZR,0
        JMP poll
carriage_return:
        ADI R6,RZR,1
finish_line:
        SRM RZR,[R4]
        NewLine
        SRZ R5
        JMP rejected_line
        ADI R8,RZR,Buffer
skip_space:
        LRM R1,[R8]
        SNZ R1
        JMP new_line
        ADI R2,R1,-32
        SNZ R2
        JMP next_space
        ADI R2,R1,-9
        SNZ R2
        JMP next_space
        JMP dispatch
next_space:
        ADI R8,R8,4
        JMP skip_space
dispatch:
        Match "DIR",cmd_dir
        Match "MKDIR",cmd_mkdir
        Match "RMDIR",cmd_rmdir
        Match "DRIVE",cmd_drive
        Match "CHDIR",cmd_chdir
        Match "COPY",cmd_copy
        Match "MOVE",cmd_move
        Match "DELETE",cmd_delete
        Match "SYS",cmd_sys
        ; Builtins take precedence. Other first tokens name YKE files.
        PSH R8
        Call exec_command
        ADI SP,SP,4
        Call submit_result
        JMP new_line
cmd_dir: ADI R0,RZR,1
        JMP run_command
disk_reset:
        Call submit_reset
        NewLine
        Call core_init
        Call file_reset
        ADI R6,RZR,0
        JMP new_line
cmd_mkdir: ADI R0,RZR,2
        JMP run_command
cmd_rmdir: ADI R0,RZR,3
        JMP run_command
cmd_drive: ADI R0,RZR,4
        JMP run_command
cmd_copy: ADI R0,RZR,5
        JMP run_command
cmd_move: ADI R0,RZR,6
        JMP run_command
cmd_delete: ADI R0,RZR,7
        JMP run_command
cmd_sys: ADI R0,RZR,8
        JMP run_command
cmd_chdir: ADI R0,RZR,9
run_command:
        PSH R0
        PSH R9
        Call core_fs_command
        ADI SP,SP,8
        Const R1,13f00h
        SRM R0,[R1]
        Call submit_result
        JMP new_line
rejected_line:
        ADI R2,R5,-1
        SNZ R2
        JMP long_line
        PutText "Invalid input byte"
        NewLine
        JMP new_line
long_line:
        PutText "Input line too long"
        NewLine
        JMP new_line
        INCLUDE core.inc
        INCLUDE chdir.inc
        INCLUDE media.inc
        INCLUDE exec.inc
        INCLUDE media-return.inc
        INCLUDE submit.inc
        END
