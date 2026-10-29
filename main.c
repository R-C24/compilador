#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "compilador.h"


Macro macros[MaxMacros];
int totalMacros = 0;

EstiloPrettyPrint estilo = propio;

extern FILE *yyin;
extern void yyrestart(FILE *new_file);
extern struct ASTNodo *raizAST;
extern int yyparse();

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 0; // El preproceso está desactivado por defecto.
    int reemplazarOriginal = 0; // Está desactivado por defecto.
    char *archivoFuente = NULL;

    while ((opt = getopt(argc, argv, "prgb")) != -1) {
        switch (opt) {
            case 'p':
                activarPreproceso = 1;
                break;
            case 'r':
                reemplazarOriginal = 1;
                break;
            case 'g':
                estilo = estiloGNU;
                break;
            case 'b':
                estilo = estiloBSD;
                break;
            default:
            fprintf(stderr, "\nError: no es una opción válida.\n");
                fprintf(stderr, "Uso correcto: %s [-p] [-r] [-g|-b] <archivo.c>\n\n", argv[0]);
                fprintf(stderr, "Opciones disponibles:\n");
                fprintf(stderr, "  -p    Activa el preprocesador opcional.\n");
                fprintf(stderr, "  -r    Reemplaza el archivo fuente original con el prettyprint.\n");
                fprintf(stderr, "  -g    Aplica el formato de Estilo GNU.\n");
                fprintf(stderr, "  -b    Aplica el formato de Estilo BSD (Allman).\n\n");
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
    char *archivoParaEscanear = NULL;

    if (activarPreproceso) {
        printf("Ejecutando Preprocesador...\n");

        FILE *f_temp = fopen(archivoTemporal, "w");
        if (!f_temp) {
            perror("Error al crear el archivo temporal");
            exit(EXIT_FAILURE);
        }

        procesarArchivo(archivoFuente, f_temp);
        fclose(f_temp);

        archivoParaEscanear = archivoTemporal;
    } else {
        printf("Preprocesador desactivado.\n");
        archivoParaEscanear = archivoFuente;
    }

    printf("Pasando archivo procesado al Scanner (Flex)...\n");
    yyin = fopen(archivoParaEscanear, "r");
    if (!yyin) {
        perror("Error al abrir el archivo temporal");
        return EXIT_FAILURE;
    }

    printf("Iniciando análisis sintáctico con Bison...\n");

    if (yyparse() == 0 && totalErrores == 0 && raizAST != NULL) {

        char archivoSalida[512];
        if (reemplazarOriginal) {
            strcpy(archivoSalida, archivoFuente);
        } else {
            snprintf(archivoSalida, sizeof(archivoSalida), "%s.pretty", archivoFuente);
        }

        FILE *fOut = fopen(archivoSalida, "w");
        if (!fOut) {
            perror("Error al crear el archivo de salida formateado");
            fclose(yyin);
            return 1;
        }

        prettyPrintAST(raizAST, fOut, 0);
        fclose(fOut);
        printf(" Prettyprint generado exitosamente en: %s\n", archivoSalida);

    } else {
        fprintf(stderr, "Error sintáctico: No se pudo generar la reimpresión.\n");
    }

    if (totalErrores > 0) {
        fprintf(stderr, "\nCompilación fallida: se encontraron %d error(es).\n", totalErrores);
        return EXIT_FAILURE;
    } else {
        printf("Compilación exitosa sin errores.\n");
        return EXIT_SUCCESS;
    }

    fclose(yyin);

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

        // Omite contenido entre comillas
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
    int numLinea = 0;

    while (fgets(linea, sizeof(linea), f_in)) {
        numLinea++;

        if (strncmp(linea, "#include", 8) == 0) {
            char subArchivo[MaxNombre] = "";

            if (sscanf(linea, "#include \"%[^\"]\"", subArchivo) == 1) {
                procesarArchivo(subArchivo, f_out);
                continue;
            }
            else if (sscanf(linea, "#include <%[^>]>", subArchivo) == 1) {
                continue;
            } else {
                fprintf(stderr, "Error de preprocesamiento: Directiva #include vacía o mal formada en '%s' (línea %d): %s", nombreArchivo, numLinea, linea);
                exit(EXIT_FAILURE);
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

// Imprime los espacios correspondientes al nivel de indentación
void imprimirIndentacion(FILE *f, int nivel) {
    int espacios = 0;
    switch (estilo) {
        case estiloGNU:
            espacios = nivel * 2;
            break;
        case estiloBSD:
            espacios = nivel * 8;
            break;
        case propio:
        default:
            espacios = nivel * 4;
            break;
    }
    for (int i = 0; i < espacios; i++) {
        fputc(' ', f);
    }
}

// Maneja el prettyprint para la declaración de variables
void printDeclaracionVariable(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "declaracion-multiple") == 0) {
        imprimirIndentacion(f, nivel);
        // Los especificadores
        if (nodo->izq) {
            printEspecificadores(nodo->izq, f);
            fprintf(f, " ");
        }
        // Lista de identificadores
        printListaIds(nodo->der, f);
        fprintf(f, ";\n");
        return;
    }

    imprimirIndentacion(f, nivel);

    // Caso 1: Declaración con inicialización o asignación
    if (nodo->izq != NULL && nodo->der != NULL) {
        struct ASTNodo *cabecera = nodo->izq;

        if (cabecera->der != NULL) {
            printEspecificadores(cabecera->der, f);
            fprintf(f, " ");
        }

        if (cabecera->izq && cabecera->izq->valorLexema) {
            fprintf(f, "%s", cabecera->izq->valorLexema);
        } else if (cabecera->valorLexema) {
            fprintf(f, "%s", cabecera->valorLexema);
        }

        if (nodo->valorLexema && strcmp(nodo->valorLexema, "array-init") == 0) {
            fprintf(f, "[] = { ");
            prettyPrintAST(nodo->der, f, 0);
            fprintf(f, " };\n");
        } else {
            fprintf(f, " = ");
            prettyPrintAST(nodo->der, f, 0);
            fprintf(f, ";\n");
        }
    }
    // Caso 2: Declaración sin asignación
    else if (nodo->izq != NULL) {
        struct ASTNodo *idNodo = nodo->izq;
        struct ASTNodo *tipoNodo = nodo->der;

        if (tipoNodo) {
            printEspecificadores(tipoNodo, f);
            fprintf(f, " ");
        }

        if (nodo->valorLexema && strcmp(nodo->valorLexema, "array") == 0) {
            fprintf(f, "%s[", idNodo->valorLexema ? idNodo->valorLexema : "");
            if (idNodo->der) {
                prettyPrintAST(idNodo->der, f, 0);
            }
            fprintf(f, "];\n");
        } else {
            fprintf(f, "%s;\n", idNodo->valorLexema ? idNodo->valorLexema : "var");
        }
    }
    else {
        fprintf(f, "%s;\n", nodo->valorLexema ? nodo->valorLexema : "var");
    }
}

// Maneja el prettyprint para las lista de identificadores
void printListaIds(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-ids") == 0) {
        printListaIds(nodo->izq, f);
        fprintf(f, ", ");
        printListaIds(nodo->der, f);
    }
    else if (nodo->valorLexema && strcmp(nodo->valorLexema, "var-simple") == 0) {
        if (nodo->izq && nodo->izq->valorLexema) {
            fprintf(f, "%s", nodo->izq->valorLexema);
        }
    }
    else if (nodo->valorLexema && strcmp(nodo->valorLexema, "var-init") == 0) {
        prettyPrintAST(nodo->izq, f, 0);
    }
}

// Maneja el prettyprint para las condiciones if
void printCondicionIf(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "if-else") == 0) {
        printCondicionIf(nodo->izq, f, nivel);

        if (estilo == estiloBSD) {
            fprintf(f, " else");
        } else if (estilo == estiloGNU) {
            imprimirIndentacion(f, nivel);
            fprintf(f, "else");
        } else { // estilo propio
            fprintf(f, " else");
        }

        if (estilo == estiloBSD) {
            fprintf(f, " {\n");
        } else if (estilo == estiloGNU) {
            fprintf(f, "\n");
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "{\n");
        } else { // estilo propio
            fprintf(f, " {\n");
        }

        // Cuerpo del else
        prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

        // Cierre de llave del else
        if (estilo == estiloGNU) {
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "}\n");
        } else {
            imprimirIndentacion(f, nivel);
            fprintf(f, "}\n");
        }
        return;
    }

    // Caso: if simple
    imprimirIndentacion(f, nivel);
    fprintf(f, "if (");

    prettyPrintAST(nodo->izq, f, 0);
    fprintf(f, ")");

    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, " {\n");
    }

    // Bloque del cuerpo del if
    prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

    // Cierre de llave del if
    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}");
    }
}

// Maneja el prettyprint para un loop do-while
void printLoopDoWhile(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);
    fprintf(f, "do");

    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, " {\n");
    }

    // Cuerpo del do-while
    prettyPrintAST(nodo->izq, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

    // Inicio de la condición while
    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "} while (");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "} while (");
    }

    // Condición del bucle
    prettyPrintAST(nodo->der, f, 0);
    fprintf(f, ");\n");
}

// Maneja el prettyprint para un loop do-while
void printLoopWhile(struct ASTNodo *nodo, FILE *f, int nivel) {
    imprimirIndentacion(f, nivel);
    fprintf(f, "while (");
    prettyPrintAST(nodo -> izq, f, 0);
    fprintf(f, ")");

    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else {
        fprintf(f, " {\n");
    }

    prettyPrintAST(nodo -> der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}\n");
    }
}

// Maneja el prettyprint para la declaración un bucle for
void printDeclaracionFor(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    if (nodo->izq != NULL && nodo->der != NULL) {
        struct ASTNodo *cabecera = nodo->izq;

        // Tipo y modificadores
        if (cabecera->der != NULL && cabecera->der->tipo == nodoExpresion) {
            if (cabecera->der->izq) {
                prettyPrintAST(cabecera->der->izq, f, 0);
                fprintf(f, " ");
            }
            if (cabecera->der->valorLexema) {
                fprintf(f, "%s ", cabecera->der->valorLexema);
            }
        }

        // Nombre de la variable
        if (cabecera->izq && cabecera->izq->valorLexema) {
            fprintf(f, "%s", cabecera->izq->valorLexema);
        } else if (cabecera->valorLexema) {
            fprintf(f, "%s", cabecera->valorLexema);
        }

        fprintf(f, " = ");
        prettyPrintAST(nodo->der, f, 0);
    }
    else if (nodo->izq != NULL) {
        struct ASTNodo *idNodo = nodo->izq;
        struct ASTNodo *tipoNodo = nodo->der;

        if (tipoNodo && tipoNodo->valorLexema) {
            fprintf(f, "%s ", tipoNodo->valorLexema);
        }
        fprintf(f, "%s", idNodo->valorLexema ? idNodo->valorLexema : "var");
    }
}

// Maneja el prettyprint para el bucle for
void printLoopFor(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);
    fprintf(f, "for (");

    //La cabecera del for
    struct ASTNodo *cabecera = nodo->izq;
    if (cabecera) {
        // Inicialización
        if (cabecera->izq) {
            if (cabecera->izq->tipo == nodoDeclaracionVariable) {
                printDeclaracionFor(cabecera->izq, f);
            } else {
                prettyPrintAST(cabecera->izq, f, 0);
            }
        }
        fprintf(f, "; ");

        struct ASTNodo *condInc = cabecera->der;
        if (condInc) {
            // Condición
            if (condInc->izq) {
                prettyPrintAST(condInc->izq, f, 0);
            }
            fprintf(f, "; ");

            // Incremento
            if (condInc->der) {
                prettyPrintAST(condInc->der, f, 0);
            }
        }
    }

    fprintf(f, ")");

    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, " {\n");
    }

    // Cuerpo del bucle
    if (nodo->der) {
        prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);
    }

    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}\n");
    }
}

// Maneja el prettyprint para un caso del switch
void printCaso(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);

    // Caso: case <elemento>:
    if (nodo->izq) {
        fprintf(f, "case ");
        prettyPrintAST(nodo->izq, f, 0);
        fprintf(f, ":\n");
    }
    // Caso: default:
    else {
        fprintf(f, "default:\n");
    }

    if (nodo->der) {
        prettyPrintAST(nodo->der, f, nivel + 1);
    }
}

// Maneja el prettyprint para imprimir una lista de casos
void printListaCasos(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    if (nodo->tipo == nodoListaCasos || (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-casos") == 0)) {
        if (nodo->izq) printListaCasos(nodo->izq, f, nivel);
        if (nodo->der) printListaCasos(nodo->der, f, nivel);
    }

    else if (nodo->tipo == nodoCaso) {
        printCaso(nodo, f, nivel);
    }
    else {
        prettyPrintAST(nodo, f, nivel);
    }
}

// Maneja el prettyprint para imprimir la sentencia switch
void printCaseSwitch(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);
    fprintf(f, "switch (");

    prettyPrintAST(nodo->izq, f, 0);
    fprintf(f, ")");

    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, " {\n");
    }

    // Determina el nivel de sangría interno
    int nivelCasos = (estilo == estiloGNU) ? nivel + 2 : nivel + 1;

    if (nodo->der) {
        printListaCasos(nodo->der, f, nivelCasos);
    }

    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}\n");
    }
}

// Maneja el prettyprint para imprimir una expresión
void printExpresion(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "()") == 0) {
        if (nivel > 0) imprimirIndentacion(f, nivel);
        fprintf(f, "(");
        prettyPrintAST(nodo->izq, f, 0);
        fprintf(f, ")");
        if (nivel > 0) fprintf(f, ";\n");
        return;
    }

    if (nodo->tipo == nodoLlamadaFuncion) {
        if (nivel > 0) imprimirIndentacion(f, nivel);
        printLlamadaFuncion(nodo, f);
        if (nivel > 0) fprintf(f, ";\n");
        return;
    }

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "casteo") == 0) {
        fprintf(f, "(");
        printEspecificadores(nodo->izq, f); // Imprime el tipo
        fprintf(f, ") ");
        prettyPrintAST(nodo->der, f, 0);   // Imprime la expresión casteada
        return;
    }

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "break") == 0) {
        if (nivel > 0) imprimirIndentacion(f, nivel);
        fprintf(f, "break;\n");
        return;
    }

    if (nivel > 0) {
        imprimirIndentacion(f, nivel);
    }

    if (nodo->izq != NULL && nodo->der != NULL) {
        // Expresión binaria
        prettyPrintAST(nodo->izq, f, 0);
        fprintf(f, " %s ", nodo->valorLexema ? nodo->valorLexema : "op");
        prettyPrintAST(nodo->der, f, 0);
    } else if (nodo->izq != NULL) {
        // Return
        if (nodo->valorLexema && strcmp(nodo->valorLexema, "return") == 0) {
            fprintf(f, "return ");
        } else if (nodo->valorLexema) {
            fprintf(f, "%s ", nodo->valorLexema);
        }
        prettyPrintAST(nodo->izq, f, 0);
    } else {
        //  Identificador o literal numérico
        if (nodo->valorLexema != NULL) {
            fprintf(f, "%s", nodo->valorLexema);
        } else {
            fprintf(f, "%.0f", nodo->valorNumerico);
        }
    }

    if (nivel > 0) {
        fprintf(f, ";\n");
    }
}

// Maneja el prettyprint para imprimir los parámetros
void printParametros(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-parametros") == 0) {
        printParametros(nodo->izq, f);
        fprintf(f, ", ");
        printParametros(nodo->der, f);
    }
    else {
            if (nodo->izq) {
                printEspecificadores(nodo->izq, f);
                fprintf(f, " ");
            }

            if (nodo->valorLexema) {
                fprintf(f, "%s", nodo->valorLexema);
            }
        }
}

// Maneja el prettyprint para imprimir los argumentos
void printArgumentos(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "arg") == 0) {
        printArgumentos(nodo->izq, f);
        fprintf(f, ", ");
        printArgumentos(nodo->der, f);
    } else {
        prettyPrintAST(nodo, f, 0);
    }
}

// Maneja el prettyprint para imprimir la llamada a una función
void printLlamadaFuncion(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    fprintf(f, "%s(", nodo->valorLexema ? nodo->valorLexema : "funcion");

    if (nodo->izq) {
        printArgumentos(nodo->izq, f);
    }

    fprintf(f, ")");
}

// Maneja el prettyprint para imprimir una función
void printFuncion(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);

    // Cabecera (Tipo, Nombre y Parámetros)
    if (nodo->izq) {
        struct ASTNodo *cabecera = nodo->izq;
        struct ASTNodo *nodoTipo = cabecera->izq;

        if (nodoTipo) {
            printEspecificadores(nodoTipo, f);
            fprintf(f, " ");
        }

        fprintf(f, "%s(", nodo->valorLexema ? nodo->valorLexema : "funcion");

        if (cabecera->der) {
            printParametros(cabecera->der, f);
        }
        fprintf(f, ")");
    } else {
        fprintf(f, "%s()", nodo->valorLexema ? nodo->valorLexema : "funcion");
    }

    if (nodo->der == NULL) {
        fprintf(f, ";\n");
    } else {
        if (estilo == estiloBSD) {
            fprintf(f, " {\n");
        } else if (estilo == estiloGNU) {
            fprintf(f, "\n");
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "{\n");
        } else { // Estilo Propio
            fprintf(f, " {\n");
        }

        prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

        if (estilo == estiloGNU) {
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "}\n\n");
        } else {
            imprimirIndentacion(f, nivel);
            fprintf(f, "}\n\n");
        }
    }
}

// Maneja el prettyprint para imprimir los modificadores
void printEspecificadores(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    if (nodo->izq) {
        printEspecificadores(nodo->izq, f);
        fprintf(f, " ");
    }
    if (nodo->valorLexema) {
        fprintf(f, "%s", nodo->valorLexema);
    }
}

// Maneja el prettyprint
void prettyPrintAST(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (nodo == NULL || f == NULL){
        return;
    }

    switch (nodo -> tipo) {

        case nodoLlamadaFuncion:
            printLlamadaFuncion(nodo, f);
            break;

        case nodoFuncion:
            printFuncion(nodo, f, nivel);
            break;

        case nodoDeclaracionVariable:
            printDeclaracionVariable(nodo, f, nivel);
            break;

        case nodoCondicionIf:
            printCondicionIf(nodo, f, nivel);
            break;

        case nodoLoopWhile:
            printLoopWhile(nodo, f, nivel);
            break;

        case nodoLoopDoWhile:
            printLoopDoWhile(nodo, f, nivel);
            break;

        case nodoLoopFor:
            printLoopFor(nodo, f, nivel);
            break;

        case nodoExpresion:
            printExpresion(nodo, f, nivel);
            break;

        case nodoCaseSwitch:
            printCaseSwitch(nodo, f, nivel);
            break;

        case nodoListaCasos:
            printListaCasos(nodo, f, nivel);
            break;

        case nodoCaso:
            printCaso(nodo, f, nivel);
            break;

        default:
            prettyPrintAST(nodo -> izq, f, nivel);
            prettyPrintAST(nodo -> der, f, nivel);
            break;
    }
}
