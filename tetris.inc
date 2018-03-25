map  db 232 dup(0)
map2 db 200 dup(0) ;(0D000h - MAP2_SIZE)


figures db 81,132,16,101,81,132,16,101,64,149,84,33,64,149,84,33,81,217,84,118,81,217,84,118,16,84,16,84,16,84,16,84,65,101,64,88,16,82,81,73,64,152,16,66,16,149,84,38,81,137,64,101,16,132,16,98

// const
CHECK = 255
WIDTH = 10
HEIGHT = 20
BPL = WIDTH+1
START = 1
MAP_SIZE = (START + (BPL * (HEIGHT + 1)))
MAP2_SIZE = (WIDTH * HEIGHT)

timer db 0
keybTimer db 0
figureO db 0
figureA db 0
figureC db 0
score   dw 0
nextC   db 0
nextO   db 0
level   db 0
levelBg db 2,2,3,2,0,0,2,2,2,4
sCopy=0
sLogo=1
sMenu=2
sGame=3
sGameOver db 4
state   db sLogo;
delays  dw 1000,800,700,600,500,400,300,200,150
timerId db 0

;----------------------------------------------------------------------------------------------------------------------

delay8000:
    lxi  d, 08000h
delay:
    dcx  d
    mov  a, d
    ora  e
    jnz  delay
    ret

;-------------------------------------------------------------------------------

copy:
        ldax d
        mov m, a
        dcx d
        dcx h
        dcx b
        mov a, c
        ora b
        jnz copy
        ret

;-------------------------------------------------------------------------------
; Перерисовать весь экран

redrawAll:
    ; Перерисовать фон
    call drawBackgrond

    ; Перерисовать экран
    lxi  h, map2
    mvi  a, 0FFh
    mvi  c, MAP2_SIZE
redrawAll_1:
    mov  m, a
    inx  h
    dcr  c
    jnz  redrawAll_1

    call redrawNewFigure
    call redrawMap
    call redrawScore

    call paletteShow
    ret

;-------------------------------------------------------------------------------
; Прерисовать новую фигуру

redrawNewFigure:
    ; Залить цветом 1 место где выводится новая фигура
    xra  a
    lxi  d, 0B8F7h
    call clearNewFigure
    cma
    lxi  d, 0F8F7h
    call clearNewFigure

    ; Нарисовать новую фигуру
    ; decodeFigure(nextO, redrawNewFigure1);
    lda  nextO
    lxi  h, redrawNewFigure1
    call decodeFigureEx
    ret

;-------------------------------------------------------------------------------
; Залить место экрана в одной плоскости где выводится новая фигура

clearNewFigure:
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
    call _DIV
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


redrawGameover:
    ; if(state == sGame) return;
    lda  state
    cpi  sGame
    rz

    ;ctx.textBaseline = "middle";
    ;ctx.textAlign = "center";
    ;ctx.fillStyle = colors[2];
    ;ctx.fillText("Game Over",384/2,256/2);
    ret

;-------------------------------------------------------------------------------

checkFigure:
    ; return map[a];
    mov a, m
    ora a
    ret

;-------------------------------------------------------------------------------

; map[a] = figureC;
; return false;

drawFigure:
    lda  figureC
    mov  m, a
    xra  a ; return z
    ret

;-------------------------------------------------------------------------------

clearFigure:
    mvi m, 0
    xra a ; return z
    ret

;-------------------------------------------------------------------------------

; return fn(x, y, figureA + START + x + y * BPL);

; a & 3, (a >> 2) & 3
; a = y
; d = x

decodeFigure2:
    mov  l, a
    ani  3
    mov  d, a ; d - это X
    mov  a, l
    rrc
    rrc
    ani  3
    mov  e, a ; e - это Y
    add  a ; *2
    add  a ; *4
    add  e ; *5
    add  a ; *10
    add  e ; *11
    add  d
    mov  b, a
    lda  figureA
    add  b
    mvi  h, 0
    mov  l, a
    lxi  b, map + START
    dad  b

    ; call
decodeFigure3 = $+1
    jmp 0

;-------------------------------------------------------------------------------
; return decodeFigure2(fn, a) || decodeFigure2(a >> 4);

decodeFigure1:
    mov  l, a
    push h
    call decodeFigure2
    pop  h
    rnz
    mov  a, l
    rrc
    rrc
    rrc
    rrc
    jmp  decodeFigure2

;-------------------------------------------------------------------------------
; return true;

decodeFigure:
    lda  figureO

decodeFigureEx:
    shld decodeFigure3

    ; if(decodeFigure1(fn, figures[figureO + 0])) return true;
    mov  l, a
    mvi  h, 0
    lxi  d, figures
    dad  d
    mov  a, m
    push h
    call decodeFigure1
    pop  h
    rnz

    ; return decodeFigure1(fn, figures[figureO + 1]);
    inx  h
    mov  a, m
    jmp  decodeFigure1

;-------------------------------------------------------------------------------

gen:
    ;nextO = Math.floor(Math.random() * 7) * 2 * 4;
    call rand
gen_2:
    sbi  7
    jnc  gen_2
    adi  7
    ; * 2 * 4
    add  a
    add  a
    add  a
    sta  nextO

    ;nextC = Math.floor(Math.random() * (colors.length - 1)) + 1;
    call rand
gen_3:
    sbi  3
    jnc  gen_3
    adi  3
    inr  a
    sta  nextC
    ret

;-------------------------------------------------------------------------------

newFigureEx:
    ; figureO = nextO;
    lda  nextO
    sta  figureO

    ; figureC = nextC;
    lda  nextC
    sta  figureC

    ; figureA = START + ((WIDTH - 1) >> 1);
    mvi  a, 4
    sta  figureA

    ; if(decodeFigure(checkFigure)) state = sGameOver;
    lxi  h, checkFigure
    call decodeFigure
    jz   newFigure_1
    mvi  a, sGameOver
    sta  state
newFigure_1:

    ; decodeFigure(drawFigure);
    lxi  h, drawFigure
    call decodeFigure

    ; gen();
    jmp gen

;-------------------------------------------------------------------------------

newFigure:
    call newFigureEx

    ; redrawNewFigure();
    call redrawNewFigure

    ; redrawMap();
    call redrawMap

    ; redrawGameover();
    jmp redrawGameover

;-------------------------------------------------------------------------------

; b = d
; c = r

move:
    ;if(state != sGame) return;
    lda  state
    cpi  sGame
    rnz

    ; decodeFigure(clearFigure);
    push b
    lxi  h, clearFigure
    call decodeFigure
    pop  b

    ; figureA += d;
    lda figureA
    push psw
    add c
    sta figureA

    ; var o1 = figureO;
    lda  figureO
    push psw

    ; if(r) figureO = ((figureO + 2) & 6) + (figureO & ~6);
    dcr b
    jnz move_1

    mov b, a
    inr a
    inr a
    ani 6
    mov c, a
    mov a, b
    ani 0F9h
    add c
    sta figureO

move_1:

    ; if(!decodeFigure(checkFigure)) {
    lxi  h, checkFigure
    call decodeFigure
    jz   move_2

    ; figureA -= d;
    ; figureO = o1;
    pop psw
    sta figureO
    pop psw
    sta figureA

    ; decodeFigure(drawFigure);
    lxi  h, drawFigure
    call decodeFigure

    ; return false;
    xra a
    ret

move_2:
    ; free tmp
    pop psw
    pop psw

    ; decodeFigure(drawFigure);
    lxi  h, drawFigure
    call decodeFigure

    ; redrawMap();
    call redrawMap

    ; return true;
    xra a
    inr a
    ret

;-------------------------------------------------------------------------------

; hl - address

deleteLine:
    ; a--;
    dcx  h

    ; prepare
    mov  d, h
    mov  e, l
    lxi  b, BPL
    dad  b

    ; for(;a != 0; a--) {
deleteLine_1:
    ldax d
    dcx  d
    mov  m, a
    dcx  h

    ; if(a==0) break;
    mov  a, e
    sui  map
    mov  a, d
    sbi  (map/256)
    jnc  deleteLine_1

    ;clearLine(START);
    lxi  h, map + START
    call clearLine

    ; redrawMap();

    jmp  redrawMap

;-------------------------------------------------------------------------------
; hl - address

clearLine:
    ; for(var x=0; x<WIDTH; x++, a++)
    ;   map[a] = 0;
    ; return a;

    mvi  b, WIDTH
clearLine_1:
    mvi  m, 0
    inx  h
    dcr  b
    jnz  clearLine_1
    ret

;-------------------------------------------------------------------------------

newGame:
    call gen
    xra  a
    sta  level
    sta  score + 0
    sta  score + 1
    mvi  a, sGame
    sta  state

    ; for(a = 1, y = HEIGHT; y != 0; y--)
    ;    a = clearLine(a) + 1;

    lxi  h, map
    mvi  c, HEIGHT
newGame_1:
    mvi  m, 1
    inx  h
    call clearLine
    dcr  c
    jnz  newGame_1
    mvi  c, BPL + 1
newGame_1a:
    mvi  m, 1
    inx  h
    dcr  c
    jnz  newGame_1a


    call  newFigureEx

    call  redrawAll

    ret

;-------------------------------------------------------------------------------

down1:
    ; if(move(BPL, 0)) return true;
    lxi  b, BPL
    call move
    rnz

    ; checkLines();
    call checkLines

    ; newFigure();
    call newFigure

    ; return false;
    xra a
    ret

;-------------------------------------------------------------------------------

checkLine:
    ; for(var x = WIDTH; x != 0; x--, a++)
    ;     if(map[a] == 0)
    ;         return false;
    mvi  c, WIDTH
checkLine_1:
    mov  a, m
    ora  a
    rz
    inx  h
    dcr  c
    jnz  checkLine_1

    ; return true;
    xra a
    inr a
    ret

;-------------------------------------------------------------------------------

setLevel:
    ; level = l;
    sta level

    ; initTimer();
    ; call initTimer

    ; redrawAll();
    jmp redrawAll

;-------------------------------------------------------------------------------

checkLines:
    mvi  a, 0C9h
    sta  checkLines_3
    ; var f = false;
    ; var a = START + BPL * (HEIGHT - 1);
    lxi h, map + START + (BPL * (HEIGHT - 1))
checkLines_1:
    ; for(;a != START;)
    ; {
    ;     if(checkLine(a)) {
    push h
    call checkLine
    pop  h
    jz checkLines_2
    ;         deleteLine(a);
    push h
    call deleteLine
    ;         score++;
    lhld score
    inx  h
    shld score
    ; f = true;
    xra  a
    sta  checkLines_3
    pop  h
    ;         continue;
    jmp checkLines_1
checkLines_2:
    ;     }
    ;     a -= BPL;
    lxi d, 65536-BPL
    dad d
    ; cond
    mov a, l
    sui (map + START + 1)
    mov a, h
    sbi (map + START + 1) / 256
    jnc checkLines_1
    ; if(f) {
checkLines_3:
    ret
    ;     redrawScore();
    call redrawScore
    ; var l = Math.floor(score/10);
    ; if(level < 8 && level < l) setLevel(l);
    lhld score
    xchg
    lxi  h, 10
    call _div
    xra a
    ora h
    rnz
    mov a, l
    cpi LEVELS_COUNT
    rnc
    lda level
    cmp l
    rz
    mov a, l
    sta level
    jmp redrawAll

down:
    ; while(move(BPL, 0));
    lxi  b, BPL
    call move
    jnz  down

    ; checkLines();
    call checkLines

    ; newFigure();
    call newFigure

    ; initTimer();
    ;!!!!!!!!!!!!!!!!!!!!!!
    jmp gameLoop

tick:
    ;if(state != sGame) return;
    lda  state
    cpi  sGame
    rnz

    ;down1();
    call down1

    ;redrawMap();
    call redrawMap

    ret

;----------------------------------------------------------------------------------------------------------------------

tetris:
    xra  a
    sta  palette
    call newGame

gameLoop:
    lda timer
    inr a
    sta timer ; by level
    jnz gameLoop_2
    call tick
gameLoop_2:

    ; Привязываем генератор случайных чисел к нажатиях клавиш пользователем.
    call rand

    ; Получить код нажатой клавиши
    call inkey_FC12
    mov  b, a

    ; Если таймер установлен
    lda  keybTimer
    ora  a
    jz   gameLoop_1
    ; Если таймер больше 1, уменьшаем и пропускаем
    dcr  a
    jnz  gameLoop_164
    ;
    mov  a, b
    cpi  0FFh
    jnz  gameLoop_17
    xra  a
gameLoop_164:
    sta  keybTimer
gameLoop_17:
    mvi  b, 0FFh
gameLoop_1:
    mov  a, b

    cpi  0FFh
    jz   gameLoop_4
    mvi  a, 2
    sta  keybTimer
    mov  a, b
gameLoop_4:

    ; Клавиша "Вправо"
    cpi  13
    jnz  gameLoop_185
    call newGame
    jmp  gameLoop

gameLoop_185:
    ; Клавиша "Вверх"
    cpi  136
    jnz  gameLoop_16
    lxi  b, 100h
    call move
    jmp  gameLoop
gameLoop_16:

    ; Клавиша "Вниз"
    cpi  130
    jz   down

    ; Клавиша "Влево"
    cpi  132
    jnz  gameLoop_18
    lxi  b, 0FFh
    call move
gameLoop_18:
    ; Клавиша "Вправо"
    cpi  134
    jnz  gameLoop_19
    lxi  b, 01h
    call move
    jmp  gameLoop
gameLoop_19:
    lxi  b, 0
    call move
    jmp  gameLoop
