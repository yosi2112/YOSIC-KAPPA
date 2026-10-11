; Read a confirmation line, sharing CR/LF state with the command editor.
; Returns1 for Y,0 for N,-1 for EOF/Ctrl-C; no host-side input.
core_answer:
        ADI R7,RZR,-1
answer_poll:
        LRM R0,[R10]
        ADI R1,RZR,1
        ANL R2,R0,R1
        SRZ R2
        JMP answer_receive
        ADI R1,RZR,4
        ANL R2,R0,R1
        SRZ R2
        JMP answer_done
        JMP answer_poll
answer_receive:
        LRM R1,[R10+4]
        ADI R2,R1,-10
        SRZ R2
        JMP answer_not_lf
        SRZ R6
        JMP answer_skip_lf
        JMP answer_done
answer_skip_lf:
        ADI R6,RZR,0
        JMP answer_poll
answer_not_lf:
        ADI R6,RZR,0
        ADI R2,R1,-13
        SRZ R2
        JMP answer_char
        ADI R6,RZR,1
        JMP answer_done
answer_char:
        SRM R1,[R10+8]
        ADI R2,R1,-3
        SRZ R2
        JMP answer_letter
        ADI R7,RZR,-1
        JMP answer_done
answer_letter:
        ADI R2,R7,1
        SRZ R2
        JMP answer_poll
        PSH R7
        Upper
        POP R7
        ADI R2,R1,-'Y'
        SRZ R2
        JMP answer_no
        ADI R7,RZR,1
        JMP answer_poll
answer_no:
        ADI R2,R1,-'N'
        SNZ R2
        ADI R7,RZR,0
        JMP answer_poll
answer_done:
        NewLine
        ADD R0,R7,RZR
        JMP return_sub
