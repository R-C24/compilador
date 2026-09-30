#ifndef BEAMER_H
#define BEAMER_H

#define MaxErrores 1000
#define ErroresPorPagina 8

typedef struct TokenStats TokenStats;
struct TokenStats {
    int palabrasReservadas;
    int modificadores;
    int identificadores;
    int num;
    int operadores;
    int separadores;
    int erroresLexicos;
};

// Estructura para registrar errores léxicos
typedef struct ErrorLexico ErrorLexico;
struct ErrorLexico {
    int linea;
    char lexema[64];
    char descripcion[128];
};

void generarBeamer(const char *nombreArchivo, TokenStats *stats, ErrorLexico *errores, int cantErrores);

int compilarTexPdf(char *archivoTex);
int desplegarPdf(char *archivoPdf);

#endif //BEAMER_H