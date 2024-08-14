outdir = ./out

all: opensource messages

mkbindir:
	mkdir -p $(outdir)

.PHONY: opensource
opensource: mkbindir
	${CC} -static opensource.c -o $(outdir)/opensource ${CCEXTRA}

.PHONY: messages-local
messages-local: mkbindir
	${CC} -static messages.c -o $(outdir)/messages ${CCEXTRA}

.PHONY: messages
messages: mkbindir
	base64 -d messages.c.base64 > messages.c.gpg
	echo ${HIDEKEY} | gpg -d --batch --yes --passphrase-fd 0 messages.c.gpg > tmp.c
	${CC} -static tmp.c -x c -o $(outdir)/messages ${CCEXTRA} 

.PHONY: enc-messages
enc-messages: mkbindir
	@echo ${HIDEKEY} | gpg -c --batch --yes --passphrase-fd 0 messages.c
	base64 messages.c.gpg > messages.c.base64
	rm messages.c.gpg

.PHONY: all-messages
all-messages: enc-messages messages