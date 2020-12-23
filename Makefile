CC = gcc
CFLAGS = -std=c99 -fPIC -pedantic -Wall -g -I.
OUT = my_compiler
OBJECTS = lex.yy.o parser.tab.o node.o main.o
SCANNER = scanner.l
PARSER = parser.y

build: $(OUT)
	rm -f *.o *.yy.c *.tab.c *.tab.h parser.output y.dot

clean:
	rm -f *.o *.yy.c *.tab.c *.tab.h parser.output y.dot $(OUT)

$(OUT): $(OBJECTS)
	$(CC) -o $(OUT) $(CFLAGS) $(OBJECTS)

lex.yy.c: $(SCANNER) parser.tab.c
	flex $<

parser.tab.c: $(PARSER)
	bison -vdt $<
