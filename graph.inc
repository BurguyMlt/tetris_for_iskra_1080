SYS_BACKUP_FROM = 0C800h
SYS_BACKUP_SIZE = 80h
SYS_BACKUP_TO   = 3h

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
    shld palette

    ;push h

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
    ; Установить цвет фона 1, цвет текста 0
    call negativeText_F798

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
    xra  a
    lxi  d, 0B8F7h
    call clearNewFigure_0
    cma
    lxi  d, 0F8F7h
clearNewFigure_0:
    mvi  c, 48
clearNewFigure_1:
    mov  h, d
    mov  l, e
    mov  m, a
    inr  h
    mov  m, a
    inr  h
    mov  m, a
    inr  h
    mov  m, a
    inr  h
    mov  m, a
    inr  h
    mov  m, a
    inr  h
    mov  m, a
    dcr  e
    dcr  c
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
    mvi  a, 0
    call drawCell

    pop  b
    pop  d
    pop  h

    ; Продолжить рисование
    xra  a
    ret
