LOGO_WIDTH  = 320
LOGO_HEIGHT = 166
LOGO_SIZE   = (LOGO_WIDTH / 8 * LOGO_HEIGHT)
LOGO_DEST   = (VIDEO_PAGE_0 + ((VIDEO_WIDTH - LOGO_WIDTH) / 2 / 8 * 256) - ((VIDEO_HEIGHT - LOGO_HEIGHT) / 2))

intro:
    ; Не инверсный текст
    call normalText_F793

    ; Черный фон. Красный текст. Гасим вторую плоскость видеопамяти.
    mvi  a, PALETTE_BLACK
    out  IO_PALETTE_0
    out  IO_PALETTE_1
    out  IO_PALETTE_2
    out  IO_PALETTE_3

    ; Очистка первой плоскости видеопамяти
    call clearScreen1

    ; Вывод текста
    lxi  h, introText
    call drawText

    ; Показываем текст
    mvi  a, PALETTE_DARK_RED
    mvi  b, PALETTE_RED
    call paletteEffect

    ; Ждем нажатия на любую клавиашу
    call pressAnyKey

    ; Гасим экран
introOff:
    call paletteOff

    ; При повторном вызове этой функции данные уже будут уничтожены, так как они находятся в видеопамяти
;    mvi a, 0C9h
;    sta introOff

    ; Распаковываем лого
    lxi  d, packedLogo
    lxi  b, LOGO_DEST
    call unmlz

    ; Разворачиваем логотип из 320x166 в 320x256
    lxi  d, LOGO_DEST + LOGO_SIZE
    lxi  h, LOGO_DEST + (LOGO_WIDTH / 8 * VIDEO_HEIGHT)
    mvi  b, 40
intro_1:
    mvi  c, LOGO_HEIGHT
intro_2:
    dcx  d
    ldax d
    dcx  h
    mov  m, a
    dcr  c
    jnz  intro_2
    mvi  c, VIDEO_HEIGHT - LOGO_HEIGHT
    xra  a
intro_3:
    dcx  h
    mov  m, a
    dcr  c
    jnz  intro_3
    dcr  b
    jnz  intro_1

    ; Показываем лого
    mvi  a, PALETTE_DARK_RED
    mvi  b, PALETTE_RED
    call paletteEffect

    ; Ждем нажатия на любую клавиашу
    call pressAnyKey

    ; Гасим экран
    ; jmp paletteOff

;----------------------------------------------------------------------------------------------------------------------

paletteOff:
    mvi  a, PALETTE_DARK_RED
    mvi  b, PALETTE_BLACK
paletteEffect:
    out  IO_PALETTE_2
    out  IO_PALETTE_3
    call delay8000
    mov  a, b
    out  IO_PALETTE_2
    out  IO_PALETTE_3
    ret

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

;----------------------------------------------------------------------------------------------------------------------

introText:
    db  4,19,"SPECTRUM HOLOBYTE PRESENTS"
    db  6,26,"T E T R I S"
    db  8,15,"VERSION"
    db  8,33,"PROGRAMMER"
    db 10,15,"ISKRA 1080 TARTU"
    db 10,33,"ALEKSEI MOROZOV"
    db 12,15,"IBM CGA"
    db 12,33,"ENG AN JIO"
    db 13,15,"RAM RESIDENT"
    db 13,33,"ERICK JAP"
    db 14,15,"TANDY"
    db 14,33,"BILLY SUTYONO"
    db 15,15,"IBM EGA"
    db 15,33,"ARYANTO WIDODO"
    db 17,15,"GRAPHICS"
    db 17,33,"DAN GUERRA"
    db 18,15,"PRODUCT MANAGER"
    db 18,33,"R. ANTON WIDJAJA"
    db 19,15,"PRODUCER"
    db 19,33,"SEAN B. BARGER"
    db 0
