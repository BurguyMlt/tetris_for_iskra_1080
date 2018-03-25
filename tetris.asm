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

    ; Игра
    jmp  tetris

;----------------------------------------------------------------------------------------------------------------------

.include "tetris.inc"
.include "bios.inc"
.include "div16.inc"
.include "graph.inc"
.include "intro.inc"
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

;----------------------------------------------------------------------------------------------------------------------

.include "intro.inc"
packedLogo:
.include "graph/logo.inc"

;----------------------------------------------------------------------------------------------------------------------

end1:
make_binary_file "tetris.lvt", fileStart, end1
.end
