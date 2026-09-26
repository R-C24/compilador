#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compilador.h"
#include "beamer.h"

Macro macros[MaxMacros];
int totalMacros = 0;

extern FILE *yyin;

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 1; // El preproceso está activado por defecto.
    char *archivoFuente = NULL;
    char *archivoSalida = NULL;

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

    TokenStats stats = {0};
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
                case TokenID:
                    stats.identificadores++;
                    break;
                case TokenInt | TokenFloat:
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
    } while (token.tipo != TokenEOF);

    fclose(yyin);

    char *nombreBeamer = archivoSalida ? archivoSalida : "salidaPresentacion.tex";
    generarBeamer(nombreBeamer, &stats, errores, cantErrores);

    printf("Proceso del Proyecto 1 finalizado.\n");
    return EXIT_SUCCESS;
}

// Agrega la información de un nuevo macro al diccionario de macros.
void agregarMacro(char *nombre, char *valor) {
    if (totalMacros < MaxMacros) {
        strncpy(macros[totalMacros].nombre, nombre, MaxNombre);
        strncpy(macros[totalMacros].valor, valor, MaxValor);
        totalMacros++;
    }
}

// REVISAR Hay que cuidar que la sustitución no reemplace nombres de macros dentro de cadenas de texto literal (por ejemplo printf("TOTAL");) o dentro de nombres de variables compuestas (por ejemplo TOTAL_SUMA).
// Reemplaza las macros por el valor que representan.
void traducirMacros(char *linea) {
    char resultado[TamBuffer];
    int traducido = 1;

    while (traducido) {
        traducido = 0;
        for (int i = 0; i < totalMacros; i++) {
            char *pos = strstr(linea, macros[i].nombre);
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
                continue; // REVISAR
            }
        }

        if (strncmp(linea, "#define", 7) == 0) {
            char nombre[MaxNombre];
            char valor[MaxValor];

            if (sscanf(linea, "#define %s %[^\n]", nombre, valor) >= 2) {
                agregarMacro(nombre, valor);
                continue; // REVISAR
            }
        }

        traducirMacros(linea);
        fputs(linea, f_out);
    }
    fclose(f_in);
}

char* obtenerNombreToken(TipoToken tipo) {
    switch (tipo) {
        case TokenPalabraReservada: return "PALABRA_RESERVADA";
        case TokenID: return "IDENTIFICADOR";
        case TokenInt: return "INT";
        case TokenFloat: return "FLOAT";
        case TokenChar: return "CHAR";
        case TokenString: return "STRING";
        case TokenOp: return "OPERADOR";
        case TokenSeparador: return "SEPARADOR";
        case TokenModificador: return "MODIFICADOR";
        case TokenError: return "ERROR";
        default: return "DESCONOCIDO";
    }
}

void generarBeamer(const char *nombreArchivo, TokenStats *stats, ErrorLexico *errores, int cantErrores) {
    FILE *f = fopen(nombreArchivo, "w");
    if (!f) {
        perror("Error al crear el archivo .tex");
        return;
    }

    // Prólogo de LaTeX y definición de colores y estilos
    fprintf(f, "\\documentclass{beamer}\n");
    //fprintf(f, "\\usepackage[utf8]{utf8}\n");
    fprintf(f, "\\usepackage[spanish]{babel}\n");
    fprintf(f, "\\usepackage{pgfplots}\n");
    fprintf(f, "\\usepackage{booktabs}\n");
    fprintf(f, "\\usepackage{xcolor}\n");
    fprintf(f, "\\usepackage{amssymb}\n");
    fprintf(f, "\\pgfplotsset{compat=1.18}\n\n");
    
    fprintf(f, "\\usetheme{Madrid}\n");
    fprintf(f, "\\usecolortheme{whale}\n\n");

    // COLOREEEEEEEEEEESSSSSSSSSSS
    fprintf(f, "\\definecolor{ColorKeyword}{RGB}{41, 128, 185}\n");    // Azul
    fprintf(f, "\\definecolor{ColorId}{RGB}{39, 174, 96}\n");         // Verde
    fprintf(f, "\\definecolor{ColorConst}{RGB}{142, 68, 173}\n");     // Morado
    fprintf(f, "\\definecolor{ColorOp}{RGB}{230, 126, 34}\n");        // Naranja
    fprintf(f, "\\definecolor{ColorDelim}{RGB}{127, 140, 141}\n");    // Gris
    fprintf(f, "\\definecolor{ColorError}{RGB}{192, 57, 43}\n\n");    // Rojo

    // Datos del documento Beamer
    fprintf(f, "\\title[Análisis Léxico]{Reporte de Análisis Léxico}\n");
    fprintf(f, "\\subtitle{Compilador de C - Proyecto 1}\n");
    fprintf(f, "\\author{Dayana Rojas Campos y Nicole Bou Espinoza}\n");
    fprintf(f, "\\date{\\today}\n\n");

    fprintf(f, "\\begin{document}\n\n");


    // Diapositiva 1: Portada
    fprintf(f, "\\frame{\\titlepage}\n\n");


    // Diapositiva 2: Resumen de Categorías de Tokens
    fprintf(f, "\\begin{frame}{Resumen de Categorías de Tokens}\n");
    fprintf(f, "  \\begin{center}\n");
    fprintf(f, "    \\begin{tabular}{llc}\n");
    fprintf(f, "      \\toprule\n");
    fprintf(f, "      \\textbf{Categoría} & \\textbf{Estilo Visual} & \\textbf{Cantidad} \\\\\n");
    fprintf(f, "      \\midrule\n");
    fprintf(f, "      Palabras Reservadas & \\textcolor{ColorKeyword}{$\blacksquare$~Keyword} & %d \\\\\n", stats->palabrasReservadas);
    fprintf(f, "      Identificadores     & \\textcolor{ColorId}{$\blacksquare$~Identifier} & %d \\\\\n", stats->identificadores);
    fprintf(f, "      Constantes          & \\textcolor{ColorConst}{$\blacksquare$~Constant} & %d \\\\\n", stats->num);
    fprintf(f, "      Operadores          & \\textcolor{ColorOp}{$\blacksquare$~Operator} & %d \\\\\n", stats->operadores);
    fprintf(f, "      Separadores       & \\textcolor{ColorDelim}{$\blacksquare$~Delimiter} & %d \\\\\n", stats->separadores);
    fprintf(f, "      \\midrule\n");
    fprintf(f, "      \\textbf{Errores Léxicos} & \\textcolor{ColorError}{$\blacksquare$~Lexical Error} & \\textbf{%d} \\\\\n", stats->erroresLexicos);
    fprintf(f, "      \\bottomrule\n");
    fprintf(f, "    \\end{tabular}\n");
    fprintf(f, "  \\end{center}\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 3: Reporte de errores léxicos
    fprintf(f, "\\begin{frame}{Detalle de Errores Léxicos}\n");
    if (cantErrores == 0) {
        fprintf(f, "  \\begin{exampleblock}{Estado del Análisis}\n");
        fprintf(f, "    No se detectaron errores léxicos durante la fase de escaneo.\n");
        fprintf(f, "  \\end{exampleblock}\n");
    } else {
        fprintf(f, "  Se identificaron los siguientes caracteres no reconocidos o mal formados:\n\\vspace{0.3cm}\n");
        fprintf(f, "  \\begin{center}\n");
        fprintf(f, "    \\begin{tabular}{c c l}\n");
        fprintf(f, "      \\toprule\n");
        fprintf(f, "      \\textbf{Línea} & \\textbf{Lexema} & \\textbf{Descripción} \\\\\n");
        fprintf(f, "      \\midrule\n");
        for (int i = 0; i < cantErrores; i++) {
            fprintf(f, "      %d & \\textcolor{ColorError}{\\texttt{%s}} & %s \\\\\n",
                    errores[i].linea, errores[i].lexema, errores[i].descripcion);
        }
        fprintf(f, "      \\bottomrule\n");
        fprintf(f, "    \\end{tabular}\n");
        fprintf(f, "  \\end{center}\n");
    }
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 4: Histograma de Tokens con pgfplots
    fprintf(f, "\\begin{frame}{Distribución de Tokens (Histograma)}\n");
    fprintf(f, "  \\begin{center}\n");
    fprintf(f, "    \\begin{tikzpicture}\n");
    fprintf(f, "      \\begin{axis}[\n");
    fprintf(f, "        ybar,\n");
    fprintf(f, "        symbolic x coordinates={Key, Id, Const, Op, Delim, Error},\n");
    fprintf(f, "        xtick=data,\n");
    fprintf(f, "        nodes near coords,\n");
    fprintf(f, "        nodes near coords align={vertical},\n");
    fprintf(f, "        ymin=0,\n");
    fprintf(f, "        ylabel={Frecuencia},\n");
    fprintf(f, "        xlabel={Categorías},\n");
    fprintf(f, "        bar width=18pt,\n");
    fprintf(f, "        width=0.85\\textwidth,\n");
    fprintf(f, "        height=0.6\\textwidth,\n");
    fprintf(f, "        enlarge x limits=0.15\n");
    fprintf(f, "      ]\n");

    // Graficar histograma asociando las frecuencias
    fprintf(f, "        \\addplot[draw=blue!50!black, fill=blue!30] coordinates {\n");
    fprintf(f, "          (Key,%d)\n", stats->palabrasReservadas);
    fprintf(f, "          (Id,%d)\n", stats->identificadores);
    fprintf(f, "          (Const,%d)\n", stats->num);
    fprintf(f, "          (Op,%d)\n", stats->operadores);
    fprintf(f, "          (Delim,%d)\n", stats->separadores);
    fprintf(f, "          (Error,%d)\n", stats->erroresLexicos);
    fprintf(f, "        };\n");

    fprintf(f, "      \\end{axis}\n");
    fprintf(f, "    \\end{tikzpicture}\n");
    fprintf(f, "  \\end{center}\n");
    fprintf(f, "\\end{frame}\n\n");

    fprintf(f, "\\end{document}\n");

    fclose(f);
    printf("[OK] Archivo Beamer generado exitosamente: %s\n", nombreArchivo);
}