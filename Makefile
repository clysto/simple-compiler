CC = gcc
OUT = my_compiler
OBJ = lex.yy.o y.tab.o node.o main.o
SCANNER = scanner.l
PARSER = parser.y

build: $(OUT)
	rm -f *.o lex.yy.c y.tab.c y.tab.h y.output

clean:
	rm -f *.o lex.yy.c y.tab.c y.tab.h y.output y.dot $(OUT)

graph: $(PARSER)
	bison -vdty -Wno-yacc --graph $< && rm -f *.o y.tab.c y.tab.h y.output

$(OUT): $(OBJ)
	$(CC) -o $(OUT) $(OBJ)

lex.yy.c: $(SCANNER) y.tab.c
	flex $<

y.tab.c: $(PARSER)
	bison -vdty -Wno-yacc $<
