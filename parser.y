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
    listaUnidadesTraduccion
    ;

listaUnidadesTraduccion:
    unidadTraduccion
    | listaUnidadesTraduccion unidadTraduccion
    ;

unidadTraduccion:
    declaracionGlobal
    ;

declaracionGlobal:
      TokenTipoDato TokenID TokenSeparador
    | TokenPalabraReservada TokenSeparador
    | TokenSeparador
    ;

%%

/* Manejo de errores sintácticos */
void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en línea %d, columna %d: %s\n", lineaAct, columnaAct, s);
}
