%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

/* Declaraciones necesarias para conectar con el scanner (Flex) */
extern int yylex(void);
extern int lineaAct;
extern int columnaAct;
extern FILE *yyin;

extern int yyleng;
extern const char *archivoFuente;

char *tiposUsuario[MaxTipos];
int totalTiposUsuario = 0;

char *tiposEstandar[] = {
    "FILE", "DIR", "size_t", "ssize_t", "ptrdiff_t", "wchar_t", "time_t", "clock_t",
    "off_t", "pid_t", "va_list", "bool", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t", "intptr_t", "uintptr_t", NULL
};

int totalErrores = 0;

void yyerror(const char *s);
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando);

struct ASTNodo *raizAST = NULL;

%}

%define parse.error verbose

/* Definición de los datos que puede almacenar un nodo en el parser */
%union {
    InfoToken tokenInfo;
    struct ASTNodo *nodo;
}


/* Definición de Tokens*/

%token <tokenInfo> TokenInt TokenFloat TokenChar TokenString TokenID TokenVoid
%token <tokenInfo> TokenModificador TokenTipoDato TokenFlecha
%token <tokenInfo> TokenNombreTipo
%token <tokenInfo> TokenBreak TokenContinue TokenEnum TokenGoto TokenSizeof
%token <tokenInfo> TokenStruct TokenTypedef TokenUnion TokenFor TokenIf
%token <tokenInfo> TokenElse TokenSwitch TokenWhile TokenDo TokenCase
%token <tokenInfo> TokenReturn TokenDefault TokenOpAsignacion TokenOpLogico
%token <tokenInfo> TokenOpBits TokenOpIgualdad TokenOpDesplazamiento
%token <tokenInfo> TokenOpAditivos TokenOpMultiplicativos TokenOpInc TokenError

%type <nodo> tipoCast programa funcion retornar bloqueCodigo sentenciaBloque sentenciaSimple condicionIf loopWhile loopDoWhile loopFor inicializacionFor condicional elementoCase caseSwitch listaCasos caso listaParametros parametro listaDeclaracionVariables declaracionVariable declaracionArrays listaDimensiones listaElementos elemento listaModificadores expresion listaArgumentosOpt listaArgumentos especificadores listaIds elementoInit

%right '=' TokenOpAsignacion
%right '?' ':'
%left TokenOpLogico
%left TokenOpBits
%left TokenOpIgualdad TokenOpDesplazamiento
%left TokenOpAditivos TokenOpMultiplicativos
%right UNARIO
%left TokenOpInc '[' ']' TokenFlecha '.'

/* Regla inicial */
%start programa

%%

/* Reglas Gramaticales */

programa:
    /* Por si el archivo está vacío */
    {
        $$ = crearNodo(nodoPrograma, NULL, NULL, NULL);
        raizAST = $$;
    }
    | programa funcion {
        $$ = crearNodo(nodoPrograma, $1, $2, "programa");
        raizAST = $$;
    }
    | programa declaracionVariable {
        $$ = crearNodo(nodoPrograma, $1, $2, "programa");
        raizAST = $$;
    }
    /* Llave '}' huérfana fuera de cualquier función */
    | programa '}' {
        yyerror("sintaxis inválida: '}' extra o fuera de cualquier función");
        yyerrok;
        $$ = $1; // Conserva lo procesado hasta el momento
        raizAST = $$;
    }
    /* Puntos y coma extra a nivel global */
    | programa ';' {
        $$ = $1;
        raizAST = $$;
    }
    /* Para cualquier otro token corrupto a nivel global */
    | programa error ';' {
        yyerrok;
        $$ = $1;
        raizAST = $$;
    }
;

funcion:
    especificadores TokenID '(' listaParametros ')' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, NULL, $2.lexema);
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID '(' listaParametros ')' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $5, $3.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, NULL, $3.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '(' listaParametros ')' '{' bloqueCodigo '}' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $7, $2.lexema);
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID '(' listaParametros ')' '{' bloqueCodigo '}' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $5, $3.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $8, $3.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '(' listaParametros ')' sentenciaBloque {
        yyerror("se esperaba '{' al inicio del cuerpo de la función");
        yyerrok;
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $6, $2.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '(' listaParametros '{' {
        yyerror("falta ')' en la declaración de la función");
        YYABORT; // Bloquea errores en cascada
    }
    | especificadores TokenID '(' error '}' {
        yyerror("error de sintaxis en la firma de la función");
        YYABORT;
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
    | TokenReturn expresion {  // Sin punto y coma al final de línea
        yyerror("falta ';' después de return");
        yyerrok;
        $$ = crearNodo(nodoExpresion, $2, NULL, "return");
        raizAST = $$;
    }
    | TokenReturn { // Return vacío sin punto y coma
        yyerror("falta ';' después de return");
        yyerrok;
        $$ = crearNodo(nodoExpresion, NULL, NULL, "return");
        raizAST = $$;
    }
;

bloqueCodigo:
    /* Por si el bloque está vacío */
    {
        $$ = NULL;
    }
    | bloqueCodigo sentenciaBloque
    {
        if ($1 == NULL) {
            $$ = $2;
        } else {
            $$ = crearNodo(nodoBloqueCodigo, $1, $2, "bloque");
        }
        raizAST = $$;
    }
;

sentenciaBloque:
      declaracionVariable { $$ = $1; raizAST = $$; }
    | sentenciaSimple     { $$ = $1; raizAST = $$; }
    | funcion {
        yyerror("se esperaba '}' al final de la función anterior");
        yyerrok;
        $$ = $1;
        raizAST = $$;
    }
    | error ';' {
        yyerrok;
        $$ = NULL;
    }
    | error '}' {
        yyerrok;
        $$ = NULL;
    }
;


/* Sentencia que puede ir sola como cuerpo de if/else */
sentenciaSimple:
      expresion ';'        { $$ = $1; raizAST = $$; }
    | condicionIf          { $$ = $1; raizAST = $$; }
    | caseSwitch           { $$ = $1; raizAST = $$; }
    | loopWhile            { $$ = $1; raizAST = $$; }
    | loopFor              { $$ = $1; raizAST = $$; }
    | loopDoWhile          { $$ = $1; raizAST = $$; }
    | retornar             { $$ = $1; raizAST = $$; }
    | TokenBreak ';' {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "break");
        raizAST = $$;
    }
    | TokenContinue ';' {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "continue");
        raizAST = $$;
    }
    | '{' bloqueCodigo '}' { $$ = $2; raizAST = $$; }
    | ';'                  { $$ = NULL; }
    | TokenElse sentenciaSimple {
        yyerror("'else' sin un 'if' previo");
        yyerrok;
        $$ = $2;
        raizAST = $$;
    }
    | TokenGoto TokenID ';' {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "goto");
        raizAST = $$;
    }
    | TokenID ':' sentenciaSimple {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $3, "etiqueta");
        raizAST = $$;
    }
;

condicionIf:
    TokenIf '(' expresion ')' sentenciaSimple {
        $$ = crearNodo(nodoCondicionIf, $3, $5, "if");
        raizAST = $$;
    }
    | TokenIf '(' expresion ')' sentenciaSimple TokenElse sentenciaSimple {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, $3, $5, "if");
        $$ = crearNodo(nodoCondicionIf, ifParcial, $7, "if-else");
        raizAST = $$;
    }
    | TokenIf '(' expresion '{' bloqueCodigo '}' {
        yyerror("se esperaba ')' antes del token '{'");
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, $3, $5, "if-error");
        raizAST = $$;
    }
    | TokenIf '(' expresion sentenciaSimple {
        yyerror("se esperaba ')' después de la condición del 'if'");
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, $3, $4, "if-error");
        raizAST = $$;
    }
    | TokenIf error '{' bloqueCodigo '}' {
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, NULL, $4, "if-error");
        raizAST = $$;
    }
;

loopWhile:
    TokenWhile '(' expresion ')' sentenciaSimple {
        $$ = crearNodo(nodoLoopWhile, $3, $5, "while");
        raizAST = $$;
    }
    | TokenWhile error '{' bloqueCodigo '}' {
        yyerrok;
        $$ = crearNodo(nodoLoopWhile, NULL, $4, "while-error");
        raizAST = $$;
    }
;

loopDoWhile:
    TokenDo sentenciaSimple TokenWhile '(' expresion ')' ';' {
        $$ = crearNodo(nodoLoopDoWhile, $2, $5, "do-while");
        raizAST = $$;
    }
    | TokenDo sentenciaSimple TokenWhile '(' expresion ')' {
        yyerror("falta ';' al final del do-while");
        yyerrok;
        $$ = crearNodo(nodoLoopDoWhile, $2, $5, "do-while");
        raizAST = $$;
    }
;

loopFor:
    TokenFor '(' inicializacionFor condicional ';' condicional ')' sentenciaSimple {
        struct ASTNodo *condInc = crearNodo(nodoLoopFor, $4, $6, "for-cond-inc");
        struct ASTNodo *cabeceraFor = crearNodo(nodoLoopFor, $3, condInc, "for-cabecera");
        $$ = crearNodo(nodoLoopFor, cabeceraFor, $8, "for");
        raizAST = $$;
    }
    // For con paréntesis balanceados pero mal formado por dentro
    | TokenFor '(' error ')' sentenciaSimple {
        yyerrok;
        $$ = crearNodo(nodoLoopFor, NULL, $5, "for-error");
        raizAST = $$;
    }
    // For con paréntesis desbalanceados o totalmente corrupto
    | TokenFor error '{' bloqueCodigo '}' {
        yyerrok;
        $$ = crearNodo(nodoLoopFor, NULL, $4, "for-error");
        raizAST = $$;
    }
;

inicializacionFor:
    ';' { $$ = NULL; raizAST = $$; }
    | declaracionVariable { $$ = $1; raizAST = $$; }
    | expresion ';' { $$ = $1; raizAST = $$; }
;

condicional:
    { $$ = NULL; raizAST = $$; }
    | expresion { $$ = $1; raizAST = $$; }
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
    ;

listaCasos:
    caso { $$ = $1; raizAST = $$;}
    | listaCasos caso {
        $$ = crearNodo(nodoListaCasos, $1, $2, "lista-casos");
        raizAST = $$;
    }
;

caso:
    TokenCase elementoCase ':' bloqueCodigo {
        $$ = crearNodo(nodoCaso, $2, $4, "case");
        raizAST = $$;
    }
    | TokenCase elementoCase ':' bloqueCodigo TokenBreak ';' {
        struct ASTNodo *nodoBreak = crearNodo(nodoExpresion, NULL, NULL, "break");
        struct ASTNodo *cuerpoConBreak = crearNodo(nodoBloqueCodigo, $4, nodoBreak, "bloque-break");
        $$ = crearNodo(nodoCaso, $2, cuerpoConBreak, "case-break");
        raizAST = $$;
    }
;

elementoCase:
    TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.valor;
        raizAST = $$;
    }
    | TokenChar {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
;

listaParametros:
    { $$ = NULL; raizAST = $$; }
    | parametro { $$ = $1; raizAST = $$; }
    | listaParametros ',' parametro {
        $$ = crearNodo(nodoExpresion, $1, $3, "lista-parametros");
        raizAST = $$;
    }
;

parametro:
      especificadores TokenID {
        $$ = crearNodo(nodoExpresion, $1, NULL, $2.lexema);
        raizAST = $$;
    }
    | especificadores {
        $$ = crearNodo(nodoExpresion, $1, NULL, "parametro-tipo");
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            $$ = crearNodo(nodoExpresion, $1, ptrNodo, "parametro-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            $$ = crearNodo(nodoExpresion, $1, NULL, $3.lexema);
        }
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", NULL);
            $$ = crearNodo(nodoExpresion, $1, ptrNodo, "parametro-puntero-anon");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            $$ = crearNodo(nodoExpresion, $1, NULL, "parametro-error");
        }
        raizAST = $$;
    }
    | especificadores TokenID listaDimensiones {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        struct ASTNodo *arrNodo = crearNodo(nodoExpresion, idNodo, $3, "parametro-array");
        $$ = crearNodo(nodoExpresion, $1, arrNodo, "parametro");
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID listaDimensiones {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *arrNodo = crearNodo(nodoExpresion, ptrNodo, $4, "parametro-array");
            $$ = crearNodo(nodoExpresion, $1, arrNodo, "parametro-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración del parámetro");
            $$ = crearNodo(nodoExpresion, $1, NULL, $3.lexema);
        }
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
    especificadores ';' {
        $$ = crearNodo(nodoDeclaracionVariable, $1, NULL, "def-tipo");
        raizAST = $$;
    }
    |
      especificadores listaIds ';' {
        $$ = crearNodo(nodoDeclaracionVariable, $1, $2, "declaracion-multiple");
        raizAST = $$;
    }
    | declaracionArrays {
        $$ = $1;
        raizAST = $$;
    }
    | especificadores listaIds error {
        yyclearin;
        yyerrok;
        $$ = crearNodo(nodoDeclaracionVariable, $1, $2, "declaracion-multiple");
        raizAST = $$;
    }
    | TokenTypedef especificadores listaIds ';' {
        registrarTipoDeclarado($3);
        $$ = crearNodo(nodoDeclaracionVariable, $2, $3, "typedef");
        raizAST = $$;
    }
    | especificadores TokenID '=' '{' listaElementos '}' ';' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, $5, "=");
        raizAST = $$;
    }
    | especificadores ':' expresion ';' {
        $$ = crearNodo(nodoDeclaracionVariable, $1, $3, "bitfield-anon");
        raizAST = $$;
    }
;

listaIds:
      TokenID {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        raizAST = $$;
    }
    | TokenOpMultiplicativos listaIds {
        if (strcmp($1.lexema, "*") == 0) {
            struct ASTNodo *nodoPtr = crearNodoUnario("*", $2);
            $$ = crearNodo(nodoDeclaracionVariable, nodoPtr, NULL, "var-puntero");
        } else {
            yyerror("se esperaba '*' en la declaración de puntero");
            $$ = $2;
        }
        raizAST = $$;
    }
    | '(' TokenOpMultiplicativos TokenID ')' '(' listaParametros ')' {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $6, "puntero-funcion");
        } else {
            yyerror("se esperaba '*' en la declaración del puntero a función");
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            $$ = crearNodo(nodoDeclaracionVariable, idNodo, $6, "puntero-funcion");
        }
        raizAST = $$;
    }
    | TokenID '=' expresion {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $3, "=");
        $$ = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        raizAST = $$;
    }
    | listaIds ',' TokenID {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }
    | listaIds ',' TokenOpMultiplicativos TokenID {
        if (strcmp($3.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, NULL, "var-puntero");
            $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        } else {
            yyerror("se esperaba '*' en la declaración de puntero");
            $$ = $1;
        }
        raizAST = $$;
    }
    | listaIds ',' '(' TokenOpMultiplicativos TokenID ')' '(' listaParametros ')' {
        if (strcmp($4.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $5.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, $8, "puntero-funcion");
            $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        } else {
            yyerror("se esperaba '*' en la declaración del puntero a función");
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $5.lexema);
            struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, idNodo, $8, "puntero-funcion");
            $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        }
        raizAST = $$;
    }
    | listaIds ',' TokenID '=' expresion {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $5, "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }

    | TokenID ':' expresion {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, $3, "bitfield");
        raizAST = $$;
    }
    | listaIds ',' TokenID ':' expresion {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *nuevoBitfield = crearNodo(nodoDeclaracionVariable, idNodo, $5, "bitfield");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevoBitfield, "lista-ids");
        raizAST = $$;
    }
    | TokenID '=' '{' listaElementos '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $4, "=");
        $$ = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        raizAST = $$;
    }
    | listaIds ',' TokenID '=' '{' listaElementos '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $6, "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }
;

declaracionArrays:
    especificadores TokenID listaDimensiones ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, NULL, "matriz");
        raizAST = $$;
    }
    | especificadores TokenID listaDimensiones '=' '{' listaElementos '}' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, $6, "matriz-init");
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID '[' ']' '=' '{' listaElementos '}' ';' {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $8, "arr-puntero-init");
        } else {
            yyerror("se esperaba '*' en la declaración del arreglo de punteros");
            $$ = NULL;
        }
        raizAST = $$;
    }
    | especificadores TokenOpMultiplicativos TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';' {
        if (strcmp($2.lexema, "*") == 0) {
            struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
            $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $9, "arr-puntero-init");
        } else {
            yyerror("se esperaba '*' en la declaración del arreglo de punteros");
            $$ = NULL;
        }
        raizAST = $$;
    }
;

listaDimensiones:
    '[' TokenInt ']' {
        struct ASTNodo *tam = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, NULL, tam, "dimension");
    }
    | '[' ']' {
        $$ = crearNodo(nodoExpresion, NULL, NULL, "dimension");
    }
    | listaDimensiones '[' TokenInt ']' {
        struct ASTNodo *tam = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *nuevaDim = crearNodo(nodoExpresion, NULL, tam, "dimension");
        $$ = crearNodo(nodoExpresion, $1, nuevaDim, "lista-dimensiones");
    }
    | listaDimensiones '[' ']' {
        struct ASTNodo *nuevaDim = crearNodo(nodoExpresion, NULL, NULL, "dimension");
        $$ = crearNodo(nodoExpresion, $1, nuevaDim, "lista-dimensiones");
    }
;

especificadores:
      tipoCast { $$ = $1; }
    | TokenID {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod, tipoNodo, "mod-tipo");
        raizAST = $$;
    }
;

tipoCast:
    TokenUnion TokenID {
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, unNodo, NULL, "union");
        raizAST = $$;
    }
    | TokenModificador TokenUnion TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        $$ = crearNodo(nodoExpresion, mod, unionNodo, "mod-union");
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenUnion TokenID {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        $$ = crearNodo(nodoExpresion, mod2, unionNodo, "mod-union");
        raizAST = $$;
    }
    | TokenUnion TokenID '{' listaDeclaracionVariables '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-union");
        raizAST = $$;
    }
    | TokenUnion '{' listaDeclaracionVariables '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-union-anon");
        raizAST = $$;
    }
    |TokenTipoDato {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenNombreTipo {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenNombreTipo {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod, tipoNodo, "mod-tipo");
        raizAST = $$;
    }
    | TokenVoid {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenTipoDato {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod, NULL, $2.lexema);
        raizAST = $$;
    }
    | TokenModificador TokenVoid {
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

    | TokenStruct TokenID {
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        raizAST = $$;
    }
    | TokenModificador TokenStruct TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        $$ = crearNodo(nodoExpresion, mod, structNodo, "mod-struct");
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenStruct TokenID {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        $$ = crearNodo(nodoExpresion, mod2, structNodo, "mod-struct");
        raizAST = $$;
    }
    | TokenStruct TokenID '{' listaDeclaracionVariables '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-struct");
        raizAST = $$;
    }
    | TokenStruct '{' listaDeclaracionVariables '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-struct-anon");
        raizAST = $$;
    }

    | TokenEnum TokenID {
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        raizAST = $$;
    }
    | TokenEnum '{' listaIds '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-enum-anon");
        raizAST = $$;
    }
    | TokenModificador TokenEnum TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        $$ = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        raizAST = $$;
    }
    | TokenEnum TokenID '{' listaIds '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-enum");
        raizAST = $$;
    }
    | TokenModificador TokenEnum '{' listaIds '}' {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, NULL, $4, "def-enum-anon");
        $$ = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        raizAST = $$;
    }
;

elementoInit:
      TokenInt {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.valor;
    }
    | TokenFloat {
        $$ = crearNodo(nodoExpresion, NULL, NULL, NULL);
        $$->valorNumerico = $1.valor;
    }
    | TokenChar {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
    }
    | TokenString {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
    }
    | TokenID {
        /* Permite identificadores como NULL, SIMBOLO_FUNC, etc. */
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
    }
    | expresion {
        $$ = $1;
    }
;

listaElementos:
      elementoInit {
        $$ = $1;
    }
    | listaElementos ',' elementoInit {
        $$ = crearNodo(nodoExpresion, $1, $3, "lista-elementos");
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
    | '{' listaElementos '}' {
        $$ = crearNodo(nodoExpresion, $2, NULL, "bloque-matriz");
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
    | TokenString {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        raizAST = $$;
    }
    | '(' expresion ')' {
        $$ = crearNodo(nodoExpresion, $2, NULL, "()");
        raizAST = $$;
    }
    | expresion '[' expresion ']' {
        $$ = crearNodo(nodoExpresion, $1, $3, "indexacion");
        raizAST = $$;
    }
    | '(' tipoCast ')' expresion %prec UNARIO {
        $$ = crearNodo(nodoExpresion, $2, $4, "casteo");
        raizAST = $$;
    }
    | '(' tipoCast TokenOpMultiplicativos ')' expresion %prec UNARIO {
        if (strcmp($3.lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", $2);
            $$ = crearNodo(nodoExpresion, ptrNodo, $5, "casteo-puntero");
        } else {
            yyerror("se esperaba '*' en el casteo a puntero");
            $$ = crearNodo(nodoExpresion, $2, $5, "casteo");
        }
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
    /* Operadores unarios prefijo */
    | TokenOpAditivos expresion %prec UNARIO {
        $$ = crearNodoUnario($1.lexema, $2);
        raizAST = $$;
    }
    | '!' expresion %prec UNARIO {
        $$ = crearNodoUnario("!", $2);
        raizAST = $$;
    }
    | TokenOpBits expresion %prec UNARIO {
        if (strcmp($1.lexema, "&") != 0 && strcmp($1.lexema, "~") != 0) {
            char msg[96];
            snprintf(msg, sizeof(msg), "se esperaba una expresión antes de '%s'", $1.lexema);
            yyerror(msg);
        }
        $$ = crearNodoUnario($1.lexema, $2);
        raizAST = $$;
    }
    | TokenOpMultiplicativos expresion %prec UNARIO {
        if (strcmp($1.lexema, "*") != 0) {
            char msg[96];
            snprintf(msg, sizeof(msg), "se esperaba una expresión antes de '%s'", $1.lexema);
            yyerror(msg);
        }
        $$ = crearNodoUnario($1.lexema, $2);
        raizAST = $$;
    }
    | TokenOpInc expresion %prec UNARIO {
        $$ = crearNodoUnario($1.lexema, $2);
        raizAST = $$;
    }
    | TokenSizeof expresion %prec UNARIO {
        $$ = crearNodoUnario("sizeof ", $2);
        raizAST = $$;
    }
    | TokenSizeof '(' tipoCast ')' %prec UNARIO {
        $$ = crearNodo(nodoExpresion, $3, NULL, "sizeof-tipo");
        raizAST = $$;
    }
    | TokenSizeof '(' tipoCast TokenOpMultiplicativos ')' %prec UNARIO {
        if (strcmp($4.lexema, "*") == 0) {
            struct ASTNodo *ptrNodo = crearNodoUnario("*", $3);
            $$ = crearNodo(nodoExpresion, ptrNodo, NULL, "sizeof-puntero");
        } else {
            yyerror("se esperaba '*' en el sizeof del puntero");
            $$ = crearNodo(nodoExpresion, $3, NULL, "sizeof-tipo");
        }
        raizAST = $$;
    }
    /* Postfijos */
    | expresion TokenOpInc {
        $$ = crearNodo(nodoExpresion, $1, NULL, $2.lexema);
        raizAST = $$;
    }

    | '(' TokenTipoDato ')' expresion %prec UNARIO {
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, tipoNodo, $4, "casteo");
        raizAST = $$;
    }
    | '(' TokenModificador TokenTipoDato ')' expresion %prec UNARIO {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        struct ASTNodo *tipo = crearNodo(nodoExpresion, mod, NULL, $3.lexema);
        $$ = crearNodo(nodoExpresion, tipo, $5, "casteo");
        raizAST = $$;
    }
    | '(' TokenVoid TokenOpMultiplicativos ')' expresion %prec UNARIO {
        if (strcmp($3.lexema, "*") == 0) {
            struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", tipoNodo);
            $$ = crearNodo(nodoExpresion, ptrNodo, $5, "casteo-puntero");
        } else {
            yyerror("se esperaba '*' en el casteo de puntero");
            $$ = $5;
        }
        raizAST = $$;
    }
    | '(' TokenStruct TokenID TokenOpMultiplicativos ')' expresion %prec UNARIO {
        if (strcmp($4.lexema, "*") == 0) {
            struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
            struct ASTNodo *ptrNodo = crearNodoUnario("*", strNodo);
            $$ = crearNodo(nodoExpresion, ptrNodo, $6, "casteo-puntero-struct");
        } else {
            yyerror("se esperaba '*' en el casteo de struct");
            $$ = $6;
        }
        raizAST = $$;
    }

    /* Condicional ternario: cond ? a : b */
    | expresion '?' expresion ':' expresion {
        struct ASTNodo *ramas = crearNodo(nodoExpresion, $3, $5, ":");
        $$ = crearNodo(nodoExpresion, $1, ramas, "?");
        raizAST = $$;
    }
    | TokenID '(' listaArgumentosOpt ')' {
        $$ = crearNodo(nodoLlamadaFuncion, $3, NULL, $1.lexema);
        raizAST = $$;
    }
    | expresion TokenFlecha TokenID {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        $$ = crearNodo(nodoExpresion, $1, idNodo, "->");
        raizAST = $$;
    }
    | expresion '.' TokenID {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        $$ = crearNodo(nodoExpresion, $1, idNodo, ".");
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

/* Crea un nodo de operador unario prefijo */
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando) {
    char buf[64];
    snprintf(buf, sizeof(buf), "unario:%s", op);
    return crearNodo(nodoExpresion, operando, NULL, buf);
}

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


static void imprimirLineaErrorGCC(const char *archivo, int numLinea, int numColumna, int longitudToken) {
    FILE *f = fopen(archivo, "r");
    if (!f) return;

    char lineaBuffer[1024];
    int lineaActual = 1;

    while (fgets(lineaBuffer, sizeof(lineaBuffer), f)) {
        if (lineaActual == numLinea) {
            lineaBuffer[strcspn(lineaBuffer, "\r\n")] = 0;

            fprintf(stderr, " %5d | %s\n", numLinea, lineaBuffer);
            fprintf(stderr, "       | ");

            for (int i = 1; i < numColumna; i++) {
                if (lineaBuffer[i - 1] == '\t') {
                    fputc('\t', stderr);
                } else {
                    fputc(' ', stderr);
                }
            }

            fputc('^', stderr);
            for (int i = 1; i < longitudToken; i++) {
                fputc('~', stderr);
            }
            fputc('\n', stderr);

            break;
        }
        lineaActual++;
    }
    fclose(f);
}

// Función yyerror adaptada al formato nativo de GCC
void yyerror(const char *s) {
    totalErrores++;

    int columna = yylval.tokenInfo.columna > 0 ? yylval.tokenInfo.columna : columnaAct;
    int lenToken = yyleng > 0 ? yyleng : 1;

    const char *archivo = (archivoFuente && strlen(archivoFuente) > 0)
                          ? archivoFuente
                          : "fuente.c";

    fprintf(stderr, "%s:%d:%d: \033[1;31merror:\033[0m %s\n", archivo, lineaAct, columna, s);
    imprimirLineaErrorGCC(archivo, lineaAct, columna, lenToken);
}

int esTipoUsuario(char *s) {
    for (int i = 0; tiposEstandar[i]; i++){
        if (strcmp(s, tiposEstandar[i]) == 0) {
            return 1;
        }
    }
    for (int i = 0; i < totalTiposUsuario; i++) {
        if (strcmp(s, tiposUsuario[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void registrarTipo(char *s) {
    if (!s || esTipoUsuario(s) || totalTiposUsuario >= MaxTipos) {
        return;
    }
    tiposUsuario[totalTiposUsuario++] = strdup(s);
}

// Registra los nombres declarados en un typedef (la lista de ids)
void registrarTipoDeclarado(struct ASTNodo *n) {
    if (!n) {
        return;
    }
    if (n->valorLexema && strcmp(n->valorLexema, "lista-ids") == 0) {
        registrarTipoDeclarado(n->izq);
        registrarTipoDeclarado(n->der);
        return;
    }
    struct ASTNodo *h = n;
    while (h && h->izq) h = h->izq; // el nombre esta al fondo de la rama izquierda
    if (h && h->valorLexema) {
        registrarTipo(h->valorLexema);
    }
}