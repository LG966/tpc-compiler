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

.PHONY : all clean mrproper tar

all: addFolders $(EXECPATH)$(EXEC) #clean

$(EXECPATH)$(EXEC): parser.tab.o lex.yy.o abstract-tree.o symbolTable.o gen_code_asm.o operator.o type.o struct.o func.o
	$(CC) -o $@ $(addprefix $(BINPATH), $(notdir $^)) $(LDFLAGS)

parser.tab.c: parser.y
	bison --defines=$(SRCPATH)parser.tab.h -o $(SRCPATH)parser.tab.c $<

lex.yy.c: lexer.lex
	flex -o $(SRCPATH)lex.yy.c $<

parser.tab.o: parser.tab.c abstract-tree.o operator.o type.o

abstract-tree.o: abstract-tree.c abstract-tree.h operator.o type.o

symbolTable.o: symbolTable.c symbolTable.h type.o struct.o

gen_code_asm.o: gen_code_asm.c gen_code_asm.h operator.h symbolTable.h

operator.o: operator.c operator.h

type.o : type.c type.h

struct.o : struct.c struct.h abstract-tree.o type.o

func.o : func.c func.h type.o struct.o abstract-tree.o 

%.o: %.c
	$(CC) -o $(BINPATH)$@ -c $(SRCPATH)$(notdir $<) $(CFLAGS)

addFolders: | $(EXECPATH) $(BINPATH)

$(EXECPATH):
	mkdir $(EXECPATH)

$(BINPATH):
	mkdir $(BINPATH)

clean:
	rm -f $(BINPATH)*.o
	rm -f $(SRCPATH)lex.yy.*
	rm -f $(SRCPATH)parser.tab.*

mrproper: clean
	rm -f $(EXECPATH)$(EXEC)
	rm -f comp.asm
	rm -f ./test/resultat.log

tar:
	cd ..; tar -czf $(CURRENT_DIR).tar.gz $(CURRENT_DIR)
