CC=gcc
ASM=nasm
LDFLAGS=-lfl
CFLAGS=-Wall
EXEC=tpcc

EXECPATH=bin/
BINPATH=obj/
SRCPATH=src/
CURRENT_DIR=$(notdir $(shell pwd))

vpath %.c $(SRCPATH)
vpath %.h $(SRCPATH)
vpath %.y $(SRCPATH)
vpath %.lex $(SRCPATH)
vpath %.o $(BINPATH)  

$(EXEC): parser.tab.o lex.yy.o abstract-tree.o symbolTable.o
	$(CC) -o $(EXECPATH)$@ $(addprefix $(BINPATH), $(notdir $^)) $(LDFLAGS)

parser.tab.c: parser.y
	bison --defines=$(SRCPATH)parser.tab.h -o $(SRCPATH)parser.tab.c $<  

lex.yy.c: lexer.lex
	flex -o $(SRCPATH)lex.yy.c $< 

parser.tab.o: parser.tab.c abstract-tree.h

abstract-tree.o: abstract-tree.c abstract-tree.h

symbolTable.o: symbolTable.h symbolTable.c

%.o: %.c
	$(CC) -o $(BINPATH)$@ -c $(SRCPATH)$(notdir $<) $(CFLAGS) 

clean:
	rm -f $(BINPATH)*.o
	rm -f $(SRCPATH)lex.yy.*
	rm -f $(SRCPATH)parser.tab.*

mrproper: clean
	rm -f $(EXECPATH)$(EXEC)

tar:
	cd ..; tar -czf $(CURRENT_DIR).tar.gz $(CURRENT_DIR)