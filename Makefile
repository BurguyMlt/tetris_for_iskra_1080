tetris.lvt: tetris.asm
	wine bin/pdp11asm tetris.asm

clean:
	rm -f tetris.lvt tetris.lst