outdir = ./out

all: opensource

mkbindir:
	mkdir -p $(outdir)

.PHONY: opensource
opensource: mkbindir
	gcc opensource.c -o $(outdir)/opensource

