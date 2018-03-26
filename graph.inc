SYS_BACKUP_FROM = 0C800h
SYS_BACKUP_SIZE = 80h
SYS_BACKUP_TO   = 3h

NEW_BACKUP_FROM = 0C000h

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
    lhld level
    mvi  h, 0
    dad  h
    dad  h
    dad  h
    lxi  d, levels
    dad  d
    mov  e, m
    inx  h
    mov  d, m
    inx  h    
    mov  a, m
    sta  redrawNewFigure1_1
    inx  h
    shld palette

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
    inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_0
    inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_1
    inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_2
    inx  h
    mov  a, m
    ora  b
    out  IO_PALETTE_3
    ret

;-------------------------------------------------------------------------------
; Перерисовать текущий счет

redrawScore:
    lhld palette
    mov  a, m
    ora  a
    cnz  negativeText_F798
    cz   normalText_F793

    ; Переместить курсор в строку
    mvi  a, 1
    call setCursorY_F7DC

    ; Столбец для вывода первого символа
    mvi  b, 61

    ; Очки в DE
    lhld score
    xchg

redrawScore_1:
    push b
    ; Переместить крурсор в нужный столбец
    push h
    mov  a, b
    call setCursorX_F7BE
    pop  h
    ; Разделить ВУ на 10
    lxi  h, 10
    call div16
    ; Остаток от деления вывести на экран
    push h
    mov  a, e
    adi  '0'
    call drawChar1_F7FB
    pop  d
    pop  b
    ; Положение следующего символа
    dcr b
    ; Если DE=0 выходим
    mov  a, d
    ora  e
    jnz redrawScore_1
    ret

;-------------------------------------------------------------------------------
; Залить место экрана где выводится новая фигура

clearNewFigure:
    lxi  h, NEW_BACKUP_FROM
    lxi  d, 0B8F8h-48
    call clearNewFigure_0
    lxi  d, 0F8F8h-48
clearNewFigure_0:
    mvi  b, 7
clearNewFigure_1:
    push d
    mvi  c, 48
clearNewFigure_2:
    nop
    call memcpy8
clearNewFigure_3:
    nop
    pop  d
    inr  d
    dcr  b
    jnz  clearNewFigure_1
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
    mov  e, a

    ; Расчет координаты X
    mov  a, d
    adi  10
    mov  d, a

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
