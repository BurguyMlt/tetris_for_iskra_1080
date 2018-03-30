LOGO_WIDTH  = 320
LOGO_HEIGHT = 166
LOGO_SIZE   = (LOGO_WIDTH / 8 * LOGO_HEIGHT)
LOGO_DEST   = (VIDEO_PAGE_0 + ((VIDEO_WIDTH - LOGO_WIDTH) / 2 / 8 * 256) - 16)

intro:
    mvi  a, 3
    call setTextColor

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

    ; Вывод текста
    call setTextColorNt
    lxi  h, introText2
intro_4:
    call inkey_FC12
    cpi  0FFh
    jnz  paletteOff
    lxi  d, 2000h
    call delay
    mov  a, m
    cpi  '!'
    jz   intro_5
    call drawText2
    jmp  intro_4

;----------------------------------------------------------------------------------------------------------------------
; Прокрутка

intro_5:
    push h
    mvi  b, 14
intro_6:
    push b
    lxi  d, 0FFFFh - (15*14) - (6*256) - 3
    push d
    pop  h
    mvi  b, 36
scroll_0:
    push d
    push h
    mvi  c, 26
scroll_1:
    dcx  h
    mov  a, m
    stax d
    dcx  d
    dcr  c
    jnz  scroll_1
    pop  h
    pop  d
    dcr  h
    dcr  d
    dcr  b
    jnz  scroll_0
    pop  b
    dcr  b
    jnz  intro_6

    lxi  h, 0FFFFh - (17*14) - (6*256)
    shld drawCharAddr
    pop  h
    inx  h
    jmp  intro_4

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

introText2:
    db 33,"TETRIS WAS INVENTED BY A 30-YEAR-OLD"
    db 33,"SOVIET RESEARCHER NAMED ALEXEY"
    db 33,"PAZHITNOV WHO CURRENTLY WORKS AT"
    db 33,"THE COMPUTER CENTRE(ACADEMY SOFT) OF"
    db 33,"THE USSR ACADEMY OF SCIENCES IN"
    db 33,"MOSCOW. THE ORIGINAL PROGRAMMER WAS"
    db 33,"18-YO VADIM GERASIMOV. A STUDENT"
    db 33,"STUDYING COMPUTER INFORMATICS AT"
    db 33,"MOSCOW UNIVERSITY. NOW YOU CAN ENJOY"
    db 33,"TETRIS BECAUSE OF THE JOINT EFFORTS"
    db 33,"OF ACADEMY SOFT, MOSCOW, ANDROMEDA"
    db 33,"SOFTWARE LTD, LONDON, ALEMORF, SPB"
    db 33,"AND SPECTRUM HOLOBYTE, USA."
    db 0

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
