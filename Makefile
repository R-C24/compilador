# Nombre del ejecutable final
TARGET = compiladorC

# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lfl

# Archivos fuente
SRCS = main.c lex.yy.c parser.tab.c

all: $(TARGET)

# Regla para generar el scanner con Bison
parser.tab.c parser.tab.h: parser.y
	bison -d parser.y

# Regla para generar el scanner con Flex
lex.yy.c: scanner.l parser.tab.h
	flex scanner.l

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Limpieza del proyecto
clean:
	rm -f $(TARGET) lex.yy.c parser.tab.c parser.tab.h *.o *.aux *.log *.nav *.snm *.toc *.out *.pdf *.tex *.tmp *.vrb

.PHONY: all clean