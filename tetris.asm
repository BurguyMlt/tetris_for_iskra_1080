; 2-04-2018 Aleksey Morozov aka Alemorf
; aleksey.f.morozov@gmail.com

; TETRIS WAS INVENTED BY A 30-YEAR-OLD SOVIET RESEARCHER NAMED ALEXEY PAZHITNOV
; WHO CURRENTLY WORKS AT THE COMPUTER CENTRE(ACADEMY SOFT) OF THE USSR ACADEMY
; OF SCIENCES IN MOSCOW. THE ORIGINAL PROGRAMMER WAS 18-YO VADIM GERASIMOV. A
; STUDENT STUDYING COMPUTER INFORMATICS AT MOSCOW UNIVERSITY. NOW YOU CAN ENJOY
; TETRIS BECAUSE OF THE JOINT EFFORTS OF ACADEMY SOFT, MOSCOW, ANDROMEDA
; SOFTWARE LTD, LONDON, ALEMORF, SPB AND SPECTRUM HOLOBYTE, USA.

; VERSION            PROGRAMMER
; ISKRA 1080 TARTU   ALEKSEY MOROZOV
; IBM CGA            ENG AN JIO
; RAM RESIDENT       ERICK JAP
; TANDY              BILLY SUTYONO
; IBM EGA            ARYANTO WIDODO
; GRAPHICS           DAN GUERRA
; PRODUCT MANAGER    R. ANTON WIDJAJA
; PRODUCER           SEAN B. BARGER

; igrab - 1406, 592
; 

.i8080

; Карта памяти после загрузки
; 0100h - 8C00h Код игры
; 8С00h - 93FEh Сжатая заставка интро заехала в видеопамять
; 9000h - BFFFh (Видеопамять)
; C800h - C87Fh (Системные переменные)
; C880h - C900h (Стек)
; D000h - FFFFh (Видеопамять)

; Карта памяти после переноса
; 0003h - 00CAh Кеш игового поля (10x20), временное место хранения системных переменных (80h)
; 00CBh - 00FFh (Стек)
; 0100h - 8C00h Код игры
; 9000h - BFFFh (Видеопамять)
; С000h - C7FFh Сжатая заставка интро, изображения для игры
; C800h - C87Fh (Системные переменные)
; C880h - CFFFh Изображения для интро
; D000h - FFFFh (Видеопамять)

;----------------------------------------------------------------------------------------------------------------------

org 234
fileStart:
.db 4ch, 56h, 4fh, 56h, 2fh, 32h, 2eh, 30h, 2fh, 0d0h, "TETRIS"
.dw entry
.dw end1
.dw entry

OPCODE_MVI_M = 036h

;----------------------------------------------------------------------------------------------------------------------

entry:
    ; Инициализируем стек
    lxi sp, 100h

    ; Вступление
    call intro

    ; Что бы не было видно мусора с неинициализированного экрана
    xra  a
    sta  palette

    ; Игра
restartGame:
    call tetris

    ; Очистить прямоугольник
    lxi  h, OPCODE_MVI_M
    call copyImageTo
    lxi  h, 0FFFFh - 135 - (32 * 256)
    lxi  b, 18 * 256 + 30 ; width * 256 + height
    lxi  d, 0
    call copyImage

    ; Выводим надпись "GAME OVER"
    mvi  a, 2
    call setTextColor
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

;----------------------------------------------------------------------------------------------------------------------

gameOverText:
    db 9,20,"GAME OVER",0

;----------------------------------------------------------------------------------------------------------------------

drawText:
    call drawText2
    jnz  drawText
    ret

drawText2:
    mov  a, m
    ora  a
    rz
    inx  h
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
    mov  a, m
    inx  h
drawText_1:
    push h
    call drawChar
    dcr  m
    pop  h
    xra  a ; return nz
    inr  a
    ret

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
OPCODE_XRA_A = 0AFh

;setTextColorNt:
;    mvi  a, 1
;    lxi  h, OPCODE_NOP + (OPCODE_NOP * 256)
;    lxi  d, OPCODE_XRA_A + (OPCODE_NOP * 256)
;    jmp  setTextColor1

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
       db 0,3,4
       dw level2
       db 0,2,4
       dw level3
       db 0,3,5
       dw level4
       db 0,3,4
       dw level5
       db 0,3,4
       dw level6
       db 3,3,4

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
font:
.include "graph/font.inc"

;----------------------------------------------------------------------------------------------------------------------

.include "intro.inc"
plane:
.include "graph/plane.inc"
igrab:
.include "graph/igrab.inc"
packedLogo:
.include "graph/logo.inc"
plane_end:

;----------------------------------------------------------------------------------------------------------------------

end1:
make_binary_file "tetris.lvt", fileStart, end1
.end
