#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "compilador.h"
#include "beamer.h"


Macro macros[MaxMacros];
int totalMacros = 0;

extern FILE *yyin;
extern void yyrestart(FILE *new_file);
extern int yyparse();

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 0; // El preproceso está desactivado por defecto.
    char *archivoFuente = NULL;
    char *archivoSalida = "salidaPresentacion.tex";

    //REVISAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAR
    while ((opt = getopt(argc, argv, "po:")) != -1) {
        switch (opt) {
            case 'p':
                activarPreproceso = 1;
            break;
            case 'o':
                archivoSalida = optarg;
            break;
            default:
                printf("Error: No es una opción válida. Utilice '-p' o '-o nombreArchivo'. \n");
            return EXIT_SUCCESS;
        }
    }

    if (optind < argc) {
        archivoFuente = argv[optind];
    } else {
        fprintf(stderr, "Error: Falta el archivo fuente .c de entrada.\n");
        return EXIT_FAILURE;
    }

    if (access(archivoFuente, F_OK) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe o no tiene acceso.\n", archivoFuente);
        return EXIT_FAILURE;
    }

    char *archivoTemporal = "archivoTemp.tmp";

    if (activarPreproceso) {
        printf("Ejecutando Preprocesador...\n");

        FILE *f_temp = fopen(archivoTemporal, "w");
        if (!f_temp) {
            perror("Error al crear el archivo temporal");
            exit(EXIT_FAILURE);
        }

        procesarArchivo(archivoFuente, f_temp);
        fclose(f_temp);

    } else {
        printf("Preprocesador desactivado.\n");
    }

    printf("Pasando archivo procesado al Scanner (Flex)...\n");
    yyin = fopen(archivoTemporal, "r");
    if (!yyin) {
        perror("Error al abrir el archivo temporal");
        return EXIT_FAILURE;
    }

    printf("Iniciando análisis sintáctico con Bison...\n");
    int resultado = yyparse();

    /*TokenStats stats = {0};
    ErrorLexico errores[MaxErrores];
    int cantErrores = 0;

    Token token;
    printf("\n Inicio de tokens\n");
    do {
        token = getToken();

        if (token.tipo != TokenEOF) {
            printf("Token: %-15s | Lexema: %-15s | Valor: %-5.2f | Línea: %d | Columna: %d\n",
                   obtenerNombreToken(token.tipo),
                   token.lexema,
                   token.valor,
                   token.numLinea,
                   token.numColumna);

            switch (token.tipo) {
                case TokenPalabraReservada:
                    stats.palabrasReservadas++;
                    break;
                case TokenModificador:
                    stats.palabrasReservadas++;
                    break;
                case TokenTipoDato:
                    stats.palabrasReservadas++;
                    break;
                case TokenID:
                    stats.identificadores++;
                    break;
                case TokenInt:
                    stats.num++;
                    break;
                case TokenFloat:
                    stats.num++;
                    break;
                case TokenOp:
                    stats.operadores++;
                    break;
                case TokenSeparador:
                    stats.separadores++;
                    break;
                case TokenError:
                    stats.erroresLexicos++;
                    if (cantErrores < MaxErrores) {
                        errores[cantErrores].linea = token.numLinea;
                        strncpy(errores[cantErrores].lexema,
                            token.lexema ? token.lexema : "",
                            sizeof(errores[cantErrores].lexema) - 1);
                        snprintf(errores[cantErrores].descripcion,
                             sizeof(errores[cantErrores].descripcion),
                             "Carácter o token no reconocido");
                        cantErrores++;
                    }
                    break;
                default:
                    break;
            }
            
            if (token.lexema && strcmp(token.lexema, "EOF") != 0) {
                free(token.lexema);
            }
        }
    } while (token.tipo != TokenEOF); */

    //rewind(yyin);
    //yyrestart(yyin);

    //char *nombreBeamer = archivoSalida ? archivoSalida : "salidaPresentacion.tex";
    //generarBeamer(nombreBeamer, &stats, errores, cantErrores);

    fclose(yyin);

    /*char archivoPDF[256];
    strncpy(archivoPDF, nombreBeamer, sizeof(archivoPDF) - 1);
    archivoPDF[sizeof(archivoPDF) - 1] = '\0';

    char *ext = strrchr(archivoPDF, '.');
    if (ext && strcmp(ext, ".tex") == 0) {
        strcpy(ext, ".pdf");
    } else {
        strcat(archivoPDF, ".pdf");
    }

    if (compilarTexPdf(nombreBeamer)) {
        desplegarPdf(archivoPDF);
    }*/

    printf("Proceso del Proyecto 1 finalizado.\n");
    return EXIT_SUCCESS;
}

// Agrega la información de un nuevo macro al "diccionario" de macros.
void agregarMacro(char *nombre, char *valor) {
    if (totalMacros < MaxMacros) {
        strncpy(macros[totalMacros].nombre, nombre, MaxNombre);
        strncpy(macros[totalMacros].valor, valor, MaxValor);
        totalMacros++;
    }
}

// Verifica si un carácter puede ser parte de un identificador
int esCaracterIdentificador(char c){
    return isalnum((unsigned char)c) || c == '_';
}

// Verifica si un carácter en una posición está escapado ('\')
int esEscapado(char *str, int pos){
    int slashes = 0;
    while (pos > 0 && str[pos - 1] == '\\') {
        slashes++;
        pos--;
    }
    return (slashes % 2 != 0);
}

// Verifica que la macro sea válida; es decir, que no esté en dentro de comentarios o sea subcadena.
char *buscarMacroValida(char *linea, char *nombre){
    int len = strlen(nombre);
    int enCadena = 0;
    int enChar = 0;
    int enComentarioBloque = 0;

    for (int i = 0; linea[i] != '\0'; i++) {
        // Omite comentarios de línea '//'
        if (!enCadena && !enChar && !enComentarioBloque && linea[i] == '/' && linea[i + 1] == '/') {
            break;
        }

        // Omite comentarios de bloque '/*' ... '*/'
        if (!enCadena && !enChar && !enComentarioBloque && linea[i] == '/' && linea[i + 1] == '*') {
            enComentarioBloque = 1;
            i++;
            continue;
        }
        if (enComentarioBloque) {
            if (linea[i] == '*' && linea[i + 1] == '/') {
                enComentarioBloque = 0;
                i++;
            }
            continue;
        }

        // Omite contenido entre comillas "..." y '...'
        if (!enCadena && linea[i] == '\'' && !esEscapado(linea, i)) {
            enChar = !enChar;
            continue;
        }
        if (!enChar && linea[i] == '"' && !esEscapado(linea, i)) {
            enCadena = !enCadena;
            continue;
        }

        if (enCadena || enChar) {
            continue;
        }

        // Valida si la macro coincide en posición y límites de palabra
        if (strncmp(&linea[i], nombre, len) == 0) {
            int limiteIzquierdo = (i == 0) || !esCaracterIdentificador(linea[i - 1]);
            int limiteDerecho = !esCaracterIdentificador(linea[i + len]);

            if (limiteIzquierdo && limiteDerecho) {
                return (char *)&linea[i];
            }
        }
    }

    return NULL;
}

// Reemplaza las macros por el valor que representan.
void traducirMacros(char *linea) {
    char resultado[TamBuffer];
    int traducido = 1;
    int limitePasadas = 0;

    while (traducido && limitePasadas < 15) {
        traducido = 0;
        for (int i = 0; i < totalMacros; i++) {
            char *pos = buscarMacroValida(linea, macros[i].nombre);
            if (pos != NULL) {
                int antes = pos - linea;
                snprintf(resultado, sizeof(resultado), "%.*s%s%s",
                         antes, linea,
                         macros[i].valor,
                         pos + strlen(macros[i].nombre));
                strcpy(linea, resultado);
                traducido = 1;
                break;
            }
        }
        limitePasadas++;
    }
    if (limitePasadas >= 15)
    {
        fprintf(stderr, "Advertencia: se encontró un ciclo en los #define.\n");
    }
}

// Abre el archivo, revisa y procesa recursivamente los include y los define.
void procesarArchivo(char *nombreArchivo, FILE *f_out) {
    FILE *f_in = fopen(nombreArchivo, "r");
    if (!f_in) {
        fprintf(stderr, "Error: No se pudo abrir el archivo fuente '%s'.\n", nombreArchivo);
        exit(EXIT_FAILURE);
    }

    char linea[TamBuffer];
    while (fgets(linea, sizeof(linea), f_in)) {

        if (strncmp(linea, "#include", 8) == 0) {
            char subArchivo[MaxNombre];
            if (sscanf(linea, "#include \"%[^\"]\"", subArchivo) == 1) {
                procesarArchivo(subArchivo, f_out);
                continue;
            }
        }

        if (strncmp(linea, "#define", 7) == 0) {
            char nombre[MaxNombre];
            char valor[MaxValor];

            if (sscanf(linea, "#define %s %[^\n]", nombre, valor) >= 2) {
                agregarMacro(nombre, valor);
                fputs("\n", f_out);
                continue;
            }
        }

        traducirMacros(linea);
        fputs(linea, f_out);
    }
    fclose(f_in);
}

// Traduce el nombre del tipo del token a texto para imprimir.
char* obtenerNombreToken(TipoToken tipo) {
    switch (tipo) {
        case TokenPalabraReservada: return "PALABRA RESERVADA";
        case TokenID: return "IDENTIFICADOR";
        case TokenInt: return "INT";
        case TokenFloat: return "FLOAT";
        case TokenChar: return "CHAR";
        case TokenString: return "STRING";
        case TokenOp: return "OPERADOR";
        case TokenSeparador: return "SEPARADOR";
        case TokenModificador: return "MODIFICADOR";
        case TokenTipoDato: return "TIPO DE DATO";
        case TokenError: return "ERROR";
        default: return "DESCONOCIDO";
    }
}

/*
// Prepara algunos elementos para imprimir en LaTex.
void escaparCadena(char *lexema, char *lexemaArreglado){
    int j = 0;
    for (int i = 0; lexema[i] != '\0'; i++) {

        unsigned char c = (unsigned char)lexema[i];

        if (c >= 128) {
            j += sprintf(&lexemaArreglado[j], "\\textbackslash{}x%02X", c);
            continue;
        } else
        {
            switch (lexema[i]){
            case '{': case '}': case '#': case '%':
            case '&': case '_': case '$':
                lexemaArreglado[j++] = '\\';
                lexemaArreglado[j++] = lexema[i];
                break;
            case '\\':
                j += sprintf(&lexemaArreglado[j], "\\textbackslash{}");
                break;
            case '<':
                j += sprintf(&lexemaArreglado[j], "\\textless{}");
                break;
            case '>':
                j += sprintf(&lexemaArreglado[j], "\\textgreater{}");
                break;
            case '^':
                j += sprintf(&lexemaArreglado[j], "\\textasciicircum{}");
                break;
            case '~':
                j += sprintf(&lexemaArreglado[j], "\\textasciitilde{}");
                break;
            case '\'':
                j += sprintf(&lexemaArreglado[j], "\\textquotesingle{}");
                break;
            case '"':
                j += sprintf(&lexemaArreglado[j], "\"{}");
                break;
            default:
                lexemaArreglado[j++] = lexema[i];
                break;
            }
        }
    }
    lexemaArreglado[j] = '\0';
}

// Convierte un token normal o error léxico a su representación en Beamer con estilos
void formatearLexema(FILE *f_tex, int tipoToken, const char *lexema) {
    char lexemaArreglado[8192] = "";
    escaparCadena(lexema, lexemaArreglado);

    switch (tipoToken) {
        case TokenPalabraReservada:
            fprintf(f_tex, "{\\textcolor{ColorReservada}{\\ttfamily\\textbf{%s}}} ", lexemaArreglado);
            break;
        case TokenID:
            fprintf(f_tex, "{\\textcolor{ColorId}{\\sffamily %s}} ", lexemaArreglado);
            break;
        case TokenInt:
            fprintf(f_tex, "{\\textcolor{ColorNum}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenFloat:
            fprintf(f_tex, "{\\textcolor{ColorNum}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenOp:
            fprintf(f_tex, "{\\textcolor{ColorOp}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenSeparador:
            fprintf(f_tex, "{\\textcolor{ColorSeparador}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenError:
            fprintf(f_tex, "{\\textcolor{ColorError}{\\ttfamily\\textbf{%s}}} ", lexemaArreglado);
            break;
        case TokenChar:
            fprintf(f_tex, "{\\textcolor{ColorChar}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenString:
            fprintf(f_tex, "{\\textcolor{ColorString}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenModificador:
            fprintf(f_tex, "{\\textcolor{ColorMod}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenTipoDato:
            fprintf(f_tex, "{\\textcolor{ColorTipoDato}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        default:
            fprintf(f_tex, "%s ", lexemaArreglado);
    }
}

// Maneja la escritura del código preprocesado en los slides con un máximo de 17 líneas en cada uno.
void dividirCodigoSlides(FILE *f_tex) {

    Token token;
    int lineaAct = -1;
    int lineasSlide = 0;
    int numSlide = 1;

    do {
        token = getToken();

        if (token.tipo != TokenEOF) {
            if (lineaAct == -1) {
                lineaAct = token.numLinea;
                fprintf(f_tex, "\\begin{frame}[fragile]{Programa Fuente Procesado (Pág. %d)}\n  \\small\\raggedright\n  ", numSlide);
            } else {
                while (lineaAct < token.numLinea) {
                    fprintf(f_tex, "\\par\\noindent");
                    lineaAct++;
                    lineasSlide++;

                    if (lineasSlide >= 17) {
                        numSlide++;
                        fprintf(f_tex, "\n\\end{frame}\n\n");
                        fprintf(f_tex, "\\begin{frame}[fragile]{Programa Fuente Procesado (Pág. %d)}\n  \\small\\raggedright\n  ", numSlide);
                        lineasSlide = 0;
                    }
                }
            }

            formatearLexema(f_tex, token.tipo, token.lexema);

        if (token.lexema && strcmp(token.lexema, "EOF") != 0) {
                free(token.lexema);
            }
        }
    } while (token.tipo != TokenEOF);

    if (lineaAct != -1) {
        fprintf(f_tex, "\n\\end{frame}\n\n");
    }
}

// Genera y escribe el código LaTex para la presentación Beamer.
void generarBeamer(const char *nombreArchivo, TokenStats *stats, ErrorLexico *errores, int cantErrores) {
    FILE *f = fopen(nombreArchivo, "w");
    if (!f) {
        perror("Error al crear el archivo .tex");
        return;
    }

    // Prólogo de LaTeX y definición de colores y estilos
    fprintf(f, "\\documentclass{beamer}\n");
    fprintf(f, "\\usepackage[utf8]{inputenc}\n");
    fprintf(f, "\\usepackage[spanish]{babel}\n");
    fprintf(f, "\\usepackage{pgfplots}\n");
    fprintf(f, "\\usepackage{booktabs}\n");
    fprintf(f, "\\usepackage{xcolor}\n");
    fprintf(f, "\\usepackage{amssymb}\n");
    fprintf(f, "\\usepackage{textcomp}\n");

    fprintf(f, "\\usepgfplotslibrary{polar}\n");
    fprintf(f, "\\pgfplotsset{compat=1.18}\n");
    
    fprintf(f, "\\usetheme{Madrid}\n");
    fprintf(f, "\\usecolortheme{whale}\n\n");

    // COLORES
    fprintf(f, "\\definecolor{ColorReservada}{RGB}{41, 128, 185}\n");
    fprintf(f, "\\definecolor{ColorId}{RGB}{39, 174, 96}\n");
    fprintf(f, "\\definecolor{ColorNum}{RGB}{142, 68, 173}\n");
    fprintf(f, "\\definecolor{ColorOp}{RGB}{230, 126, 34}\n");
    fprintf(f, "\\definecolor{ColorSeparador}{RGB}{181, 125, 219}\n");
    fprintf(f, "\\definecolor{ColorError}{RGB}{192, 57, 43}\n\n");
    fprintf(f, "\\definecolor{ColorChar}{RGB}{28, 89, 5}\n\n");
    fprintf(f, "\\definecolor{ColorString}{RGB}{202, 30, 100}\n\n");
    fprintf(f, "\\definecolor{ColorMod}{RGB}{21, 80, 82}\n\n");
    fprintf(f, "\\definecolor{ColorTipoDato}{RGB}{10, 125, 121}\n\n");

    // Datos del documento Beamer
    fprintf(f, "\\title[Análisis Léxico]{Reporte de Análisis Léxico}\n");
    fprintf(f, "\\subtitle{Compilador de C - Proyecto 1}\n");
    fprintf(f, "\\author{Dayana Rojas Campos y Nicole Bou Espinoza}\n");
    fprintf(f, "\\institute{Compiladores e Intérpretes\\\\"
               "                    Semestre II, 2026}\n");
    fprintf(f, "\\date{\\today}\n\n");

    fprintf(f, "\\begin{document}\n\n");


    // Diapositiva 1: Portada
    fprintf(f, "\\frame{\\titlepage}\n\n");


    // Diapositiva 2
    fprintf(f, "\\begin{frame}{Flujo de Ejecución (1/5)}\n");
    fprintf(f, "Para iniciar, se revisa si recibió instrucciones de preproceso o nombre de archivo de salida. ");
    fprintf(f, "El preproceso, como fue especificado, está predeterminado a estar encendido. ");
    fprintf(f, "Se maneja el archivo fuente y se abre un archivo temporal. Pasa a \\texttt{procesarArchivo}. ");
    fprintf(f, "Ahí se encarga de revisar los \\texttt{\\#include} de manera recursiva y las macros, ");
    fprintf(f, "que deben pasar por dos funciones: \\texttt{agregarMacro} y \\texttt{traducirMacros}. ");
    fprintf(f, "La primera, se encarga de agregarlos al ``diccionario'' de macros y, el segundo, ");
    fprintf(f, "se encarga de reemplazarlas con sus valores. Para asegurarse de que no reemplace de más, ");
    fprintf(f, "se tiene otra función llamada \\texttt{buscarMacroValida}.\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 3
    fprintf(f, "\\begin{frame}{Flujo de Ejecución (2/5)}\n");
    fprintf(f, "Una vez completado el preproceso, pasamos al scanner. Para esta parte, usamos Flex, ");
    fprintf(f, "el cual nos permite escribir alfabetos y reglas como expresiones regulares por aparte en un archivo \\texttt{.l}. ");
    fprintf(f, "Flex después toma esas expresiones regulares y, al compilar el \\texttt{scanner.l}, ");
    fprintf(f, "las convierte en un DFA como lo sería el archivo \\texttt{lex.yy.c}. ");
    fprintf(f, "Para asegurarnos de que no se le pase ningún error, los vamos guardando junto con un contador de errores.\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 4
    fprintf(f, "\\begin{frame}{Flujo de Ejecución (3/5)}\n");
    fprintf(f, "Después, entramos en un ciclo para procesar los tokens. Con ayuda de \\texttt{getToken} ");
    fprintf(f, "(el cual fue especificado en el flex), se van a abordar las estadísticas para tener un recuento ");
    fprintf(f, "de los tipos identificados. Para esta parte, agrupamos algunos tokens en la misma categoría ");
    fprintf(f, "para simplificar la visualización de las estadísticas. Si es un token de tipo error ");
    fprintf(f, "y aún no hemos llegado al máximo de errores, se guarda la línea, el lexema y una descripción general del error.\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 5
    fprintf(f, "\\begin{frame}{Flujo de Ejecución (4/5)}\n");
    fprintf(f, "Ahora, para la parte de escribir el código en el beamer, vamos a usar el archivo temporal como referencia. ");
    fprintf(f, "Para esto, necesitamos hacer un \\texttt{rewind(yyin)} y \\texttt{yyrestart(yyin)} para que el puntero regrese al inicio. ");
    fprintf(f, "Después de eso, llama a la función \\texttt{generarBeamer}, donde se imprime la portada, ");
    fprintf(f, "esta descripción del preproceso, un resumen de categorías de tokens, el reporte de los errores léxicos, ");
    fprintf(f, "el histograma, el gráfico de pastel y la visualización del código preprocesado con ayuda de una función llamada ");
    fprintf(f, "\\texttt{dividirCodigoSlides}. Una vez esto esté listo, se cierra \\texttt{yyin} para evitar fugas.\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 6
    fprintf(f, "\\begin{frame}{Flujo de Ejecución (5/5)}\n");
    fprintf(f, "La última parte corresponde a que no es solo escribir el Latex, sino también desplegarlo. ");
    fprintf(f, "Para esto, llamamos a \\texttt{compilarTexPdf}, el cual ejecuta el comando de \\texttt{pdflatex} dos veces, ");
    fprintf(f, "para asegurarse de que sí compile bien. Si todo está en orden con eso, llama a \\texttt{desplegarPdf}, ");
    fprintf(f, "quien se encarga de correr el comando de \\texttt{evince} para poder visualizar el beamer.\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 7: Resumen de Categorías de Tokens
    fprintf(f, "\\begin{frame}{Resumen de Categorías de Tokens}\n");
    fprintf(f, "  \\begin{center}\n");
    fprintf(f, "    \\begin{tabular}{llc}\n");
    fprintf(f, "      \\toprule\n");
    fprintf(f, "      \\textbf{Categoría} & \\textbf{Estilo Visual} & \\textbf{Cantidad} \\\\\n");
    fprintf(f, "      \\midrule\n");
    fprintf(f, "      Palabras Reservadas & \\textcolor{ColorReservada}{$\\blacksquare$~Reservada} & %d \\\\\n", stats->palabrasReservadas);
    fprintf(f, "      Identificadores     & \\textcolor{ColorId}{$\\blacksquare$~ID} & %d \\\\\n", stats->identificadores);
    fprintf(f, "      Números          & \\textcolor{ColorNum}{$\\blacksquare$~Num} & %d \\\\\n", stats->num);
    fprintf(f, "      Operadores          & \\textcolor{ColorOp}{$\\blacksquare$~Operador} & %d \\\\\n", stats->operadores);
    fprintf(f, "      Separadores       & \\textcolor{ColorSeparador}{$\\blacksquare$~Separador} & %d \\\\\n", stats->separadores);
    fprintf(f, "      \\midrule\n");
    fprintf(f, "      \\textbf{Errores Léxicos} & \\textcolor{ColorError}{$\\blacksquare$~Error Léxico} & \\textbf{%d} \\\\\n", stats->erroresLexicos);
    fprintf(f, "      \\bottomrule\n");
    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "  \\end{center}\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 8: Reporte de errores léxicos
    if (cantErrores == 0) {
    fprintf(f, "\\begin{frame}{Detalle de Errores Léxicos}\n");
    fprintf(f, "  \\begin{exampleblock}{Estado del Análisis}\n");
    fprintf(f, "    No se detectaron errores léxicos durante la fase de escaneo.\n");
    fprintf(f, "  \\end{exampleblock}\n");
    fprintf(f, "\\end{frame}\n\n");
} else {
    int totalPaginas = (cantErrores + ErroresPorPagina - 1) / ErroresPorPagina;

    for (int i = 0; i < cantErrores; i++) {
        int paginaActual = (i / ErroresPorPagina) + 1;

        // Apertura de un nuevo frame y tabla al inicio de cada página
        if (i % ErroresPorPagina == 0) {
            fprintf(f, "\\begin{frame}{Detalle de Errores Léxicos (%d/%d)}\n", paginaActual, totalPaginas);
            fprintf(f, "  \\begin{center}\n");
            fprintf(f, "    \\small\n");
            fprintf(f, "    \\begin{tabular}{c c p{5.5cm}}\n"); // Ancho fijo en la 3ra columna para evitar desbordes
            fprintf(f, "      \\toprule\n");
            fprintf(f, "      \\textbf{Línea} & \\textbf{Lexema} & \\textbf{Descripción} \\\\\n");
            fprintf(f, "      \\midrule\n");
        }

        char lexemaArreglado[8192];
        escaparCadena(errores[i].lexema, lexemaArreglado);

        // Imprimir la fila con cuatro barras invertidas (\\\\) al final
        fprintf(f, "      %d & \\textcolor{ColorError}{\\texttt{%s}} & %s \\\\\n",
                errores[i].linea, lexemaArreglado, errores[i].descripcion);

        // Cierre de la tabla y del frame
        if ((i + 1) % ErroresPorPagina == 0 || i == cantErrores - 1) {
            fprintf(f, "      \\bottomrule\n");
            fprintf(f, "    \\end{tabular}\n");
            fprintf(f, "  \\end{center}\n");
            fprintf(f, "\\end{frame}\n\n");
        }
    }
}

    // Diapositiva 9: Histograma de Tokens con pgfplots
    fprintf(f, "\\begin{frame}{Distribución de Tokens (Histograma)}\n");
    fprintf(f, "  \\begin{center}\n");
    fprintf(f, "    \\begin{tikzpicture}\n");
    fprintf(f, "      \\begin{axis}[\n");
    fprintf(f, "        ybar,\n");
    fprintf(f, "        symbolic x coords={Reservada,Id,Num,Op,Separador,Error},\n");
    fprintf(f, "        xtick={Reservada,Id,Num,Op,Separador,Error},\n");
    fprintf(f, "        x tick label style={rotate=30, anchor=east, font=\\small},\n");
    fprintf(f, "        nodes near coords,\n");
    fprintf(f, "        nodes near coords align={vertical},\n");
    fprintf(f, "        ymin=0,\n");
    fprintf(f, "        ylabel={Frecuencia},\n");
    fprintf(f, "        xlabel={Categorías},\n");
    fprintf(f, "        bar width=18pt,\n");
    fprintf(f, "        bar shift=0pt,\n");
    fprintf(f, "        width=0.85\\textwidth,\n");
    fprintf(f, "        height=0.6\\textwidth,\n");
    fprintf(f, "        enlarge x limits=0.15\n");
    fprintf(f, "      ]\n");

    // Graficar histograma asociando las frecuencias
    fprintf(f, "        \\addplot[draw=ColorReservada!80!black, fill=ColorReservada] coordinates {(Reservada,%d)};\n", stats->palabrasReservadas);
    fprintf(f, "        \\addplot[draw=ColorId!80!black, fill=ColorId] coordinates {(Id,%d)};\n", stats->identificadores);
    fprintf(f, "        \\addplot[draw=ColorNum!80!black, fill=ColorNum] coordinates {(Num,%d)};\n", stats->num);
    fprintf(f, "        \\addplot[draw=ColorOp!80!black, fill=ColorOp] coordinates {(Op,%d)};\n", stats->operadores);
    fprintf(f, "        \\addplot[draw=ColorSeparador!80!black, fill=ColorSeparador] coordinates {(Separador,%d)};\n", stats->separadores);
    fprintf(f, "        \\addplot[draw=ColorError!80!black, fill=ColorError] coordinates {(Error,%d)};\n", stats->erroresLexicos);

    
    fprintf(f, "      \\end{axis}\n");
    fprintf(f, "    \\end{tikzpicture}\n");
    fprintf(f, "  \\end{center}\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 10: Gráfico de Pastel en pgfplots
    
    int totalTokens = stats->palabrasReservadas + stats->identificadores + 
                       stats->num + stats->operadores + 
                       stats->separadores + stats->erroresLexicos;

    fprintf(f, "\\begin{frame}{Distribución Porcentual (Gráfico de Pastel)}\n");
    if (totalTokens == 0) {
        fprintf(f, "  \\begin{alertblock}{Sin Datos}\n");
        fprintf(f, "    No se registraron tokens para generar el gráfico de pastel.\n");
        fprintf(f, "  \\end{alertblock}\n");
    } else {
        fprintf(f, "  \\begin{center}\n");
        fprintf(f, "    \\begin{tikzpicture}\n");
        // Entorno pgfplots con 'axis equal image' para evitar la deformación del gráfico
        fprintf(f, "      \\begin{axis}[\n");
        fprintf(f, "        hide axis,\n");
        fprintf(f, "        axis equal image,\n"); // CLAVE: Mantiene la proporción 1:1 en pgfplots
        fprintf(f, "        xmin=-1.8, xmax=1.8,\n");
        fprintf(f, "        ymin=-1.8, ymax=1.8\n");
        fprintf(f, "      ]\n");

        struct {
            int cantidad;
            const char *color;
        } categorias[6] = {
            {stats->palabrasReservadas, "ColorReservada"},
            {stats->identificadores,    "ColorId"},
            {stats->num,                "ColorNum"},
            {stats->operadores,         "ColorOp"},
            {stats->separadores,        "ColorSeparador"},
            {stats->erroresLexicos,     "ColorError"}
        };

        double anguloActual = 0.0;
        for (int i = 0; i < 6; i++) {
            if (categorias[i].cantidad > 0) {
                double porcentaje = ((double)categorias[i].cantidad / totalTokens) * 100.0;
                double anguloPorcion = ((double)categorias[i].cantidad / totalTokens) * 360.0;
                double anguloFin = anguloActual + anguloPorcion;
                double anguloMedio = anguloActual + (anguloPorcion / 2.0);

                // Dibujar el sector circular en el sistema de coordenadas de pgfplots
                fprintf(f, "        \\draw[fill=%s, draw=white, thick] (axis cs:0,0) -- (%.2f:1.3) arc (%.2f:%.2f:1.3) -- cycle;\n",
                        categorias[i].color, anguloActual, anguloActual, anguloFin);

                // Etiqueta de porcentaje
                if (porcentaje >= 5.0) {
                    fprintf(f, "        \\node[white, font=\\bfseries\\tiny] at (%.2f:0.85) {%.1f\\%%};\n",
                            anguloMedio, porcentaje);
                }

                anguloActual = anguloFin;
            }
        }

        fprintf(f, "      \\end{axis}\n");
        fprintf(f, "    \\end{tikzpicture}\n");

        // Leyenda
        fprintf(f, "    \\vspace{0.2cm}\n");
        fprintf(f, "    \\tiny\n");
        fprintf(f, "    \\begin{tabular}{c c c}\n");
        fprintf(f, "      \\textcolor{ColorReservada}{$\\blacksquare$}~Reservada (%.1f\\%%) &\n", ((double)stats->palabrasReservadas/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorId}{$\\blacksquare$}~Id (%.1f\\%%) &\n", ((double)stats->identificadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorNum}{$\\blacksquare$}~Num (%.1f\\%%) \\\\\n", ((double)stats->num/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorOp}{$\\blacksquare$}~Op (%.1f\\%%) &\n", ((double)stats->operadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorSeparador}{$\\blacksquare$}~Separador (%.1f\\%%) &\n", ((double)stats->separadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorError}{$\\blacksquare$}~Error (%.1f\\%%)\\\\\n", ((double)stats->erroresLexicos/totalTokens)*100.0);
        fprintf(f, "    \\end{tabular}\n");
        fprintf(f, "  \\end{center}\n");
    }
    fprintf(f, "\\end{frame}\n\n");

    // Diapositivas siguientes: código preprocesado

    dividirCodigoSlides(f);

    fprintf(f, "\\end{document}\n");

    fclose(f);
    printf("Archivo Beamer generado exitosamente: %s\n", nombreArchivo);
}
*/