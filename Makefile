# Nombre del ejecutable final
TARGET = compiladorC

# Compilador y banderas
CC = gcc
CFLAGS = -Wall -Wextra
LIBS = -lfl

# Archivos fuente
SRCS = main.c beamer.c lex.yy.c

all: $(TARGET)

# Regla para generar el scanner con Flex
lex.yy.c: scanner.l
	flex scanner.l

# Regla para compilar el ejecutable
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# Limpieza del proyecto
clean:
	rm -f $(TARGET) lex.yy.c *.o *.aux *.log *.nav *.snm *.toc *.out *.pdf *.tex *.tmp

.PHONY: all clean