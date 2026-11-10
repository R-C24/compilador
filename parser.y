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

%type <nodo> tipoCast programa funcion retornar bloqueCodigo sentenciaBloque sentenciaSimple condicionIf loopWhile loopDoWhile loopFor inicializacionFor condicional elementoCase caseSwitch listaCasos caso listaParametros parametro listaDeclaracionVariables declaracionVariable declaracionArrays listaDimensiones listaElementos elemento listaModificadores expresion listaArgumentosOpt listaArgumentos especificadores listaIds elementoInit abrirAmbito cerrarAmbito cuerpoEstructura sentenciaSola

%right '=' TokenOpAsignacion
%right '?' ':'
%left TokenOpLogico
%left TokenOpBits
%left TokenOpIgualdad TokenOpDesplazamiento
%left TokenOpAditivos
%left TokenOpMultiplicativos '*'
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
    | especificadores '*' TokenID '(' listaParametros ')' ';' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $5, $3.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, NULL, $3.lexema);
        raizAST = $$;
    }
    | especificadores TokenID '(' listaParametros ')' '{' {
        strncpy(funcionActual, $2.lexema, 255);
        contextoImpreso = 0;
    } abrirAmbito bloqueCodigo cerrarAmbito '}' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $4, $2.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $9, $2.lexema);
        raizAST = $$;
        funcionActual[0] = '\0';
    }
    | especificadores '*' TokenID '(' listaParametros ')' '{' {
        strncpy(funcionActual, $3.lexema, 255);
        contextoImpreso = 0;
    } abrirAmbito bloqueCodigo cerrarAmbito '}' {
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $5, $3.lexema);
        $$ = crearNodo(nodoFuncion, cabecera, $10, $3.lexema);
        raizAST = $$;
        funcionActual[0] = '\0';
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
    | TokenReturn expresion { 
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
      sentenciaSola { 
          $$ = $1; 
          raizAST = $$; 
      }
    | '{' abrirAmbito bloqueCodigo cerrarAmbito '}' { 
        $$ = $3; 
        raizAST = $$; 
    }
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

condicionIf:
    TokenIf '(' expresion ')' cuerpoEstructura {
        $$ = crearNodo(nodoCondicionIf, $3, $5, "if");
        raizAST = $$;
    }
    | TokenIf '(' expresion ')' cuerpoEstructura TokenElse cuerpoEstructura {
        struct ASTNodo *ifParcial = crearNodo(nodoCondicionIf, $3, $5, "if");
        $$ = crearNodo(nodoCondicionIf, ifParcial, $7, "if-else");
        raizAST = $$;
    }
    | TokenIf '(' expresion '{' abrirAmbito bloqueCodigo cerrarAmbito '}' {
        yyerror("se esperaba ')' antes del token '{'");
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, $3, $5, "if-error");
        raizAST = $$;
    }
    | TokenIf '(' expresion cuerpoEstructura {
        yyerror("se esperaba ')' después de la condición del 'if'");
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, $3, $4, "if-error");
        raizAST = $$;
    }
    | TokenIf error '{' abrirAmbito bloqueCodigo cerrarAmbito '}' {
        yyerrok;
        $$ = crearNodo(nodoCondicionIf, NULL, $4, "if-error");
        raizAST = $$;
    }
;

loopWhile:
    TokenWhile '(' expresion ')' cuerpoEstructura {
        $$ = crearNodo(nodoLoopWhile, $3, $5, "while");
        raizAST = $$;
    }
    | TokenWhile error '{' abrirAmbito bloqueCodigo cerrarAmbito '}' {
        yyerrok;
        $$ = crearNodo(nodoLoopWhile, NULL, $4, "while-error");
        raizAST = $$;
    }
;

loopDoWhile:
    TokenDo cuerpoEstructura TokenWhile '(' expresion ')' ';' {
        $$ = crearNodo(nodoLoopDoWhile, $2, $5, "do-while");
        raizAST = $$;
    }
    | TokenDo cuerpoEstructura TokenWhile '(' expresion ')' {
        yyerror("falta ';' al final del do-while");
        yyerrok;
        $$ = crearNodo(nodoLoopDoWhile, $2, $5, "do-while");
        raizAST = $$;
    }
;

loopFor:
    TokenFor '(' inicializacionFor condicional ';' condicional ')' cuerpoEstructura {
        struct ASTNodo *condInc = crearNodo(nodoLoopFor, $4, $6, "for-cond-inc");
        struct ASTNodo *cabeceraFor = crearNodo(nodoLoopFor, $3, condInc, "for-cabecera");
        $$ = crearNodo(nodoLoopFor, cabeceraFor, $8, "for");
        raizAST = $$;
    }
    // For con paréntesis balanceados pero mal formado por dentro
    | TokenFor '(' error ')' cuerpoEstructura {
        yyerrok;
        $$ = crearNodo(nodoLoopFor, NULL, $5, "for-error");
        raizAST = $$;
    }
    // For con paréntesis desbalanceados o totalmente corrupto
    | TokenFor error '{' abrirAmbito bloqueCodigo cerrarAmbito '}' {
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
    TokenSwitch '(' expresion ')' '{' abrirAmbito listaCasos cerrarAmbito '}' {
        $$ = crearNodo(nodoCaseSwitch, $3, $6, "switch");
        raizAST = $$;
    }
    | TokenSwitch '(' expresion ')' '{' abrirAmbito listaCasos TokenDefault ':' bloqueCodigo cerrarAmbito '}' {
        struct ASTNodo *nodoDef = crearNodo(nodoCaso, NULL, $10, "default");
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
    | especificadores '*' TokenID {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        $$ = crearNodo(nodoExpresion, $1, ptrNodo, "parametro-puntero");
     
        raizAST = $$;
    }
    | especificadores '*' {
       struct ASTNodo *ptrNodo = crearNodoUnario("*", NULL);
        $$ = crearNodo(nodoExpresion, $1, ptrNodo, "parametro-puntero-anon");
    
        raizAST = $$;
    }
    | especificadores TokenID listaDimensiones {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        struct ASTNodo *arrNodo = crearNodo(nodoExpresion, idNodo, $3, "parametro-array");
        $$ = crearNodo(nodoExpresion, $1, arrNodo, "parametro");
        raizAST = $$;
    }
    | especificadores '*' TokenID listaDimensiones {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        struct ASTNodo *arrNodo = crearNodo(nodoExpresion, ptrNodo, $4, "parametro-array");
        $$ = crearNodo(nodoExpresion, $1, arrNodo, "parametro-puntero");
    
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
        validarYRegistrarSimbolo($2.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
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
        validarYRegistrarSimbolo($1.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        raizAST = $$;
    }
    | '*' listaIds {
        struct ASTNodo *nodoPtr = crearNodoUnario("*", $2);
        $$ = crearNodo(nodoDeclaracionVariable, nodoPtr, NULL, "var-puntero");
    
        raizAST = $$;
    }
    | '(' '*' TokenID ')' '(' listaParametros ')' {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $6, "puntero-funcion");
    
         raizAST = $$;
     }
    | TokenID '=' expresion {
        validarYRegistrarSimbolo($1.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $3, "=");
        $$ = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        raizAST = $$;
    }
    | listaIds ',' TokenID {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, idNodo, NULL, "var-simple");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }
    | listaIds ',' '*' TokenID {
        validarYRegistrarSimbolo($4.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, NULL, "var-puntero");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
    
        raizAST = $$;
    }
    | listaIds ',' '(' '*' TokenID ')' '(' listaParametros ')' {
        validarYRegistrarSimbolo($5.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $5.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, ptrNodo, $8, "puntero-funcion");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
    
        raizAST = $$;
    }
    | listaIds ',' TokenID '=' expresion {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $5, "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }

    | TokenID ':' expresion {
        validarYRegistrarSimbolo($1.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, idNodo, $3, "bitfield");
        raizAST = $$;
    }
    | listaIds ',' TokenID ':' expresion {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *nuevoBitfield = crearNodo(nodoDeclaracionVariable, idNodo, $5, "bitfield");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevoBitfield, "lista-ids");
        raizAST = $$;
    }
    | TokenID '=' '{' listaElementos '}' {
        validarYRegistrarSimbolo($1.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $4, "=");
        $$ = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        raizAST = $$;
    }
    | listaIds ',' TokenID '=' '{' listaElementos '}' {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *asig = crearNodo(nodoExpresion, idNodo, $6, "=");
        struct ASTNodo *nuevaVar = crearNodo(nodoDeclaracionVariable, asig, NULL, "var-init-bloque");
        $$ = crearNodo(nodoDeclaracionVariable, $1, nuevaVar, "lista-ids");
        raizAST = $$;
    }
;

declaracionArrays:
    especificadores TokenID listaDimensiones ';' {
        validarYRegistrarSimbolo($2.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, NULL, "matriz");
        raizAST = $$;
    }
    | especificadores TokenID listaDimensiones '=' '{' listaElementos '}' ';' {
        validarYRegistrarSimbolo($2.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *cabecera = crearNodo(nodoExpresion, $1, $3, $2.lexema);
        $$ = crearNodo(nodoDeclaracionVariable, cabecera, $6, "matriz-init");
        raizAST = $$;
    }
    | especificadores '*' TokenID '[' ']' '=' '{' listaElementos '}' ';' {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $8, "arr-puntero-init");
    
        raizAST = $$;
    }
    | especificadores '*' TokenID '[' TokenInt ']' '=' '{' listaElementos '}' ';' {
        validarYRegistrarSimbolo($3.lexema, tipoActual, tipoVariable, lineaAct, columnaAct);
        
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", idNodo);
        $$ = crearNodo(nodoDeclaracionVariable, ptrNodo, $9, "arr-puntero-init");
    
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
;

tipoCast:
    TokenUnion TokenID {
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, unNodo, NULL, "union");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenUnion TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        $$ = crearNodo(nodoExpresion, mod, unionNodo, "mod-union");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenUnion TokenID {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *unNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *unionNodo = crearNodo(nodoExpresion, unNodo, NULL, "union");
        $$ = crearNodo(nodoExpresion, mod2, unionNodo, "mod-union");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenUnion TokenID '{' abrirAmbito listaDeclaracionVariables cerrarAmbito '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-union");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenUnion '{' abrirAmbito listaDeclaracionVariables cerrarAmbito '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-union-anon");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenTipoDato {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenNombreTipo {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }

    | TokenModificador TokenNombreTipo {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod, tipoNodo, "mod-tipo");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenVoid {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenTipoDato {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod, NULL, $2.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenVoid {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod, NULL, $2.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador {
        $$ = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenTipoDato {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenModificador TokenTipoDato {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *mod3 = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        $$ = crearNodo(nodoExpresion, mod3, NULL, $4.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenModificador {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, mod2, NULL, $3.lexema);
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }

    | TokenStruct TokenID {
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenStruct TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        $$ = crearNodo(nodoExpresion, mod, structNodo, "mod-struct");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenModificador TokenStruct TokenID {
        struct ASTNodo *mod1 = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *mod2 = crearNodo(nodoExpresion, mod1, NULL, $2.lexema);
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $4.lexema);
        struct ASTNodo *structNodo = crearNodo(nodoExpresion, strNodo, NULL, "struct");
        $$ = crearNodo(nodoExpresion, mod2, structNodo, "mod-struct");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenStruct TokenID '{' abrirAmbito listaDeclaracionVariables cerrarAmbito '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-struct");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenStruct '{' abrirAmbito listaDeclaracionVariables cerrarAmbito '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-struct-anon");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }

    | TokenEnum TokenID {
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenEnum '{' abrirAmbito listaIds cerrarAmbito '}' {
        $$ = crearNodo(nodoExpresion, NULL, $3, "def-enum-anon");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenEnum TokenID {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *enNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, enNodo, NULL, "enum");
        $$ = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenEnum TokenID '{' abrirAmbito listaIds cerrarAmbito '}' {
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $4, "def-enum");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
        raizAST = $$;
    }
    | TokenModificador TokenEnum '{' abrirAmbito listaIds cerrarAmbito '}' {
        struct ASTNodo *mod = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        struct ASTNodo *enumNodo = crearNodo(nodoExpresion, NULL, $4, "def-enum-anon");
        $$ = crearNodo(nodoExpresion, mod, enumNodo, "mod-enum");
        obtenerTextoTipo($$, tipoActual, sizeof(tipoActual));
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
        validarUsoSimbolo($1.lexema, lineaAct, columnaAct);
        
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
    | '(' tipoCast '*' ')' expresion %prec UNARIO {
        struct ASTNodo *ptrNodo = crearNodoUnario("*", $2);
        $$ = crearNodo(nodoExpresion, ptrNodo, $5, "casteo-puntero");
    
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
    | expresion '*' expresion {
        $$ = crearNodo(nodoExpresion, $1, $3, "*");
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
    | '*' expresion %prec UNARIO {
        $$ = crearNodoUnario("*", $2);
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
    | TokenSizeof '(' tipoCast '*' ')' %prec UNARIO {
        struct ASTNodo *ptrNodo = crearNodoUnario("*", $3);
        $$ = crearNodo(nodoExpresion, ptrNodo, NULL, "sizeof-puntero");
    
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
    | '(' TokenVoid '*' ')' expresion %prec UNARIO {
        struct ASTNodo *tipoNodo = crearNodo(nodoExpresion, NULL, NULL, $2.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", tipoNodo);
        $$ = crearNodo(nodoExpresion, ptrNodo, $5, "casteo-puntero");
    
        raizAST = $$;
    }
    | '(' TokenStruct TokenID '*' ')' expresion %prec UNARIO {
        struct ASTNodo *strNodo = crearNodo(nodoExpresion, NULL, NULL, $3.lexema);
        struct ASTNodo *ptrNodo = crearNodoUnario("*", strNodo);
        $$ = crearNodo(nodoExpresion, ptrNodo, $6, "casteo-puntero-struct");
    
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

abrirAmbito:
    /* vacío */ { pushAmbito(); }
;

cerrarAmbito:
    /* vacío */ { 
        popAmbito(); 
    }
;

sentenciaSola:
    expresion ';'        { $$ = $1; raizAST = $$; }
    | declaracionVariable  { $$ = $1; raizAST = $$; }
    | condicionIf          { $$ = $1; raizAST = $$; }
    | caseSwitch           { $$ = $1; raizAST = $$; }
    | loopWhile            { $$ = $1; raizAST = $$; }
    | loopFor              { $$ = $1; raizAST = $$; }
    | loopDoWhile          { $$ = $1; raizAST = $$; }
    | retornar             { $$ = $1; raizAST = $$; }
    | TokenBreak ';'       { $$ = crearNodo(nodoExpresion, NULL, NULL, "break"); raizAST = $$; }
    | TokenContinue ';'    { $$ = crearNodo(nodoExpresion, NULL, NULL, "continue"); raizAST = $$; }
    | TokenGoto TokenID ';' { $$ = crearNodo(nodoExpresion, NULL, NULL, "goto"); raizAST = $$; }
    | TokenID ':' sentenciaSola { 
        struct ASTNodo *idNodo = crearNodo(nodoExpresion, NULL, NULL, $1.lexema);
        $$ = crearNodo(nodoExpresion, idNodo, $3, "etiqueta");
        raizAST = $$;
    }
    | TokenElse sentenciaSola {
        yyerror("'else' sin un 'if' previo");
        yyerrok;
        $$ = $2;
        raizAST = $$;
    }
    | ';'                  { $$ = NULL; }
;

cuerpoEstructura:
      /* Caso A: El usuario puso llaves explícitamente */
      '{' abrirAmbito bloqueCodigo cerrarAmbito '}' { 
          $$ = $3; 
          raizAST = $$; 
      }
      /* Caso B: Sentencia huérfana. Se apila un ámbito antes y se desapila al terminar la instrucción */
    | abrirAmbito sentenciaSola cerrarAmbito { 
          $$ = $2; 
          raizAST = $$; 
      }
;

%%

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
