; 16 битное деление

div16:
    MOV  A,H
    ORA  L
    RZ
    LXI  B,0000
    PUSH B
div16_1:
    MOV  A,E
    SUB  L
    MOV  A,D
    SBB  H
    JC   div16_2
    PUSH H
    DAD  H
    JNC  div16_1
div16_2:
    LXI  H,0000
div16_3:
    POP  B
    MOV  A, B
    ORA  C
    RZ
    DAD  H
    PUSH D
    MOV  A, E
    SUB  C
    MOV  E, A
    MOV  A, D
    SBB  B
    MOV  D, A
    JC   div16_4
    INX  H
    POP  B
    JMP  div16_3
div16_4:
    POP  D
    JMP  div16_3

