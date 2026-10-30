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
extern int totalErrores;
extern const char *archivoFuente;

void agregarMacro(char *nombre, char *valor);
void traducirMacros(char *linea);
int esCaracterIdentificador(char c);
int esEscapado(char *str, int pos);
char *buscarMacroValida(char *linea, char *nombre);

void procesarArchivo(char *nombreArchivo, FILE *f_out);

// -------------Tokens-------------

typedef struct {
    char *lexema;
    double valor;
    int linea;
    int columna;
} InfoToken;

void escaparCadena(char *lexema, char *lexemaArreglado);
void formatearLexema(FILE *f_tex, int tipoToken, const char *lexema);
void dividirCodigoSlides(FILE *f_tex);

typedef enum {
    nodoPrograma,
    nodoFuncion,
    nodoBloqueCodigo,
    nodoRelleno,
    nodoCondicionIf,
    nodoCondicionElseIf,
    nodoLoopWhile,
    nodoLoopDoWhile,
    nodoLoopFor,
    nodoCaseSwitch,
    nodoListaCasos,
    nodoCaso,
    nodoDeclaracionVariable,
    nodoExpresion,
    nodoLlamadaFuncion
} TipoNodo;

struct ASTNodo {
    TipoNodo tipo;
    struct ASTNodo *izq;
    struct ASTNodo *der;
    char *valorLexema;
    double valorNumerico;
};

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema);
void imprimirAST(struct ASTNodo *nodo, int nivel);

typedef enum {
    propio,
    estiloGNU,
    estiloBSD
} EstiloPrettyPrint;

void prettyPrintAST(struct ASTNodo *nodo, FILE *f, int nivel);
void printDeclaracionVariable(struct ASTNodo *nodo, FILE *f, int nivel);
void printCondicionIf(struct ASTNodo *nodo, FILE *f, int nivel);
void printLoopWhile(struct ASTNodo *nodo, FILE *f, int nivel);
void printDeclaracionFor(struct ASTNodo *nodo, FILE *f);
void printLoopFor(struct ASTNodo *nodo, FILE *f, int nivel);
void printExpresion(struct ASTNodo *nodo, FILE *f, int nivel);
void printFuncion(struct ASTNodo *nodo, FILE *f, int nivel);
void printParametros(struct ASTNodo *nodo, FILE *f);
void printCaseSwitch(struct ASTNodo *nodo, FILE *f, int nivel);
void printListaCasos(struct ASTNodo *nodo, FILE *f, int nivel);
void printCaso(struct ASTNodo *nodo, FILE *f, int nivel);
void printLoopDoWhile(struct ASTNodo *nodo, FILE *f, int nivel);
void printArgumentos(struct ASTNodo *nodo, FILE *f);
void printLlamadaFuncion(struct ASTNodo *nodo, FILE *f);
void printEspecificadores(struct ASTNodo *nodo, FILE *f);
void printListaIds(struct ASTNodo *nodo, FILE *f);

#endif //COMPILADOR_H