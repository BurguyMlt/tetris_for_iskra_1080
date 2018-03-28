LOGO_WIDTH  = 320
LOGO_HEIGHT = 166
LOGO_SIZE   = (LOGO_WIDTH / 8 * LOGO_HEIGHT)
LOGO_DEST   = (VIDEO_PAGE_0 + ((VIDEO_WIDTH - LOGO_WIDTH) / 2 / 8 * 256) - ((VIDEO_HEIGHT - LOGO_HEIGHT) / 2))

intro:
    mvi  a, 3
    call setTextColor

    ; Не инверсный текст
    call normalText_F793

    ; Гасим экран
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

introText:
    db  4-2,19-8,"SPECTRUM HOLOBYTE PRESENTS"
    db  6-2,26-8,"T E T R I S"
    db  8-2,15-8,"VERSION"
    db  8-2,33-8,"PROGRAMMER"
    db 10-2,15-8,"ISKRA 1080 TARTU"
    db 10-2,33-8,"ALEKSEI MOROZOV"
    db 12-2,15-8,"IBM CGA"
    db 12-2,33-8,"ENG AN JIO"
    db 13-2,15-8,"RAM RESIDENT"
    db 13-2,33-8,"ERICK JAP"
    db 14-2,15-8,"TANDY"
    db 14-2,33-8,"BILLY SUTYONO"
    db 15-2,15-8,"IBM EGA"
    db 15-2,33-8,"ARYANTO WIDODO"
    db 17-2,15-8,"GRAPHICS"
    db 17-2,33-8,"DAN GUERRA"
    db 18-2,15-8,"PRODUCT MANAGER"
    db 18-2,33-8,"R. ANTON WIDJAJA"
    db 19-2,15-8,"PRODUCER"
    db 19-2,33-8,"SEAN B. BARGER"
    db 0
