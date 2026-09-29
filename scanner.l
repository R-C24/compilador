%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "compilador.h"

int lineaAct = 1;
int columnaAct = 1;

#define YY_USER_ACTION { \
    columnaAct += yyleng; \
}

Token tokenAct;


Token crearToken(TipoToken tipo, double valor) {
    Token token;
    token.tipo = tipo;
    token.lexema = strdup(yytext);
    token.valor = valor;
    token.numLinea = lineaAct;
    token.numColumna = columnaAct - yyleng;
    return token;
}
%}

%option noyywrap

    /* Expresiones Regulares*/
DIGITO        [0-9]
LETRA         [a-zA-Z_]
HEX           [0-9a-fA-F]
ID            {LETRA}({LETRA}|{DIGITO})*
INT           {DIGITO}+[uU]?[lL]{0,2}|0[xX]{HEX}+[uU]?[lL]{0,2}
FLOAT         ({DIGITO}+\.{DIGITO}*|\.{DIGITO}+)([eE][+-]?{DIGITO}+)?[fFlL]?|{DIGITO}+[eE][+-]?{DIGITO}+[fFlL]?
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

"/*"([^*]|\*+[^*/])* { return (tokenAct = crearToken(TokenError, 0)).tipo; }

    /* 1. Capturar identificadores mal formados que inician con números */
{DIGITO}+{LETRA}({LETRA}|{DIGITO})* { return (tokenAct = crearToken(TokenError, 0)).tipo; }

    /* 2. Capturar Hexadecimales corruptos con letras fuera de A-F */
0[xX]{HEX}*[g-zG-Z_]+({LETRA}|{DIGITO})* { return (tokenAct = crearToken(TokenError, 0)).tipo; }

    /* 3. Capturar Flotantes con múltiples puntos decimales */
{DIGITO}+\.{DIGITO}*\.{DIGITO}*(\.{DIGITO}*)* { return (tokenAct = crearToken(TokenError, 0)).tipo; }

={3,} { return (tokenAct = crearToken(TokenError, 0)).tipo; }

    /* Modificadores*/
"unsigned"|"signed"|"short"|"long" { return (tokenAct = crearToken(TokenModificador, 0)).tipo; }

    /* Tipo de Dato*/
"int"|"char"|"float"|"double"|"void" { return (tokenAct = crearToken(TokenTipoDato, 0)).tipo; }

    /* Palabras Reservadas*/
"auto"|"break"|"case"|"const"|"continue"|"default"|"do"|"else"|"enum"|"extern"|"for"|"goto"|"if"|"register"|"return"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"volatile"|"while"  { return (tokenAct = crearToken(TokenPalabraReservada, 0)).tipo; }

    /* Literales */
{INT}     { return (tokenAct = crearToken(TokenInt, (double)strtol(yytext, NULL, 0))).tipo;}
{FLOAT}   { return (tokenAct = crearToken(TokenFloat, atof(yytext))).tipo;}
{STRING}  { return (tokenAct = crearToken(TokenString, 0)).tipo;}

\'\' { return (tokenAct = crearToken(TokenError, 0)).tipo; }

\'(\\.|[\x80-\xFF]|[^\\'\n])\' { return (tokenAct = crearToken(TokenChar, (double)yytext[1])).tipo;}

\'([^'\n\\]|\\.)+\' { return (tokenAct = crearToken(TokenError, 0)).tipo; }

\"([^\"\n\\]|\\.)* { return (tokenAct = crearToken(TokenError, 0)).tipo; }

    /* Identificadores*/
{ID} { return (tokenAct = crearToken(TokenID, 0)).tipo; }

    /* Operadores*/
"<<="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"<<"|">>"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"\|="|"->"|"+"|"-"|"*"|"/"|"%"|"="|"<"|">"|"!"|"&"|"\|"|"^"|"~"|"?"  { return (tokenAct = crearToken(TokenOp, 0)).tipo; }

    /* Delimitadores*/
";"|","|"."|"{"|"}"|"("|")"|"["|"]"|"#"|":" { return (tokenAct = crearToken(TokenSeparador, 0)).tipo; }

    /* Caracteres fuera de ASCII (Tildes, ñ, UTF-8 en nombres de variables o símbolos) */
[\x80-\xFF]+ { return (tokenAct = crearToken(TokenError, 0)).tipo; }

.  { return (tokenAct = crearToken(TokenError, 0)).tipo; }

%%

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