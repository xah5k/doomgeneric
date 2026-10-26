all:
	make -C doomgeneric -f Makefile.ah5kos

cpsysroot:
	cp doomgeneric/doomgeneric ../../sysroot/programs/doomgeneric

clean:
	make -C doomgeneric -f Makefile.ah5kos clean