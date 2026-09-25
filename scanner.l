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
    t.tipo = tipo;
    t.lexema = strdup(yytext);
    t.valor = valor;
    t.linea = lineaAct;
    t.columna = columnaAct - yyleng;
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
CHAR          \'([^\'\n\\]|\\.)*\'

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

/* Palabras Reservadas*/
"auto"|"break"|"case"|"char"|"const"|"continue"|"default"|"do"|"double"|"else"|"enum"|"extern"|"float"|"for"|"goto"|"if"|"int"|"long"|"register"|"return"|"short"|"signed"|"sizeof"|"static"|"struct"|"switch"|"typedef"|"union"|"unsigned"|"void"|"volatile"|"while"  { return (tokenAct = crearToken(TokenPalabraReservada, 0)).tipo; }

/* Literales */
{INT}     { return (tokenAct = crearToken(TokenInt, (double)strtol(yytext, NULL, 0))).tipo; }
{FLOAT}   { return (tokenAct = crearToken(TokenFloat, atof(yytext))).tipo; }
{STRING}  { return (tokenAct = crearToken(TokenString, 0)).tipo; }
{CHAR}    { return (tokenAct = crearToken(TokenChar, (double)yytext[1])).tipo; }

/* Identificadores */
{ID} { return (tokenAct = crearToken(TokenID, 0)).tipo; }

/* Operadores*/
"<<"="|">>="|"=="|"!="|"<="|">="|"&&"|"||"|"<<"|">>"|"++"|"--"|"+="|"-="|"*="|"/="|"%="|"&="|"^="|"\x7c="|"->"|"+"|"-"|"*"|"/"|"%"|"="|"<"|">"|"!"|"&"|"\x7c"|"^"|"~"|"?"  { return (tokenAct = crearToken(TokenOp, 0)).tipo; }


/* Delimitadores */
";"|","|"."|"{"|"}"|"("|")"|"["|"]" { return (tokenAct = crearToken(TokenSeparador, 0)).tipo; }

.  { return (tokenAct = crearToken(TokenError, 0)).tipo; }

%%

Token getToken() {
    int token = yylex();
    if (token == 0) {
        Token eofToken;
        eofToken.tipo = TokenEOF;
        eofToken.lexema = "EOF";
        eofToken.valor = 0;
        eofToken.linea = lineaAct;
        eofToken.columna = columnaAct;
        return eofToken;
    }
    return tokenAct;
}