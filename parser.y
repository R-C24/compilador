%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    /*  REVISAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAR struct ASTNodo *nodo;  */
}

/* Definición de Tokens (Generan las constantes en parser.tab.h) */
%token <tokenInfo> TokenInt TokenFloat TokenChar TokenString
%token <tokenInfo> TokenID
%token <tokenInfo> TokenPalabraReservada TokenModificador TokenTipoDato
%token <tokenInfo> TokenOp TokenSeparador
%token <tokenInfo> TokenError

%type <nodo> programa listaUnidadesTraduccion declaracionGlobal expresion

/* Regla inicial de la gramática */
%start programa

%%

/* --- Reglas Gramaticales Iniciales --- */

programa:
    /* Por si no hay nada */
    | funcion
    | declaracionVariable
    ;

funcion:
    listaModificadores TokenTipoDato TokenID '(' listaParametros ')' ';'
    | listaModificadores TokenTipoDato TokenID '(' listaParametros ')' '{' bloqueCodigo '}' ';'
    ;

bloqueCodigo:
    /* Por si la función está vacía */
    | listaDeclaracionVariables listaRelleno
;

listaRelleno:
   /* Por si la función no tiene sentencias */
   | listaRelleno relleno
    ;

relleno:
    TokenPalabraReservada ';' /* (como break; continue; return;) */
    | expresion
    | condicionIf
    | caseSwitch
    | loopWhile
    | loopFor
    | loopDoWhile
    |
    ;

condicionIf:
    TokenIf   '(' expresion ')' '{' bloqueCodigo '}'
    | TokenIf '(' expresion ')'  '{' bloqueCodigo '}' condicionElseIf TokenElse '{' bloqueCodigo '}'
    ;

condicionElseIf:
    | TokenIf '(' expresion ')' '{' bloqueCodigo '}' condicionElseIf
    | TokenElse TokenIf '(' expresion ')' '{' bloqueCodigo '}' condicionElseIf
    ;

loopWhile:
    TokenWhile '(' expresion ')' '{' bloqueCodigo '}'
    ;

loopDoWhile:
    TokenDo '{' bloqueCodigo '}' TokenWhile '(' expresion ')' ';'
;

loopFor:
    TokenFor '(' TokenTipoDato TokenID '=' elemento ';' condicional ';' expresion ')' '{' bloqueCodigo '}' ';'
;

condicional:
    | expresion
;

caseSwitch:
    TokenSwitch '(' expresion ')' '{' listaCasos '}'
    | TokenSwitch '(' expresion ')' '{' listaCasos TokenDefault ':' bloqueCodigo '}'
;

listaCasos:
    caso
    | listaCasos caso
;

caso:
    TokenCase elemento ':' bloqueCodigo TokenBreak ';'
    | TokenCase elemento ':' bloqueCodigo
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
    listaModificadores TokenTipoDato TokenID ';'
    | listaModificadores TokenTipoDato TokenID '=' expresion ';'
    ;

declaracionArrays:
    TokenTipoDato TokenID '[' TokenInt ']' ';'
    | TokenTipoDato TokenID '[' ']' '=' '{' listaElementos '}' ';'
    | TokenTipoDato TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';'

listaElementos:

    | elemento
    | listaElementos ',' elemento
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
    TokenID
    | TokenInt
    | TokenFloat
    | TokenChar
    | TokenString
    | '(' expresion ')'
    | expresion TokenOp expresion
    | TokenID '=' expresion
    | TokenID TokenOp expresion
    | TokenID '++'
    | TokenID '--'
    | TokenID '(' listaArgumentosOpt ')'
;

listaArgumentosOpt:
    | listaArgumentos
;

listaArgumentos:
    expresion
    | listaArgumentos ',' expresion
;

%%

/* Manejo de errores sintácticos */
void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en línea %d, columna %d: %s\n", lineaAct, columnaAct, s);
}