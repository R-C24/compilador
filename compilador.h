#ifndef COMPILADOR_H
#define COMPILADOR_H

#define MaxMacros 100
#define MaxNombre 64
#define MaxValor 256
#define TamBuffer 1024
#define MaxTipos 256

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

int esTipoUsuario(char *s);
void registrarTipoDeclarado(struct ASTNodo *n);

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema);
void imprimirAST(struct ASTNodo *nodo, int nivel);

typedef enum {
    propio,
    estiloGNU,
    estiloBSD
} EstiloPrettyPrint;

#endif //COMPILADOR_H