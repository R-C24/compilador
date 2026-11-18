#ifndef COMPILADOR_H
#define COMPILADOR_H

#define MaxMacros 100
#define MaxNombre 64
#define MaxValor 256
#define TamBuffer 1024
#define MaxTipos 256
#define MaxAmbitos 500 //FALTAN PRUEBAS DE ESTRES

typedef struct Macro Macro;
struct Macro{
    char nombre[MaxNombre];
    char parametro[MaxNombre];
    char valor[MaxValor];
};

extern Macro macros[MaxMacros];
extern int totalMacros;
extern int totalErrores;
extern const char *archivoFuente;

extern char *tiposUsuario[MaxTipos];
extern int totalTiposUsuario;
extern char *tiposEstandar[];
extern int totalErrores;

void agregarMacro(char *nombre, char *valor);
int macroDefinida(const char *nombre);
void agregarMacroParam(const char *nombre, const char *parametro, const char *valor);

void sustituirParametroEnCuerpo(const char *cuerpo, const char *param, const char *arg, char *salida, size_t maxLen);

void traducirMacros(char *linea);
int esCaracterIdentificador(char c);
int esEscapado(char *str, int pos);
char *buscarMacroValida(char *linea, char *nombre);

void recortarDerecha(char *str);

void procesarArchivo(char *nombreArchivo, FILE *f_out);


// -------------Tokens-------------

typedef struct {
    char *lexema;
    double valor;
    int linea;
    int columna;
} InfoToken;

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
    int lineaAct;
    int columnaAct;
};

int esTipoUsuario(char *s);
void registrarTipoDeclarado(struct ASTNodo *n);

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema);

// -------------Registro, Tabla y Pila-------------

extern char funcionActual[256];
extern int contextoImpreso;
extern FILE *salidaTablas;

typedef enum {
    tipoVariable,
    tipoFuncion,
    tipoParametro
} CategoriaSimbolo;
 
typedef struct RegistroSemantico RegistroSemantico;

struct RegistroSemantico {
    char *lexema;
    char *tipoDato;
    CategoriaSimbolo categoria;
    int nivelAmbito; 
    int lineaDeclaracion;
    int columnaDeclaracion;
    int esPrototipo;
    struct RegistroSemantico *siguiente;
};

typedef struct {
    RegistroSemantico *raiz; 
    char *nombreFuncion;
    char *parametrosFuncion;
} TablaSimbolos;

extern TablaSimbolos pilaTablas[MaxAmbitos];
extern int topePila;
extern char tipoActual[256];

void inicializarPila();
void pushAmbitoConContexto(const char *funcion, const char *parametros);
void pushAmbito();
void popAmbito();
RegistroSemantico* insertarSimbolo(char *lexema, char *tipo_dato, CategoriaSimbolo categoria, int linea, int columna);
RegistroSemantico* buscarSimbolo(char *lexema);
RegistroSemantico* buscarEnAmbitoActual(const char *lexema);
void imprimirTablaActual();
void validarYRegistrarSimbolo(const char *lexema, const char *tipoDetectado, CategoriaSimbolo cat, int lineaAct, int columnaAct);
void validarYRegistrarFuncion(const char *lexema, const char *tipoRet, int esDefinicion, int linea, int col);

void registrarTipoDeclarado(struct ASTNodo *n);
void registrarTipo(char *s);
int esTipoUsuario(char *s);
void yyerror(const char *s);
void imprimirLineaErrorGCC(const char *archivo, int numLinea, int numColumna, int longitudToken);
struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema);
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando);

void reportarUsoNoDeclarado(const char *archivo, const char *lexema, int lineaErr, int colErr, int lenErr);
void validarUsoSimbolo(const char *lexema, int lineaAct, int columnaAct);
void registrarParametrosEnTabla(struct ASTNodo *nodo);
char* obtenerTextoParametros(struct ASTNodo *nodoParams);
void obtenerTextoParametrosAux(struct ASTNodo *nodo, char *buffer, size_t tam);
struct ASTNodo* extraerNodoIdParametro(struct ASTNodo *nodo);

void registrarSimbolosEstandar()

#endif //COMPILADOR_H