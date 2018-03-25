;-------------------------------------------------------------------------------
; Рисование клетки

cellOff: db 240- 0,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240- 2,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 240- 3,003h,0FFh,0F0h ; 00000011,11111111,11110000
         db 240- 5,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 240- 7,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240- 9,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 240-10,003h,0FFh,0F0h ; 00000011,11111111,11110000
         db 240-12,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 240-14,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240-16,0FFh,0FCh,000h ; 11111111,11111100,00000000

         db 254- 0,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 254- 1,003h,0FFh,0F0h ; 00000011,11111111,11110000
         db 254- 3,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 254- 5,03Fh,0FFh,000h ; 00111111,11111111,00000000

drawCell:
    ; addr = cellOff[d]
    mov b, e
    lxi h, cellOff
    mov e, d
    mvi d, 0
    dad d
    dad d
    dad d
    dad d
    mov d, m
    mov e, b
    inx  h

    mov  c, a

    call clearCell3

    mov  a, c
    rrc
    mov  c, a

    mov  a, d
    sui  40h
    mov  d, a

    call clearCell3

    ret

;-------------------------------------------------------------------------------

clearCell3:
    push h
    push d

    mov  b, m
    call clearCell

    inx  h
    mov  b, m
    call clearCell

    inx  h
    mov  b, m
    call clearCell

    pop d
    pop h
    ret

;-------------------------------------------------------------------------------

clearCell:
    push d

    mov  a, c
    rrc
    jc   setCell

    mov  a, b
    cma
    mov  b, a

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d
    dcx  d

    ldax d
    ana  b
    stax d

    pop  d
    dcr  d

    ret

;-------------------------------------------------------------------------------

setCell:
    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d
    dcx  d

    ldax d
    ora  b
    stax d

    pop  d
    dcr  d

    ret

;-------------------------------------------------------------------------------

POS_Y = 8

redrawMap:
    ;var a = START;
    lxi b, map + START
    lxi h, map2

    ;for(var y=0; y<HEIGHT; y++, a++) {
    mvi e, 0FFh - POS_Y
redrawMap_1:

    ; for(var x=0; x<WIDTH; x++, a++) {
    mvi d, 0
redrawMap_2:

    ; var c = map[a]
    ; ctx.fillStyle = colors[c];
    ; ctx.fillRect(122+x*14, 8+y*12, 14, 12);
    ldax b
    cmp  m
    jz   redrawMap_3
    mov  m, a

    push b
    push d
    push h
    call drawCell
    pop  h
    pop  d
    pop  b

redrawMap_3:
    inr  d
    inx  b
    inx  h
    mov  a, d
    cpi  WIDTH
    jnz  redrawMap_2

    inx  b
    mov  a, e
    sui  12
    mov  e, a
    cpi  0FFh - POS_Y - (HEIGHT * 12)
    jnz  redrawMap_1
    ret

;-------------------------------------------------------------------------------
