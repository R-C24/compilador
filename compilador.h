#ifndef COMPILADOR_H
#define COMPILADOR_H

#define MaxMacros 100
#define MaxNombre 64
#define MaxValor 256
#define TamBuffer 1024

typedef struct Macro Macro;
struct Macro{
    char nombre[MaxNombre];
    char valor[MaxValor];
};

extern Macro macros[MaxMacros];
extern int totalMacros;

void agregarMacro(char *nombre, char *valor);
void traducirMacros(char *linea);
void procesarArchivo(char *nombreArchivo, FILE *f_out);

// -------------Tokens-------------

typedef enum {
    TokenEOF = 0,
    TokenPalabraReservada,
    TokenModificador,
    TokenTipoDato,
    TokenID,
    TokenInt,
    TokenFloat,
    TokenChar,
    TokenOp,
    TokenString,
    TokenSeparador,
    TokenError
} TipoToken;

typedef struct Token Token;
struct Token{
    TipoToken tipo;
    char *lexema;
    double valor;
    int numLinea;
    int numColumna;
};

Token getToken();
char* obtenerNombreToken(TipoToken tipo);

void escaparCadena(char *lexema, char *lexemaArreglado);
void formatearLexema(FILE *f_tex, int tipoToken, const char *lexema);
void dividirCodigoSlides(FILE *f_tex);

#endif //COMPILADOR_H