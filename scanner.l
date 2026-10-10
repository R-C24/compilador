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
"unsigned"|"signed"|"short"|"long"|"static"|"extern"|"const"|"volatile"|"auto"|"register" { return (crearToken(TokenModificador, 0)); }
"int"|"char"|"float"|"double"|"void" { return (crearToken(TokenTipoDato, 0)); }

    /* Palabras Reservadas */
"break"     { return (crearToken(TokenBreak, 0)); }
"continue"  { return (crearToken(TokenContinue, 0)); }
"enum"      { return (crearToken(TokenEnum, 0)); }
"goto"      { return (crearToken(TokenGoto, 0)); }
"sizeof"    { return (crearToken(TokenSizeof, 0)); }
"struct"    { return (crearToken(TokenStruct, 0)); }
"typedef"   { return (crearToken(TokenTypedef, 0)); }
"union"     { return (crearToken(TokenUnion, 0)); }
"for"       { return (crearToken(TokenFor, 0)); }
"if"        { return (crearToken(TokenIf, 0)); }
"else"      { return (crearToken(TokenElse, 0)); }
"switch"    { return (crearToken(TokenSwitch, 0)); }
"while"     { return (crearToken(TokenWhile, 0)); }
"do"        { return (crearToken(TokenDo, 0)); }
"case"      { return (crearToken(TokenCase, 0)); }
"return"    { return (crearToken(TokenReturn, 0)); }
"default"   { return (crearToken(TokenDefault, 0)); }

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

    /* Operadores */
"->" {return "->"; }
"?"  {return "?"; }

    /* Operadores de asignación */
"=" { return "="; }
"+="|"-="|"*="|"/="|"%="|"&="|"^="|"|="|"<<="|">>=" {return (crearToken(TokenOpAsignacion, 0));}

    /* Operadores lógicos */
"&&"|"||"|"!"   {return (crearToken(TokenOpLogico, 0));}

    /* Operadores de bits */
"|"|"^"|"~"|"&" {return (crearToken(TokenOpBits, 0));}

    /* Operadores de igualdad y relacionales */
"=="|"!="|"<="|">="|"<"|">"    {return (crearToken(TokenOpIgualdad, 0));}

    /* Operadores de desplazamiento */
"<<"|">>"  {return (crearToken(TokenOpDesplazamiento, 0));}

    /* Operadores de aditivos */
"+"|"-" {return (crearToken(TokenOpAditivos, 0));}

    /* Operadores de multiplicativos */
"%"|"*"|"/" {return (crearToken(TokenOpMultiplicativos, 0));}

    /* Operadores de incremento */
"++"|"--"| {return (crearToken(TokenOpInc, 0));}

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

    /* Separadores */
/* ";"|","|"."|"{"|"}"|"("|")"|"["|"]"|"#"|":" { return (crearToken(TokenSeparador, 0)); } */

";" { return ";"; }
"," { return ","; }
"." { return "."; }
"{" { return "{"; }
"}" { return "}"; }
"(" { return "("; }
")" { return ")"; }
"[" { return "["; }
"]" { return "]"; }
"#" { return "#"; }
":" { return ":"; }
"..." { return "..."; }


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