; Example ROM: initialize R1, then increment forever.
    ADI R1, RZR, 5
loop:
    ADI R1, R1, 1
    JMP loop
