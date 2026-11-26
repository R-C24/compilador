#ifndef COMPILADOR_H
#define COMPILADOR_H

#define MaxMacros 100
#define MaxNombre 64
#define MaxValor 256
#define TamBuffer 2048
#define MaxTipos 256
#define MaxAmbitos 500 //FALTAN PRUEBAS DE ESTRES
#define MaxArchivosIncluidos 64
#define MaxProfundidadInclude 32
#define MaxLoops 64
#define MaxString 128

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

extern char archivosEnProceso[MaxProfundidadInclude][MaxNombre];
extern int profundidadInclude;

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

int esTipoUsuario(char *s);

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
    int offset;
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

struct ASTNodo {
    TipoNodo tipo;
    struct ASTNodo *izq;
    struct ASTNodo *der;
    char *valorLexema;
    double valorNumerico;
    int lineaAct;
    int columnaAct;
    RegistroSemantico *simbolo;
    int bytesLocales;
};

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

void registrarSimbolosEstandar();
char* crearTipoPuntero(const char *tipoBase);
void agregarPunteroAUltimoSimbolo();
const char* obtenerNombreCategoria(CategoriaSimbolo cat);
int esInclusionCircular(const char *nombreArchivo);
void limpiarTemporalAlSalir();

int esFuncionEstandar(const char *lexema);
int esConstanteEstandar(const char *lexema);
void validarLlamadaFuncion(const char *lexema, int lineaAct, int columnaAct);
void formatearTipoConDimensiones(const char *tipoBase, struct ASTNodo *dims, char *salida, size_t tam);
int contarPunteros(struct ASTNodo *nodo);

// ------------- Generación de código -------------

extern int contadorTemporales;
extern int contadorEtiquetas;
extern int offsetPilaActual;

extern int offsetFuncionActual;
extern RegistroSemantico *historicoSimbolos[4096];
extern int totalHistoricoSimbolos;

extern const char *RegArgs32[6];
extern const char *RegArgs64[6];

extern FILE *archivoASM;
extern int errorGeneracion;

typedef enum {
    IdExpr,
    LiteralExpr,
    TempExpr
} ExprKind;

typedef struct {
    ExprKind kind;
    char nombre[64];
    char direccion[64];
    int valor;
} ExprRec;

typedef struct {
    char operador[16]; 
} OpRec;

typedef struct {
    const struct ASTNodo *nodo;
    char etiqueta[32];
} LiteralCadena;

extern LiteralCadena tablaCadenas[MaxString];
extern int totalCadenas;

int generarCodigo(struct ASTNodo *raiz, const char *rutaSalidaS);
void start(void);
void finish(struct ASTNodo *raiz);
void generate(const char *opcode, const char *arg1, const char *arg2, const char *res);

ExprRec genInfix(ExprRec e1, OpRec op, ExprRec e2);
void assignX86(const char *objDir, ExprRec fuente);
ExprRec processIdNodo(struct ASTNodo *nodo);
ExprRec processLiteral(int valor);
void readId(ExprRec inVar);
void writeExpr(ExprRec outExpr);

void recorrerNodo(struct ASTNodo *nodo);
ExprRec evaluarExpresion(struct ASTNodo *nodo);

void obtenerOperando(ExprRec e, char *dest, size_t tam);
ExprRec evaluarExpresion(struct ASTNodo *nodo);
void procesarDeclaracion(struct ASTNodo *nodo);
void recorrerNodo(struct ASTNodo *nodo);

void liberarSimbolos();
int calcularEspacioPila(struct ASTNodo *nodoFuncion);
void emitirGlobalesAST(struct ASTNodo *nodo);

ExprRec genIncDec(struct ASTNodo *operandoNodo, const char *op, int esPrefijo);

extern char pilaBreak[MaxLoops][32];
extern char pilaContinue[MaxLoops][32];
extern int topeLoops;

void pushLoop(const char *etiqBreak, const char *etiqContinue);
void popLoop();
void saltarSiFalso(struct ASTNodo *condNodo, const char *etiquetaFalso);
void genIfCadena(struct ASTNodo *nodo, const char *etiqFinGlobal);
void genIf(struct ASTNodo *nodo);
void genWhile(struct ASTNodo *nodo);
void genDoWhile(struct ASTNodo *nodo);
void genFor(struct ASTNodo *nodo);
ExprRec genRelacional(ExprRec e1, const char *op, ExprRec e2);

void recolectarCadenasAST(struct ASTNodo *nodo);
const char *obtenerEtiquetaCadena(const struct ASTNodo *nodo);
int aplanarParametros(struct ASTNodo *nodo, struct ASTNodo **params, int maxParams);
int aplanarArgumentos(struct ASTNodo *nodo, struct ASTNodo **args, int maxArgs);

#endif //COMPILADOR_H