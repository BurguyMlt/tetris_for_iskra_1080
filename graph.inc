SYS_BACKUP_FROM = 0C800h
SYS_BACKUP_SIZE = 80h
SYS_BACKUP_TO   = 40h

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
    lxi d, SYS_BACKUP_FROM + SYS_BACKUP_SIZE - 1
    lxi h, SYS_BACKUP_TO + SYS_BACKUP_SIZE - 1
    lxi b, SYS_BACKUP_SIZE
    call copy

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
    lxi h, 0FFFFh
    lxi d, 0EFFFh
    lxi b, 03000h
    call copy
    ;pop  h

    ; Восстанавливаем системные переменные
    lxi d, SYS_BACKUP_TO + SYS_BACKUP_SIZE - 1
    lxi h, SYS_BACKUP_FROM + SYS_BACKUP_SIZE - 1
    lxi b, SYS_BACKUP_SIZE
    call copy

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
