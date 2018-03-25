all: tetris.lvt

tetris.lvt: tetris.asm bios.inc div16.inc graph.inc intro.inc playfieldgraph.inc rand.inc tetris.inc unmlz.inc
	wine bin/pdp11asm tetris.asm

clean:
	rm -f tetris.lvt tetris.lst
