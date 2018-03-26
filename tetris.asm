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

    ; Выводим надпись "GAME OVER"  ;!!!! Заменить на более красивую надпись
    call normalText_F793
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
    db  12,27,"GAME OVER",0

;----------------------------------------------------------------------------------------------------------------------

drawText_1:
    call drawChar1_F7FB
drawText:
    mov  a, m
    inx  h
    ora  a
    rz
    cpi  32
    jnc  drawText_1
    call setCursorY_F7DC
    mov  a, m
    inx  h
    call setCursorX_F7BE
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
       db 15,3,4,0,0,0
       dw level2
       db 15,2,4,0,0,0
       dw level3
       db 15,3,5,0,0,0
       dw level4
       db 15,3,4,0,0,0
       dw level5
       db 15,3,4,0,0,0
       dw level6
       db 15,3,4,0,0,0
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

; Надо выиграть 1235 байт

;----------------------------------------------------------------------------------------------------------------------

.include "intro.inc"
packedLogo:
.include "graph/logo.inc"

;----------------------------------------------------------------------------------------------------------------------

end1:
make_binary_file "tetris.lvt", fileStart, end1
.end
