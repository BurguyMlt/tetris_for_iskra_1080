.i8080

setCursorY_F7DC   = 0F7DCh
setCursorX_F7BE   = 0F7BEh
printString_F137  = 0F137h
waitKey_FDAC      = 0FDACh
drawChar1_F7FB    = 0F7FBh
drawChar2_F8B0    = 0F8B0h
clearScreen1_F9A0 = 0F9A0h
inkey_FC12        = 0FC12h
negativeText_F798 = 0F798h
normalText_F793   = 0F793h
videoMode_C802    = 0C802h

org 234
fileStart:
.db 4ch, 56h, 4fh, 56h, 2fh, 32h, 2eh, 30h, 2fh, 0d0h, 6ch, 61h, 62h, 20h, 20h, 20h
.dw entry
.dw end1
.dw entry

entry:
    jmp main

; 01 01 00 01
; 10 00 01 00

figures: db 81,132,16,101,81,132,16,101,64,149,84,33,64,149,84,33,81,217,84,118,81,217,84,118,16,84,16,84,16,84,16,84,65,101,64,88,16,82,81,73,64,152,16,66,16,149,84,38,81,137,64,101,16,132,16,98 

// const
CHECK = 255
WIDTH = 10
HEIGHT = 20
BPL = WIDTH+1
START = 1

// images
map db 1,0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   0,0,0,0,0,0,0,0,0,0,1
    db   1,1,1,1,1,1,1,1,1,1,1    

map2 db 10*20 dup(0FFh)

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

;----------------------------------------------------------------------------
; Очистка экрана
        
clearScreen:
    call clearScreen1

;-V-V-V----------------------------------------------------------------------
; Очистка второй плоскости экрана (9000-BFFF)
    
clearScreen2:
    lxi  h, 09000h
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
    lxi  h, 0D000h
    xra  a
    jmp  clearMem

;-------------------------------------------------------------------------------
; Перерисовать весь экран

redrawAll:
    ; Перерисовать фон
    call drawBackgrond
    
    ; Перерисовать экран
    lxi  h, map2
    mvi  a, 0FFh
    mvi  c, WIDTH * HEIGHT
redrawAll_1:
    mov  m, a
    inx  h
    dcr  c
    jnz  redrawAll_1
        
    call redrawNewFigure
    call redrawMap
    call redrawScore
    jmp  redrawGameover

;-------------------------------------------------------------------------------
; Перерисовать фон

drawBackgrond:
    ; Сохраняем системные переменные
    lxi d, 0C87Fh
    lxi h, 08FFFh
    lxi b, 080h
    call copy
    
    ; Гасим экран
    call blackPalette
        
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
    
    push h
    
    ; Распаковываем
    lxi  b, 09000h
    call unmlz

    ; Переносим вторую плоскость
    lxi h, 0FFFFh
    lxi d, 0EFFFh
    lxi b, 03000h
    call copy
    pop  h
    
    ; Восстанавливаем палитру
    inx  h
    mov  a, m
    out  90h
    inx  h
    mov  a, m
    out  91h
    inx  h
    mov  a, m
    out  92h
    inx  h
    mov  a, m
    out  93h

    ; Восстанавливаем системные переменные
    lxi d, 08FFFh
    lxi h, 0C87Fh
    lxi b, 080h
    call copy    

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
    
;-------------------------------------------------------------------------------
; Рисование клетки

cellOff: db 240- 0,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240- 2,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 240- 3,003h,0FFh,0F0h ; 00000011,11111111,11110000         
         db 240- 5,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 240- 7,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240- 9,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 240-10,003h,0FFh,0F0h ; 00000011,11111111,11110000         
         db 240-12,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 240-14,03Fh,0FFh,000h ; 00111111,11111111,00000000
         db 240-16,0FFh,0FCh,000h ; 11111111,11111100,00000000
         
         db 254- 0,0FFh,0FCh,000h ; 11111111,11111100,00000000
         db 254- 1,003h,0FFh,0F0h ; 00000011,11111111,11110000         
         db 254- 3,00Fh,0FFh,0C0h ; 00001111,11111111,11000000
         db 254- 5,03Fh,0FFh,000h ; 00111111,11111111,00000000
         
drawCell:
    ; addr = cellOff[d]
    mov b, e
    lxi h, cellOff
    mov e, d
    mvi d, 0
    dad d
    dad d
    dad d
    dad d
    mov d, m
    mov e, b
    inx  h

    mov  c, a      
         
    call clearCell3
         
    mov  a, c
    rrc
    mov  c, a
    
    mov  a, d
    sui  40h
    mov  d, a

    call clearCell3
    
    ret

;-------------------------------------------------------------------------------

clearCell3:
    push h
    push d
    
    mov  b, m    
    call clearCell

    inx  h
    mov  b, m    
    call clearCell

    inx  h
    mov  b, m    
    call clearCell
    
    pop d
    pop h
    ret

;-------------------------------------------------------------------------------

clearCell:
    push d

    mov  a, c
    rrc
    jc   setCell

    mov  a, b
    cma
    mov  b, a
        
    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    dcx  d

    ldax d
    ana  b    
    stax d
    
    pop  d
    dcr  d
    
    ret

;-------------------------------------------------------------------------------

setCell:
    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    dcx  d

    ldax d
    ora  b    
    stax d
    
    pop  d
    dcr  d

    ret

;-------------------------------------------------------------------------------

POS_Y = 8

redrawMap:
    ;var a = START;
    lxi b, map + START
    lxi h, map2
    
    ;for(var y=0; y<HEIGHT; y++, a++) {
    mvi e, 0FFh - POS_Y
redrawMap_1:
        
    ; for(var x=0; x<WIDTH; x++, a++) {
    mvi d, 0
redrawMap_2:
    
    ; var c = map[a]
    ; ctx.fillStyle = colors[c];
    ; ctx.fillRect(122+x*14, 8+y*12, 14, 12);
    ldax b
    cmp  m
    jz   redrawMap_3
    mov  m, a    
    
    push b
    push d    
    push h
    call drawCell
    pop  h
    pop  d    
    pop  b
    
redrawMap_3:
    inr  d
    inx  b
    inx  h    
    mov  a, d
    cpi  WIDTH
    jnz  redrawMap_2
    
    inx  b
    mov  a, e
    sui  12
    mov  e, a       
    cpi  0FFh - POS_Y - (HEIGHT * 12)
    jnz  redrawMap_1
    ret

;-------------------------------------------------------------------------------

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
    mvi  m, 1
    inx  h
    mvi  c, HEIGHT
newGame_1:
    call clearLine
    mvi  m, 1
    inx  h
    dcr  c
    jnz  newGame_1

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

initPalette:
    mvi  a, 15 ; black
    out  90h
    mvi  a, 3 ; red
    out  91h
    mvi  a, 4 ; cyan
    out  92h
    mvi  a, 0 ; white
    out  93h
    ret
    
;----------------------------------------------------------------------------------------------------------------------

blackPalette:
    mvi  a, 15 ; black
    out  90h
    out  91h
    out  92h
    out  93h
    ret
    
;----------------------------------------------------------------------------------------------------------------------

timer db 0 
keybTimer db 0

;----------------------------------------------------------------------------------------------------------------------

key:
    ; Ждем пока пользователь отпустит клавишу
main_3:
    call inkey_FC12
    cpi  0FFh
    jnz  main_3
main_4:
    call rand
    call inkey_FC12
    cpi  0FFh
    jz   main_4
    ret

;----------------------------------------------------------------------------------------------------------------------

main:
    lxi sp, 100h

    ; Установка палитры для каждого из 4-х цветов
    call initPalette

    ; Режим экрана 64 символа в ширину
    mvi  a, 40h
    sta  videoMode_C802
    
    ; Цвет фона 0, цвет текста 0
    call normalText_F793
        
    ; Очистка экрана
    call clearScreen
             
    ; Текст в начале
    lxi  h, aStart
main_0:
    mov  a, m
    inx  h
    ora  a
    jz   main_1
    cpi  32
    jnc  main_2    
    push h
    call setCursorY_F7DC    
    pop  h
    mov  a, m
    inx  h
    push h
    call setCursorX_F7BE
    pop  h
    jmp  main_0
main_2:    
    call drawChar1_F7FB
    jmp  main_0    
main_1:

    call key    
    
    call blackPalette

    call clearScreen1
    
    lxi  d, logo
    lxi  b, 09000h
    push b
    call unmlz
    pop  d
    
    lxi  h, 0D42Dh
    mvi  c, 40
main_5:
    push b
    
    mvi  c, 166
main_4:
    ldax d
    inx  d
    mov  m, a
    inx  h
    dcr  c
    jnz  main_4
    
    lxi  b, 256-166
    dad  b
    
    pop  b
    dcr  c
    jnz  main_5
    
    call clearScreen2
    
    call initPalette
    
    call key    
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

;----------------------------------------------------------------------------------------------------------------------

rand:
rand_seed = $+1
    mvi a, 0
    mov e,a
    add a
    add a
    add e
    inr a
    sta rand_seed
    ret

;----------------------------------------------------------------------------------------------------------------------
; Деление

_DIV0:
_DIV:	MOV A,H
	ORA L
	RZ
	LXI B,0000
	PUSH B
_DIV1:	MOV A,E
	SUB L
	MOV A,D
	SBB H
	JC _DIV2
	PUSH H
	DAD H
	JNC _DIV1
_DIV2:	LXI H,0000
_DIV3:	POP B
	MOV A,B
	ORA C
	RZ
	DAD H
	PUSH D
	MOV A,E
	SUB C
	MOV E,A
	MOV A,D
	SBB B
	MOV D,A
	JC _DIV4
	INX H
	POP B
	JMP _DIV3
_DIV4:	POP D
	JMP _DIV3

;----------------------------------------------------------------------------------------------------------------------

aStart: db  4,19,"SPECTRUM HOLOBYTE PRESENTS"
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
        
;----------------------------------------------------------------------------------------------------------------------

.include "unmlz.inc"
logo:
.include "graph/logo.inc"
LEVELS_COUNT = 3

; 1-желтый,5-зеленый,6-синий
levels dw level1
       db 15,1,4,0,0,0 ; черный, желтый, голубой, белый
       dw level2
       db 15,2,4,0,0,0 ; черный, красный, голубой, белый
       dw level3
       db 15,3,5,0,0,0 ; черный, красный, голубой, белый
       
level1:
.include "graph/level1.inc"
level2:
.include "graph/level2.inc"
level3:
.include "graph/level3.inc"
end1:
make_binary_file "tetris.lvt", fileStart, end1
.end
