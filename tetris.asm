.i8080

org 234
fileStart:
.db 4ch, 56h, 4fh, 56h, 2fh, 32h, 2eh, 30h, 2fh, 0d0h, "TETRIS"
.dw entry
.dw end1
.dw entry

;----------------------------------------------------------------------------------------------------------------------

entry:
    ; Инициализируем стек
    lxi sp, 100h

    ; Режим экрана 64 символа в ширину
    mvi  a, 40h
    sta  videoMode_C802

    ; Вступление
    call intro

    ; Что бы не было видно мусора с неинициализированного экрана
    xra  a
    sta  palette

    ; Игра
restartGame:
    call tetris

    ; Выводим надпись "GAME OVER"
    call setTextColorNt
    lxi  h, gameOverText
    call drawText

    ; Ждем нажатия Enter
entry_1:
    call rand
    call inkey_FC12
    cpi  13
    jnz  entry_1

    ;
    jmp  restartGame

gameOverText:
    db 9,19," GAME OVER ",0

;----------------------------------------------------------------------------------------------------------------------

drawText_1:
    push h
    call drawChar
    dcr  m
    pop  h
drawText:
    mov  a, m
    inx  h
    ora  a
    rz
    cpi  32
    jnc  drawText_1
    mov  e, a
    add  a ; *2
    add  e ; *3
    add  a ; *6
    add  e ; *7
    add  a ; *14
    cma
    sta  drawCharAddr
    mvi  a, 0FFh
    sub  m
    inx  h
    sta  drawCharAddr+1
    jmp  drawText
;----------------------------------------------------------------------------------------------------------------------

pressAnyKey:
    ; Ждем пока пользователь отпустит клавишу
pressAnyKey_1:
    call inkey_FC12
    cpi  0FFh
    jnz  pressAnyKey_1
pressAnyKey_2:
    call rand
    call inkey_FC12
    cpi  0FFh
    jz   pressAnyKey_2
    ret

;-------------------------------------------------------------------------------

memcpy8back:
    ldax d
    mov m, a
    dcx d
    dcx h
    dcx b
    mov a, c
    ora b
    jnz memcpy8back
    ret

;-------------------------------------------------------------------------------

memcpy8:
    mov  a, m
    stax d
    inx  d
    inx  h
    dcr  c
    jnz  memcpy8
    ret

;-------------------------------------------------------------------------------

memset8:
    mov  m, a
    inx  h
    dcr  c
    jnz  memset8
    ret

;----------------------------------------------------------------------------------------------------------------------

delay8000:
    lxi  d, 08000h
delay:
    dcx  d
    mov  a, d
    ora  e
    jnz  delay
    ret

;----------------------------------------------------------------------------------------------------------------------

FONT_HEIGHT = 12

drawChar:
    ; Адрес символа в знакогенераторе
    ; de = font + a * 12
    mov  l, a
    mvi  h, 0
    dad  h ; *2
    dad  h ; *4
    mov  d, h
    mov  e, l
    dad  h ; *8
    dad  d ; *12
    lxi  d, font - (' ' * 12)
    dad  d
    xchg

    ; Адрес в видеопамяти
drawCharAddr = $+1
    lxi  h, 0D000h

    ; Цикл
    mvi  c, FONT_HEIGHT
drawChar_1:
    push b

    ; Первая плоскость, сразу результат не записываем, а сохраняем в C
    mov  b, m
    ldax d
drawChar_cma1:
    cma    ; nop
    ana  b ; ora b
    mov  c, a

    ; Сохраняем адрес первой плоскости и вычисялем адрес второй плоскости
    push h
    mov  a, h
    sbi  40h
    mov  h, a

    ; Вторая плоскость
    mov  b, m
    ldax d
drawChar_cma2:
    cma    ; nop
    ana  b ; ora b

    ; Записываем в память значения почти одновременно
    mov  m, a
    pop  h
    mov  m, c

    ; Следующий пиксель, следующий байт шрифта
    inx  h
    inx  d

    ; Цикл
    pop  b
    dcr  c
    jnz  drawChar_1

    ; Перемещаем курсор на следующий символ
    lxi h, drawCharAddr+1
    ret

;----------------------------------------------------------------------------------------------------------------------

OPCODE_NOP   = 0
OPCODE_CMA   = 2Fh
OPCODE_ANA_B = 0A0h
OPCODE_ORA_B = 0B0h

setTextColorNt:
    lxi  h, OPCODE_NOP + (OPCODE_NOP * 256)
    lxi  d, OPCODE_NOP + (OPCODE_NOP * 256)
    jmp  setTextColor1

;----------------------------------------------------------------------------------------------------------------------


setTextColor:
    lxi  h, OPCODE_CMA + (OPCODE_ANA_B * 256)
    lxi  d, OPCODE_NOP + (OPCODE_ORA_B * 256)
setTextColor1:
    rrc
    jnc  setTextColor_1
    xchg
    cma
setTextColor_1:
    shld drawChar_cma1
    rrc
    jnc  setTextColor_2
    xchg
setTextColor_2:
    shld drawChar_cma2
    ret

;----------------------------------------------------------------------------------------------------------------------

.include "tetris.inc"
.include "bios.inc"
.include "div16.inc"
.include "graph.inc"
.include "playfieldgraph.inc"
.include "rand.inc"
.include "unmlz.inc"

;----------------------------------------------------------------------------------------------------------------------

LEVELS_COUNT = 6

levels dw level1
       db 0,1,15,3,4,0
       dw level2
       db 0,1,15,2,4,0
       dw level3
       db 0,1,15,3,5,0
       dw level4
       db 0,1,15,3,4,0
       dw level5
       db 0,1,15,3,4,0
       dw level6
       db 3,0,15,3,4,0

;       dw level7
;       db 15,3,4,0,0,0

level1:
.include "graph/level1.inc"
level2:
.include "graph/level2.inc"
level3:
.include "graph/level3.inc"
level4:
.include "graph/level4.inc"
level5:
.include "graph/level5.inc"
level6:
.include "graph/level6.inc"
;level7:
;.include "graph/level7.inc"
font:
.include "graph/font.inc"

; Надо выиграть 1235 байт

;----------------------------------------------------------------------------------------------------------------------

.include "intro.inc"
packedLogo:
.include "graph/logo.inc"

;----------------------------------------------------------------------------------------------------------------------

end1:
make_binary_file "tetris.lvt", fileStart, end1
.end
