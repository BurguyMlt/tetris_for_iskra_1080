SYS_BACKUP_FROM = 0C800h
SYS_BACKUP_SIZE = 80h
SYS_BACKUP_TO   = 3h

NEW_BACKUP_FROM = 0C000h
SCO_BACKUP_FROM = 0C000h + (7 * 48 * 2)

OPCODE_XCHG = 0EBh
OPCODE_NOP = 000h

;----------------------------------------------------------------------------
; Очистка экрана

clearScreen:
    call clearScreen1

;-V-V-V----------------------------------------------------------------------
; Очистка второй плоскости экрана (9000-BFFF)

clearScreen2:
    lxi  h, VIDEO_PAGE_1
    mvi  a, 0C0h

;-V-V-V----------------------------------------------------------------------
; Очистка памяти

clearMem:
    mvi  m, 0
    inx  h
    cmp  h
    jnz  clearMem
    ret

;-------------------------------------------------------------------------------
; Очистка первой плоскости экрана (В000-FFFF)

clearScreen1:
    lxi  h, VIDEO_PAGE_0
    xra  a
    jmp  clearMem

;-------------------------------------------------------------------------------
; Перерисовать фон

drawBackgrond:
    ; Сохраняем системные переменные
    lxi  d, SYS_BACKUP_FROM + SYS_BACKUP_SIZE - 1
    lxi  h, SYS_BACKUP_TO + SYS_BACKUP_SIZE - 1
    lxi  b, SYS_BACKUP_SIZE
    call memcpy8back

    ; Гасим экран
    mvi  b, 8
    call setPaletteEx
    call delay8000
    mvi  a, PALETTE_BLACK
    out  IO_PALETTE_0
    out  IO_PALETTE_1
    out  IO_PALETTE_2
    out  IO_PALETTE_3

    ; Получаем указатель на изображение уровня
    lda  level
    mov  d, a
    add  a ;*2
    add  a ;*4
    add  d ;*5
    mvi  d, 0
    mov  e, a
    lxi  h, levels
    dad  d
    mov  e, m
    inx  h
    mov  d, m
    inx  h
    mov  a, m ; Цвет фигуры и текста
    sta  redrawNewFigure1_1
    inx  h
    shld palette ; Палитра

    push d
    call setTextColor
    pop  d

    ; Распаковываем
    lxi  b, VIDEO_PAGE_1
    call unmlz

    ; Переносим вторую плоскость
    lxi  h, 0FFFFh
    lxi  d, 0EFFFh
    lxi  b, 03000h
    call memcpy8back
    ;pop  h

    ; Восстанавливаем системные переменные
    lxi  d, SYS_BACKUP_TO + SYS_BACKUP_SIZE - 1
    lxi  h, SYS_BACKUP_FROM + SYS_BACKUP_SIZE - 1
    lxi  b, SYS_BACKUP_SIZE
    call memcpy8back

    mvi a, OPCODE_XCHG
    sta clearNewFigure_2
    sta clearNewFigure_3
    call clearNewFigure
    call clearScore
    mvi a, OPCODE_NOP
    sta clearNewFigure_2
    sta clearNewFigure_3

    ret

;----------------------------------------------------------------------------------------------------------------------

paletteShow:
    mvi  b, 8
    call setPaletteEx
    call delay8000
    mvi  b, 0
setPaletteEx:
palette = $+1
    lxi  h, 0
    xra  a
    ora  l
    rz
    ;inx  h
    mvi  a, 15
    ora  b
    out  IO_PALETTE_0
    ;inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_1
    inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_2
    ;inx  h
    mov  a, b
    out  IO_PALETTE_3
    ret

;-------------------------------------------------------------------------------
; Перерисовать текущий счет

redrawScore:
    call clearScore

    lxi  h, 0FFFFh-24-(45*256)
    shld drawCharAddr
    lhld score
    call drawNumber

    lxi  h, 0FFFFh-40-(45*256)
    shld drawCharAddr
    lhld bestScore
    jmp  drawNumber

;-------------------------------------------------------------------------------
; Вывести число

drawNumber:
    xchg
redrawScore_1:
    ; Разделить ВУ на 10
    lxi  h, 10
    call div16
    ; Остаток от деления вывести на экран
    push h
    mov  a, e
    adi  '0'
    call drawChar
    inr  m
    pop  d
    ; Если DE=0 выходим
    mov  a, d
    ora  e
    jnz redrawScore_1
    ret

;-------------------------------------------------------------------------------

clearScore:
    lxi  b, 5 * 256 + 40
    lxi  h, SCO_BACKUP_FROM
    lxi  d, 0FFFFh-40-(45*256)
    jmp  copyImage

;-------------------------------------------------------------------------------
; Залить место экрана где выводится новая фигура

clearNewFigure:
    lxi  b, 7 * 256 + 48
    lxi  h, NEW_BACKUP_FROM
    lxi  d, 0F8F8h - 48
copyImage:
    push d
    call clearNewFigure_0
    pop  d
    mov  a, d
    sui  40h
    mov  d, a

clearNewFigure_0:
    push b
    ;mvi  b, 7
clearNewFigure_1:
    push d
    push b
    ;mvi  c, 48
clearNewFigure_2:
    nop
    call memcpy8
clearNewFigure_3:
    nop
    pop  b
    pop  d
    inr  d
    dcr  b
    jnz  clearNewFigure_1
    pop  b
    ret

;-------------------------------------------------------------------------------
; Нарисовать клетку новой фигуры.

redrawNewFigure1:
    push h
    push d
    push b

    ; Расчет координаты Y
    mov  a, e
    add  a
    add  a
    mov  e, a
    add  a
    add  e
    mov  e, a
    mvi  a, 0F7h
    sub  e
    mov  c, a

    ; Расчет координаты X
    mov  a, d
    adi  10
    mov  b, a

    ; Рисование клетки черным цветом
redrawNewFigure1_1 = $+1
    mvi  a, 0
    call drawCell

    pop  b
    pop  d
    pop  h

    ; Продолжить рисование
    xra  a
    ret
