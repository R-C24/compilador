%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    nodoExpresion
} TipoNodo;

struct ASTNodo {
    TipoNodo tipo;
    struct ASTNodo *izq;
    struct ASTNodo *der;
    char *valorLexema;
    double valorNumerico;
};

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema) {
    struct ASTNodo *nuevo = (struct ASTNodo*) malloc(sizeof(struct ASTNodo));
    if (!nuevo) {
        fprintf(stderr, "Error: Memoria agotada al crear nodo del AST\n");
        exit(1);
    }
    nuevo->tipo = tipo;
    nuevo->izq = izq;
    nuevo->der = der;
    nuevo->valorLexema = lexema ? strdup(lexema) : NULL;
    nuevo->valorNumerico = 0.0;
    return nuevo;
}

/* Declaraciones necesarias para conectar con Flex */
extern int yylex(void);
extern int lineaAct;
extern int columnaAct;
extern FILE *yyin;

void yyerror(const char *s);
%}

%define parse.error verbose

/* Definición de los datos que puede almacenar un token/nodo en el parser */
%union {
    struct {
        char *lexema;
        double valor;
        int linea;
        int columna;
    } tokenInfo;

    struct ASTNodo *nodo;
}

/* Definición de Tokens (Generan las constantes en parser.tab.h) */
%token <tokenInfo> TokenInt TokenFloat TokenChar TokenString
%token <tokenInfo> TokenID
%token <tokenInfo> TokenModificador TokenTipoDato
%token <tokenInfo> TokenSeparador

%token <tokenInfo> TokenBreak
%token <tokenInfo> TokenContinue
%token <tokenInfo> TokenEnum
%token <tokenInfo> TokenGoto
%token <tokenInfo> TokenSizeof
%token <tokenInfo> TokenStruct
%token <tokenInfo> TokenTypedef
%token <tokenInfo> TokenUnion
%token <tokenInfo> TokenFor
%token <tokenInfo> TokenIf
%token <tokenInfo> TokenElse
%token <tokenInfo> TokenSwitch
%token <tokenInfo> TokenWhile
%token <tokenInfo> TokenDo
%token <tokenInfo> TokenCase
%token <tokenInfo> TokenReturn
%token <tokenInfo> TokenDefault

%token <tokenInfo> TokenOpAsignacion
%token <tokenInfo> TokenOpLogico
%token <tokenInfo> TokenOpBits
%token <tokenInfo> TokenOpIgualdad
%token <tokenInfo> TokenOpDesplazamiento
%token <tokenInfo> TokenOpAditivos
%token <tokenInfo> TokenOpMultiplicativos
%token <tokenInfo> TokenOpInc

%token <tokenInfo> TokenError

%type <nodo> programa funcion bloqueCodigo relleno condicionIf condicionElseIf loopWhile loopDoWhile loopFor caseSwitch listaCasos caso declaracionVariable expresion

%right TokenOpAsignacion
%left TokenOpLogico
%left TokenOpBits
%left TokenOpIgualdad TokenOpDesplazamiento
%left TokenOpAditivos TokenOpMultiplicativos
%right TokenOpInc

/* Regla inicial de la gramática */
%start programa

%%

/* --- Reglas Gramaticales Iniciales --- */

programa:
    /* Por si no hay nada */ { $$ = crearNodo(nodoPrograma, NULL, NULL, NULL); }
    | programa funcion {
        $$ = crearNodo(nodoPrograma, $1, $2, "programa");
    }
    | programa declaracionVariable {
        $$ = crearNodo(nodoPrograma, $1, $2, "programa");
    }
;

funcion:
    listaModificadores TokenTipoDato TokenID '(' listaParametros ')' ';'{
        $$ = crearNodo(nodoFuncion, $5, NULL, $3.tokenInfo.lexema);
    }
    | listaModificadores TokenTipoDato TokenID '(' listaParametros ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoFuncion, $5, $8, $3.tokenInfo.lexema);
    }
;

bloqueCodigo:
    /* Por si la función está vacía */ { $$ = NULL; }
    | listaDeclaracionVariables listaRelleno {
        $$ = crearNodo(nodoBloqueCodigo, $1, $2, "bloque");
    }
;

listaRelleno:
    /* Por si la función no tiene sentencias */ { $$ = NULL; }
    | listaRelleno relleno {
        $$ = crearNodo(nodoRelleno, $1, $2, "sentencias");
    }
    ;

relleno:
    expresion ';' { $$ = $1; }
    | condicionIf { $$ = $1; }
    | caseSwitch { $$ = $1; }
    | loopWhile { $$ = $1; }
    | loopFor { $$ = $1; }
    | loopDoWhile { $$ = $1; }
    ;

condicionIf:
    TokenIf   '(' expresion ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoCondicionIf, $3, $6, "if");
    }
    | TokenIf '(' expresion ')'  '{' bloqueCodigo '}' condicionElseIf TokenElse '{' bloqueCodigo '}' {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, $3, $6, "if");
        $$ = crearNodo(nodoCondicionIf, ifParcial, $10, "if-else");
    }
    ;

condicionElseIf:
    | TokenIf '(' expresion ')' '{' bloqueCodigo '}' condicionElseIf
    | TokenElse TokenIf '(' expresion ')' '{' bloqueCodigo '}' condicionElseIf
    ;

loopWhile:
    TokenWhile '(' expresion ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoLoopWhile, $3, $6, "while");
    }
    ;

loopDoWhile:
    TokenDo '{' bloqueCodigo '}' TokenWhile '(' expresion ')' ';' {' bloqueCodigo '}' TokenWhile '(' expresion ')' ';' {
        $$ = crearNodo(nodoLoopDoWhile, $3, $7, "do-while");
    }
;

loopFor:
    TokenFor '('inicializacionFor condicional ';' condicional ')' '{' bloqueCodigo '}' {
        struct ASTNodo *cabeceraFor = crearNodo(nodoLoopFor, $3, $5, "for-cabecera");
        $$ = crearNodo(nodoLoopFor, cabeceraFor, $10, "for");
    }
;

inicializacionFor:
    ';' { $$ = $1; } /* Revisarrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr*/
    | declaracionVariable { $$ = $1; }
    | expresion ';' { $$ = $1; }
;

condicional:
    { $$ = NULL; }
    | expresion { $$ = $1; }
;

caseSwitch:
    TokenSwitch '(' expresion ')' '{' listaCasos '}' {
        $$ = crearNodo(nodoCaseSwitch, $3, $6, "switch");
    }
    | TokenSwitch '(' expresion ')' '{' listaCasos TokenDefault ':' bloqueCodigo '}' {
        struct ASTNodo *nodoDef = crearNodo(nodoCaso, NULL, $9, "default");
        $$ = crearNodo(nodoCaseSwitch, $3, $6, "switch-default");
    }
;

listaCasos:
    caso { $$ = $1; }
    | listaCasos caso {
        $$ = crearNodo(nodoListaCasos, $1, $2, "lista-casos");
    }
;

caso:
    TokenCase elemento ':' bloqueCodigo TokenBreak ';' {
        $$ = crearNodo(nodoCaso, $2, $4, "case-break");
    }
    | TokenCase elemento ':' bloqueCodigo {
        $$ = crearNodo(nodoCaso, $2, $4, "case");
    }
;

listaParametros:
    /* para void */
    | parametro
    | listaParametros ',' parametro
    ;

parametro:
    TokenTipoDato TokenID
    | TokenModificador TokenTipoDato TokenID
    ;

listaDeclaracionVariables:
    /* Por si la función no tiene declaraciones */
    | listaDeclaracionVariables declaracionVariable
    ;

declaracionVariable:
    listaModificadores TokenTipoDato TokenID ';'{
        $$ = crearNodo(nodoDeclaracionVariable, NULL, NULL, $3.tokenInfo.lexema);
    }
    | listaModificadores TokenTipoDato TokenID '=' expresion ';'{
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.tokenInfo.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, $5, "=");
    }
    | declaracionArrays { $$ = $1; }
    ;

declaracionArrays:
    TokenTipoDato TokenID '[' TokenInt ']' ';' {
        $$ = crearNodo(nodoDeclaracionVariable, NULL, NULL, $2.tokenInfo.lexema);
    }
    | TokenTipoDato TokenID '[' ']' '=' '{' listaElementos '}' ';' {
        $$ = crearNodo(nodoDeclaracionVariable, $7, NULL, $2.tokenInfo.lexema);
    }
    | TokenTipoDato TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';' {
        $$ = crearNodo(nodoDeclaracionVariable, $7, NULL, $2.tokenInfo.lexema);
    }

listaElementos:
    { $$ = NULL; }
    | elemento { $$ = crearNodo(nodoExpresion, NULL, NULL, "elemento"); }
    | listaElementos ',' elemento { $$ = $1; }
    ;

elemento:
    TokenInt
    | TokenChar
    | TokenFloat
    | TokenString
    ;

listaModificadores:
    /* Por si no tiene modificadores */
    | listaModificadores TokenModificador
    ;

expresion:
    TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.tokenInfo.lexema);
    }
    | TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.tokenInfo.valor;
    }
    | TokenFloat {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.tokenInfo.valor;
    }
    | TokenChar {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.tokenInfo.lexema);
    }
    | TokenString {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.tokenInfo.lexema);
    }
    | '(' expresion ')' {
        $$ = $2;
    }
    | expresion TokenOpAsignacion expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "=");
    }
    | expresion TokenOpLogico expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "logico");
    }
    | expresion TokenOpBits expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "bits");
    }
    | expresion TokenOpIgualdad expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "igualdad");
    }
    | expresion TokenOpDesplazamiento expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "desplazamiento");
    }
    | expresion TokenOpAditivos expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "aditivo");
    }
    | expresion TokenOpMultiplicativos expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "multiplicativo");
    }
    | TokenID '=' expresion

    | TokenID TokenInc {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "++");
    }
    | TokenInc TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "++");
    }
    | TokenID '(' listaArgumentosOpt ')' {
        $$ = crearNodo(nodoExpresion, $3, NULL, $1.tokenInfo.lexema);
    }
;

listaArgumentosOpt:
    { $$ = NULL; }
    | listaArgumentos { $$ = $1; }
;

listaArgumentos:
    expresion { $$ = $1; }
    | listaArgumentos ',' expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "arg");
    }
;

%%

/* Manejo de errores sintácticos */
void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en línea %d, columna %d: %s\n", lineaAct, columnaAct, s);
}