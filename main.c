#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compilador.h"
#include "beamer.h"

Macro macros[MaxMacros];
int totalMacros = 0;

extern FILE *yyin;
extern void yyrestart(FILE *new_file);

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 1; // El preproceso está activado por defecto.
    char *archivoFuente = NULL;
    char *archivoSalida = "salidaPresentacion.tex";

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
    } while (token.tipo != TokenEOF);

    fclose(yyin);

    char *nombreBeamer = archivoSalida ? archivoSalida : "salidaPresentacion.tex";
    generarBeamer(nombreBeamer, &stats, errores, cantErrores);

    char archivoPDF[256];
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
    }

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


// Convierte un token normal o error léxico a su representación en Beamer con estilos
void formatearLexema(FILE *f_tex, int tipoToken, const char *lexema) {
    char lexemaArreglado[256] = "";
    int j = 0;
    for (int i = 0; lexema[i] != '\0'; i++) {
        if (lexema[i] == '{' || lexema[i] == '}' || lexema[i] == '#' ||
            lexema[i] == '%' || lexema[i] == '&' || lexema[i] == '_') {
            lexemaArreglado[j++] = '\\';
        }
        lexemaArreglado[j++] = lexema[i];
    }
    lexemaArreglado[j] = '\0';

    switch (tipoToken) {

        case TokenPalabraReservada:
            fprintf(f_tex, "\\colorbox{bgKey}{\\textcolor{ColorKeyword}{\\ttfamily\\textbf{%s}}} ", lexemaArreglado);
            break;
        case TokenID:
            fprintf(f_tex, "\\colorbox{bgId}{\\textcolor{ColorId}{\\sffamily %s}} ", lexemaArreglado);
            break;
        case TokenInt:
            fprintf(f_tex, "\\colorbox{bgConst}{\\textcolor{ColorConst}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenFloat:
            fprintf(f_tex, "\\colorbox{bgConst}{\\textcolor{ColorConst}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenOp:
            fprintf(f_tex, "\\colorbox{bgOp}{\\textcolor{ColorOp}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenSeparador:
            fprintf(f_tex, "\\colorbox{bgDelim}{\\textcolor{ColorDelim}{\\texttt{%s}}} ", lexemaArreglado);
            break;
        case TokenError:
            fprintf(f_tex, "\\colorbox{bgError}{\\textcolor{ColorError}{\\ttfamily\\textbf{%s}}} ", lexemaArreglado);
            break;
        default:
            fprintf(f_tex, "%s ", lexemaArreglado);
    }
}

// Maneja la escritura del código preprocesado en los slides con un máximo de 20 líneas en cada uno.
void dividirCodigoSlides(FILE *f_tex) {

    Token token;
    int lineaAct = 1;
    int lineasSlide = 0;
    int numSlide = 1;

    fprintf(f_tex, "\\begin{frame}[fragile]{Programa Fuente Procesado (Pág. %d)}\n  \\small\n  ", numSlide);

    do {
        token = getToken();

        if (token.tipo != TokenEOF) {
            // Si el token proviene de una nueva línea en el archivo original, hace salto de línea en LaTeX
            while (lineaAct < token.numLinea) {
                fprintf(f_tex, " \\\\\n  ");
                lineaAct++;
                lineasSlide++;

                if (lineasSlide >= 20) {
                    numSlide++;
                    fprintf(f_tex, "\\end{frame}\n\n");
                    fprintf(f_tex, "\\begin{frame}[fragile]{Programa Fuente Procesado (Pág. %d)}\n  \\small\n  ", numSlide);
                    lineasSlide = 0;
                }
            }

            formatearLexema(f_tex, token.tipo, token.lexema);
        }
    } while (token.tipo != TokenEOF);

    fprintf(f_tex, "\n\\end{frame}\n\n");
}

void generarBeamer(const char *nombreArchivo, TokenStats *stats, ErrorLexico *errores, int cantErrores) {
    FILE *f = fopen(nombreArchivo, "w");
    if (!f) {
        perror("Error al crear el archivo .tex");
        return;
    }

    // Prólogo de LaTeX y definición de colores y estilos
    fprintf(f, "\\documentclass{beamer}\n");
    fprintf(f, "\\usepackage[utf8]{inputenc}\n");
    //fprintf(f, "\\usepackage[spanish]{babel}\n");
    fprintf(f, "\\usepackage{pgfplots}\n");
    fprintf(f, "\\usepackage{booktabs}\n");
    fprintf(f, "\\usepackage{xcolor}\n");
    fprintf(f, "\\usepackage{amssymb}\n");

    fprintf(f, "\\usepgfplotslibrary{polar}\n");
    fprintf(f, "\\pgfplotsset{compat=1.18}\n");
    
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
    fprintf(f, "      Palabras Reservadas & \\textcolor{ColorKeyword}{$\\blacksquare$~Keyword} & %d \\\\\n", stats->palabrasReservadas);
    fprintf(f, "      Identificadores     & \\textcolor{ColorId}{$\\blacksquare$~Identifier} & %d \\\\\n", stats->identificadores);
    fprintf(f, "      Constantes          & \\textcolor{ColorConst}{$\\blacksquare$~Constant} & %d \\\\\n", stats->num);
    fprintf(f, "      Operadores          & \\textcolor{ColorOp}{$\\blacksquare$~Operator} & %d \\\\\n", stats->operadores);
    fprintf(f, "      Separadores       & \\textcolor{ColorDelim}{$\\blacksquare$~Delimiter} & %d \\\\\n", stats->separadores);
    fprintf(f, "      \\midrule\n");
    fprintf(f, "      \\textbf{Errores Léxicos} & \\textcolor{ColorError}{$\\blacksquare$~Lexical Error} & \\textbf{%d} \\\\\n", stats->erroresLexicos);
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
    fprintf(f, "        symbolic x coords={Reservada,Id,Num,Op,Separador,Error},\n");
    fprintf(f, "        xtick=data,\n");
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
    fprintf(f, "        \\addplot[draw=ColorKeyword!80!black, fill=ColorKeyword] coordinates {(Reservada,%d)};\n", stats->palabrasReservadas);
    fprintf(f, "        \\addplot[draw=ColorId!80!black, fill=ColorId] coordinates {(Id,%d)};\n", stats->identificadores);
    fprintf(f, "        \\addplot[draw=ColorConst!80!black, fill=ColorConst] coordinates {(Num,%d)};\n", stats->num);
    fprintf(f, "        \\addplot[draw=ColorOp!80!black, fill=ColorOp] coordinates {(Op,%d)};\n", stats->operadores);
    fprintf(f, "        \\addplot[draw=ColorDelim!80!black, fill=ColorDelim] coordinates {(Separador,%d)};\n", stats->separadores);
    fprintf(f, "        \\addplot[draw=ColorError!80!black, fill=ColorError] coordinates {(Error,%d)};\n", stats->erroresLexicos);

    
    fprintf(f, "      \\end{axis}\n");
    fprintf(f, "    \\end{tikzpicture}\n");
    fprintf(f, "  \\end{center}\n");
    fprintf(f, "\\end{frame}\n\n");

    // Diapositiva 5: Gráfico de Pastel en pgfplots
    
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
            {stats->palabrasReservadas, "ColorKeyword"},
            {stats->identificadores,    "ColorId"},
            {stats->num,                "ColorConst"},
            {stats->operadores,         "ColorOp"},
            {stats->separadores,        "ColorDelim"},
            {stats->erroresLexicos,     "ColorError"}
        };

        double angulo_actual = 0.0;
        for (int i = 0; i < 6; i++) {
            if (categorias[i].cantidad > 0) {
                double porcentaje = ((double)categorias[i].cantidad / totalTokens) * 100.0;
                double anguloPorcion = ((double)categorias[i].cantidad / totalTokens) * 360.0;
                double anguloFin = angulo_actual + anguloPorcion;
                double anguloMedio = angulo_actual + (anguloPorcion / 2.0);

                // Dibujar el sector circular en el sistema de coordenadas de pgfplots
                fprintf(f, "        \\draw[fill=%s, draw=white, thick] (axis cs:0,0) -- (%.2f:1.3) arc (%.2f:%.2f:1.3) -- cycle;\n",
                        categorias[i].color, angulo_actual, angulo_actual, anguloFin);

                // Etiqueta de porcentaje
                if (porcentaje >= 5.0) {
                    fprintf(f, "        \\node[white, font=\\bfseries\\tiny] at (%.2f:0.85) {%.1f\\%%};\n",
                            anguloMedio, porcentaje);
                }

                angulo_actual = anguloFin;
            }
        }

        fprintf(f, "      \\end{axis}\n");
        fprintf(f, "    \\end{tikzpicture}\n");

        // Leyenda
        fprintf(f, "    \\vspace{0.2cm}\n");
        fprintf(f, "    \\tiny\n");
        fprintf(f, "    \\begin{tabular}{c c c}\n");
        fprintf(f, "      \\textcolor{ColorKeyword}{$\\blacksquare$}~Key (%.1f\\%%) &\n", ((double)stats->palabrasReservadas/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorId}{$\\blacksquare$}~Id (%.1f\\%%) &\n", ((double)stats->identificadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorConst}{$\\blacksquare$}~Const (%.1f\\%%) \\\\\n", ((double)stats->num/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorOp}{$\\blacksquare$}~Op (%.1f\\%%) &\n", ((double)stats->operadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorDelim}{$\\blacksquare$}~Delim (%.1f\\%%) &\n", ((double)stats->separadores/totalTokens)*100.0);
        fprintf(f, "      \\textcolor{ColorError}{$\\blacksquare$}~Error (%.1f\\%%)\n", ((double)stats->erroresLexicos/totalTokens)*100.0);
        fprintf(f, "    \\end{tabular}\n");
        fprintf(f, "  \\end{center}\n");
    }
    fprintf(f, "\\end{frame}\n\n");

    rewind(yyin);
    yyrestart(yyin);
    dividirCodigoSlides(f);

    fprintf(f, "\\end{document}\n");

    fclose(f);
    printf("Archivo Beamer generado exitosamente: %s\n", nombreArchivo);
}