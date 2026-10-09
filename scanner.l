%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.tab.h"

int lineaAct = 1;
int columnaAct = 1;

#define YY_USER_ACTION { \
    columnaAct += yyleng; \
}


int crearToken(int codToken, double valor) {

    yylval.tokenInfo.lexema = strdup(yytext);
    yylval.tokenInfo.valor = valor;
    yylval.tokenInfo.linea = lineaAct;
    yylval.tokenInfo.columna = columnaAct - yyleng;

    return codToken;
}
%}

%option noyywrap

    /* Expresiones Regulares */
DIGITO        [0-9]
LETRA         [a-zA-Z_]
HEX           [0-9a-fA-F]

ID            {LETRA}({LETRA}|{DIGITO})*
INT           {DIGITO}+[uU]?[lL]{0,2}|0[xX]{HEX}+[uU]?[lL]{0,2}

FLOAT         ({DIGITO}+\.{DIGITO}*|\.{DIGITO}+|{DIGITO}+)[eE][+-]?{DIGITO}+[fFlL]?|{DIGITO}+\.{DIGITO}*[fFlL]?|\.{DIGITO}+[fFlL]?
STRING        \"([^\"\n\\]|\\.)*\"

LINEACOMENTARIO  "//".*
BLOQUECOMENTARIO "/*"([^*]|\*+[^*/])*\*+"/"

%%

\n  { lineaAct++; columnaAct = 1; }
[ \t\r]+

{LINEACOMENTARIO}   {  }
{BLOQUECOMENTARIO}  {
                    for(int i = 0; yytext[i] != '\0'; i++) {
                        if(yytext[i] == '\n') {
                            lineaAct++;
                            columnaAct = 1;
                        }
                    }
                 }
    /* Capturar un comentario de bloque que nunca se cierra */
"/*"([^*]|\*+[^*/])* { return ( crearToken(TokenError, 0)); }

    /* Literales numéricos válidos */
{INT}     { return crearToken(TokenInt, (double)strtol(yytext, NULL, 0));}
{FLOAT}   { return crearToken(TokenFloat, atof(yytext));}
{STRING}  { return crearToken(TokenString, 0);}

    /* Modificadores y Tipos de Dato */
"unsigned"|"signed"|"short"|"long" { return (crearToken(TokenModificador, 0)); }
"int"|"char"|"float"|"double"|"void" { return (crearToken(TokenTipoDato, 0)); }

    /* Palabras Reservadas */
"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"else"|"enum"|"extern"|"for"|"goto"|"if"|"register"|"return"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"volatile"|"while"  { return (crearToken(TokenPalabraReservada, 0)); }


    /* Patrones de errores léxicos */

    /* 1. Capturar identificadores mal formados */
{DIGITO}+{LETRA}({LETRA}|{DIGITO})* { return (crearToken(TokenError, 0)); }

    /* 2. Capturar hexadecimales corruptos con letras fuera de A-F */
0[xX]{HEX}*[g-zG-Z_]+({LETRA}|{DIGITO})* { return (crearToken(TokenError, 0)); }

    /* 3. Capturar flotantes con múltiples puntos decimales */
{DIGITO}+\.{DIGITO}*\.{DIGITO}*(\.{DIGITO}*)* { return (crearToken(TokenError, 0)); }

    /* 4. Capturar exponente con letras/caracteres inválidos */
({DIGITO}+\.{DIGITO}*|\.{DIGITO}+|{DIGITO}+)[eE][+-]?{DIGITO}*{LETRA}({LETRA}|{DIGITO})* { return (crearToken(TokenError, 0)); }

    /* 5. Capturar exponente inconcluso seguido de operadores */
({DIGITO}+\.{DIGITO}*|\.{DIGITO}+|{DIGITO}+)[eE][+-]?[+\-*/%&|^~!=<>]+ { return (crearToken(TokenError, 0)); }

    /* 6. Capturar exponente inconcluso al final o antes de separadores */
({DIGITO}+\.{DIGITO}*|\.{DIGITO}+|{DIGITO}+)[eE][+-]? { return crearToken(TokenError, 0); }

    /* 7. Capturar secuencia de 3 o más símbolos de operador */
[+\-*/%&|^~!=<>?:]{3,} {return (crearToken(TokenError, 0));}


    /* Tokens válidos */

    /* Operadores válidos de 3 caracteres */
"<<="|">>="|"\.\.\." {return (crearToken(TokenOp, 0));}

    /* Operadores */
"=="|"!="|"<="|">="|"&&"|"||"|"<<"|">>"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"|="|"->"|"+"|"-"|"*"|"/"|"%"|"="|"<"|">"|"!"|"&"|"|"|"^"|"~"|"?" {return (  crearToken(TokenOp, 0)); }

    /* Capturar un literal de carácter vacío */
\'\' { return (crearToken(TokenError, 0)); }

    /* Reconocer un literal de carácter válido en C */
\'(\\.|[\x80-\xFF]|[^\\'\n])\' { return (crearToken(TokenChar, (double)yytext[1]));}

    /* Capturar literales de carácter con múltiples elementos */
\'([^'\n\\]|\\.)+\' { return (crearToken(TokenError, 0)); }

    /* Capturar una cadena de texto sin cerrar */
\"([^\"\n\\]|\\.)* { return (crearToken(TokenError, 0)); }

    /* Identificadores */
{ID} { return (crearToken(TokenID, 0)); }

    /* Delimitadores */
";"|","|"."|"{"|"}"|"("|")"|"["|"]"|"#"|":" { return (crearToken(TokenSeparador, 0)); }

    /* Caracteres fuera de ASCII */
[\x80-\xFF]+ { return (  crearToken(TokenError, 0)); }

.  { return (crearToken(TokenError, 0)); }

%%
/*
Token getToken() {
    int token = yylex();
    if (token == 0) {
        Token eofToken;
        eofToken.tipo = TokenEOF;
        eofToken.lexema = "EOF";
        eofToken.valor = 0;
        eofToken.numLinea = lineaAct;
        eofToken.numColumna = columnaAct;
        return eofToken;
    }
    return tokenAct;
}
*/