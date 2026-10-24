%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

/* Declaraciones necesarias para conectar con Flex */
extern int yylex(void);
extern int lineaAct;
extern int columnaAct;
extern FILE *yyin;

void yyerror(const char *s);

struct ASTNodo *raizAST = NULL;
%}

%define parse.error verbose

/* Definición de los datos que puede almacenar un token/nodo en el parser */
%union {
    InfoToken tokenInfo;
    struct ASTNodo *nodo;
}


/* Definición de Tokens (Generan las constantes en parser.tab.h) */

%token <tokenInfo> TokenInt TokenFloat TokenChar TokenString TokenID
%token <tokenInfo> TokenModificador TokenTipoDato
%token <tokenInfo> TokenBreak TokenContinue TokenEnum TokenGoto TokenSizeof
%token <tokenInfo> TokenStruct TokenTypedef TokenUnion TokenFor TokenIf
%token <tokenInfo> TokenElse TokenSwitch TokenWhile TokenDo TokenCase
%token <tokenInfo> TokenReturn TokenDefault TokenOpAsignacion TokenOpLogico
%token <tokenInfo> TokenOpBits TokenOpIgualdad TokenOpDesplazamiento
%token <tokenInfo> TokenOpAditivos TokenOpMultiplicativos TokenOpInc TokenError

%type <nodo> programa funcion retornar bloqueCodigo listaRelleno relleno condicionIf condicionElseIf loopWhile loopDoWhile loopFor inicializacionFor condicional caseSwitch listaCasos caso listaParametros parametro listaDeclaracionVariables declaracionVariable declaracionArrays listaElementos elemento listaModificadores expresion listaArgumentosOpt listaArgumentos llamadaFuncion especificadores

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
        raizAST = $$;
    }
    | programa declaracionVariable {
        $$ = crearNodo(nodoPrograma, $1, $2, "programa");
        raizAST = $$;
    }
;

funcion:
    especificadores TokenID '(' listaParametros ')' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, NULL, $2.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '(' listaParametros ')' '{' bloqueCodigo '}' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $7, $2.lexema);
        raizAST = $$;
    }
;

retornar:
    TokenReturn ';' { 
        $$ = crearNodo(nodoExpresion, NULL, NULL, "return");
        raizAST = $$;
        }
    | TokenReturn expresion ';' {
        $$ = crearNodo(nodoExpresion, $2, NULL, "return");
        raizAST = $$;
        }
    ;

bloqueCodigo:
    /* Por si la función está vacía */ { $$ = NULL; raizAST = $$;}
    | listaDeclaracionVariables listaRelleno {
        $$ = crearNodo(nodoBloqueCodigo, $1, $2, "bloque");
        raizAST = $$;
    }
;

listaRelleno:
    /* Por si la función no tiene sentencias */ { $$ = NULL; raizAST = $$;}
    | listaRelleno relleno {
        $$ = crearNodo(nodoRelleno, $1, $2, "sentencias");
        raizAST = $$;
    }
    ;

relleno:
    expresion ';' { $$ = $1; raizAST = $$;}
    | condicionIf { $$ = $1; raizAST = $$;}
    | caseSwitch { $$ = $1; raizAST = $$;}
    | loopWhile { $$ = $1; raizAST = $$;}
    | loopFor { $$ = $1; raizAST = $$;}
    | loopDoWhile { $$ = $1; raizAST = $$;}
    | retornar { $$ = $1; raizAST = $$;}
    ;

condicionIf:
    TokenIf '(' expresion ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoCondicionIf, $3, $6, "if");
        raizAST = $$;
    }
    | TokenIf '(' expresion ')'  '{' bloqueCodigo '}' TokenElse '{' bloqueCodigo '}' {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, $3, $6, "if");
        $$ = crearNodo(nodoCondicionIf, ifParcial, $10, "if-else");
        raizAST = $$;
    }
    | TokenIf '(' expresion ')' '{' bloqueCodigo '}' condicionElseIf {
        $$ = crearNodo(nodoCondicionIf, $3, $8, "if-elseif");
        raizAST = $$;
    }
    | TokenIf '(' expresion ')'  '{' bloqueCodigo '}' condicionElseIf TokenElse '{' bloqueCodigo '}' {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, $3, $6, "if");
        $$ = crearNodo(nodoCondicionIf, ifParcial, $11, "if-elseif-else");
        raizAST = $$;
    }
    ;

condicionElseIf:
    TokenElse TokenIf '(' expresion ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoCondicionIf, $4, $7, "else-if");
        raizAST = $$;
    }
    | condicionElseIf TokenElse TokenIf '(' expresion ')' '{' bloqueCodigo '}' {
        struct ASTNodo *nuevoElseIf = crearNodo(nodoCondicionIf, $5, $8, "else-if");
        $$ = crearNodo(nodoCondicionIf, $1, nuevoElseIf, "cadena-else-if");
        raizAST = $$;
   }
    ;

loopWhile:
    TokenWhile '(' expresion ')' '{' bloqueCodigo '}' {
        $$ = crearNodo(nodoLoopWhile, $3, $6, "while");
        raizAST = $$;
    }
    ;

loopDoWhile:
    TokenDo '{' bloqueCodigo '}' TokenWhile '(' expresion ')' ';' {
        $$ = crearNodo(nodoLoopDoWhile, $3, $7, "do-while");
        raizAST = $$;
    }
;

loopFor:
    TokenFor '('inicializacionFor condicional ';' condicional ')' '{' bloqueCodigo '}' {
        struct ASTNodo *condInc = crearNodo(nodoLoopFor, $4, $6, "for-cond-inc");
        struct ASTNodo *cabeceraFor = crearNodo(nodoLoopFor, $3, condInc, "for-cabecera");
        $$ = crearNodo(nodoLoopFor, cabeceraFor, $9, "for");
        raizAST = $$;
    }
;

inicializacionFor:
    ';' { $$ = NULL; raizAST = $$;} /* Revisarrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrrr*/
    | declaracionVariable { $$ = $1; raizAST = $$;}
    | expresion ';' { $$ = $1; raizAST = $$;}
;

condicional:
    { $$ = NULL; raizAST = $$;}
    | expresion { $$ = $1; raizAST = $$;}
;

caseSwitch:
    TokenSwitch '(' expresion ')' '{' listaCasos '}' {
        $$ = crearNodo(nodoCaseSwitch, $3, $6, "switch");
        raizAST = $$;
    }
    | TokenSwitch '(' expresion ')' '{' listaCasos TokenDefault ':' bloqueCodigo '}' {
        struct ASTNodo *nodoDef = crearNodo(nodoCaso, NULL, $9, "default");
        struct ASTNodo *listaCompleta = crearNodo(nodoListaCasos, $6, nodoDef, "lista-casos");
        $$ = crearNodo(nodoCaseSwitch, $3, listaCompleta, "switch-default");
        raizAST = $$;
    }
    | TokenSwitch '(' expresion ')' '{' listaCasos TokenDefault ':' bloqueCodigo TokenBreak ';' '}' {
        struct ASTNodo *nodoBreak = crearNodo(nodoExpresion, NULL, NULL, "break");
        struct ASTNodo *cuerpoConBreak = crearNodo(nodoBloqueCodigo, $9, nodoBreak, "bloque-break");
        struct ASTNodo *nodoDef = crearNodo(nodoCaso, NULL, cuerpoConBreak, "default-break");
        struct ASTNodo *listaCompleta = crearNodo(nodoListaCasos, $6, nodoDef, "lista-casos");
        $$ = crearNodo(nodoCaseSwitch, $3, listaCompleta, "switch-default");
        raizAST = $$;
    }
    ;

listaCasos:
    caso { $$ = $1; raizAST = $$;}
    | listaCasos caso {
        $$ = crearNodo(nodoListaCasos, $1, $2, "lista-casos");
        raizAST = $$;
    }
;

caso:
    TokenCase elemento ':' bloqueCodigo TokenBreak ';' {
        struct ASTNodo *nodoBreak = crearNodo(nodoExpresion, NULL, NULL, "break");
        struct ASTNodo *cuerpoConBreak = crearNodo(nodoBloqueCodigo, $4, nodoBreak, "bloque-break");
        $$ = crearNodo(nodoCaso, $2, cuerpoConBreak, "case-break");
        raizAST = $$;
    }
    | TokenCase elemento ':' bloqueCodigo {
        $$ = crearNodo(nodoCaso, $2, $4, "case");
        raizAST = $$;
    }
    ;

listaParametros:
    { $$ = NULL; raizAST = $$;}
    | parametro { $$ = $1; raizAST = $$;}
    | listaParametros ',' parametro {
        $$ = crearNodo(nodoExpresion, $1, $3, "lista-parametros");
        raizAST = $$;
    }
    ;

parametro:
    especificadores TokenID {
        // $1 contiene la estructura completa de los especificadores acumulados
        // $2.lexema contiene el nombre del parámetro
        $$ = crearNodo(nodoExpresion, $1, NULL, $2.lexema);
        raizAST = $$;
    }
    ;

listaDeclaracionVariables:
    { $$ = NULL; raizAST = $$;} /* Por si la función no tiene declaraciones */
    | listaDeclaracionVariables declaracionVariable {
        $$ = crearNodo(nodoBloqueCodigo, $1, $2, "lista-declaraciones");
        raizAST = $$;
    }
    ;

declaracionVariable:
    especificadores TokenID ';' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, $1, $2.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '=' expresion ';' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, idNodo, $1, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, $4, "=");
        raizAST = $$;
    }
    | declaracionArrays {
        $$ = $1;
        raizAST = $$;
    }
    ;

declaracionArrays:
    especificadores TokenID '[' TokenInt ']' ';' {
        struct ASTNodo *tamano = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, tamano, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, NULL, "array");
        raizAST = $$;
    }
    | especificadores TokenID '[' ']' '=' '{' listaElementos '}' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, NULL, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, $7, "array-init");
        raizAST = $$;
    }
    | especificadores TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';' {
        struct ASTNodo *tamano = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, tamano, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, $8, "array-init");
        raizAST = $$;
    }
    ;

especificadores:
    TokenTipoDato {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenTipoDato {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod, NULL, $2.lexema);
        raizAST = $$;
    }
    | TokenModificador {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenTipoDato {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenModificador {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenModificador TokenTipoDato {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *mod3 = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        $$ = crearNodo(nodoExpresion, mod3, NULL, $4.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenModificador {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        raizAST = $$;
    }
    ;

listaElementos:
    { $$ = NULL; raizAST = $$;}
    | elemento { $$ = $1; raizAST = $$;}
    | listaElementos ',' elemento {
         $$ = crearNodo(nodoExpresion, $1, $3, "lista-elementos");
         raizAST = $$;
        }
    ;

elemento:
    TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.valor;
        raizAST = $$;
    }
    | TokenChar {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenFloat {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.valor;
        raizAST = $$;
    }
    | TokenString {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
;

listaModificadores:
    TokenModificador {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | listaModificadores TokenModificador {
        $$ = crearNodo(nodoExpresion, $1, NULL, $2.lexema);
        raizAST = $$;
    }
;

expresion:
    TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$->valorNumerico = $1.valor;
        raizAST = $$;
    }
    | TokenFloat {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$->valorNumerico = $1.valor;
        raizAST = $$;
    }
    | TokenChar {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenOpAditivos TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        // Si el operador es '-', hacemos que el valor sea negativo (-1 * valor)
        char temp[128];
        snprintf(temp, sizeof(temp), "%s%s", $1.lexema, $2.lexema);
        $$->valorLexema = strdup(temp);

        if (strcmp($1.lexema, "-") == 0) {
            $$->valorNumerico = -($2.valor);
        } else {
            $$->valorNumerico = $2.valor;
        }
        raizAST = $$;
    }
    | TokenOpAditivos TokenFloat {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        char temp[128];
        snprintf(temp, sizeof(temp), "%s%s", $1.lexema, $2.lexema);
        $$->valorLexema = strdup(temp);

        if (strcmp($1.lexema, "-") == 0) {
            $$->valorNumerico = -($2.valor);
        } else {
            $$->valorNumerico = $2.valor;
        }
        raizAST = $$;
    }
    | TokenString {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | '(' expresion ')' {
        $$ = $2;
        raizAST = $$;
    }
    | expresion '=' expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "=");
        raizAST = $$;
    }
    | expresion TokenOpAsignacion expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpLogico expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpBits expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpIgualdad expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpDesplazamiento expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpAditivos expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | expresion TokenOpMultiplicativos expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        raizAST = $$;
    }
    | TokenID TokenOpInc {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        raizAST = $$;
    }
    | TokenOpInc TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        raizAST = $$;
    }
    | TokenID '(' listaArgumentosOpt ')' {
        $$ = crearNodo(nodoLlamadaFuncion, $3, NULL, $1.lexema);
        raizAST = $$;
    }
;

listaArgumentosOpt:
    { $$ = NULL; raizAST = $$; }
    | listaArgumentos { $$ = $1; raizAST = $$; }
;

listaArgumentos:
    expresion { $$ = $1; raizAST = $$;}
    | listaArgumentos ',' expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "arg");
        raizAST = $$;
    }
;

%%

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

/* Manejo de errores sintácticos */
void yyerror(const char *s) {
    fprintf(stderr, "Error sintáctico en línea %d, columna %d: %s\n", lineaAct, columnaAct, s);
}