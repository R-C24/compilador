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

    //REVISAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAR
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
            //REVISAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAR
            // fprintf(stderr, "Uso: %s [-p] [-r] [-g|-b] <archivo.c>\n", argv[0]);
                printf("Error: No es una opción válida. Utilice '-p' para activar el preproceso, "
                       "'-r nombreArchivo' REVISAAAAAAAAAR BIEN DESPUÉS QUÉ HACIA, '-g' para utilizar estilo GNU o '-b' para utilizar el estilo BSD  \n");
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

    if (yyparse() == 0 && raizAST != NULL) {

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

        // 4. Generar la reimpresión Prettyprint en el archivo
        prettyPrintAST(raizAST, fOut, 0);
        fclose(fOut);
        printf(" Prettyprint generado exitosamente en: %s\n", archivoSalida);

    } else {
        fprintf(stderr, "Error sintáctico: No se pudo generar la reimpresión.\n");
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

void imprimirAST(struct ASTNodo *nodo, int nivel) {
    if (!nodo) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("Tipo: %d, Lexema: %s, Valor: %.2f\n", nodo -> tipo, nodo -> valorLexema ? nodo -> valorLexema : "N/A", nodo -> valorNumerico);
    imprimirAST(nodo -> izq, nivel + 1);
    imprimirAST(nodo -> der, nivel + 1);
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

void printDeclaracionVariable(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;
    imprimirIndentacion(f, nivel);

    if (nodo->izq != NULL && nodo->der != NULL) {
        struct ASTNodo *cabecera = nodo->izq;

        if (cabecera->der != NULL && cabecera->der->tipo == nodoExpresion) {
            if (cabecera->der->izq) {
                prettyPrintAST(cabecera->der->izq, f, 0);
                fprintf(f, " ");
            }
            if (cabecera->der->valorLexema) {
                fprintf(f, "%s ", cabecera->der->valorLexema);
            }
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

    else if (nodo->izq != NULL) {
        struct ASTNodo *idNodo = nodo->izq;
        struct ASTNodo *tipoNodo = nodo->der;

        if (tipoNodo && tipoNodo->izq) {
            prettyPrintAST(tipoNodo->izq, f, 0);
            fprintf(f, " ");
        }

        if (tipoNodo && tipoNodo->valorLexema) {
            fprintf(f, "%s ", tipoNodo->valorLexema);
        }

        if (nodo->valorLexema && strcmp(nodo->valorLexema, "array") == 0) {
            fprintf(f, "%s[", idNodo->valorLexema ? idNodo->valorLexema : "");
            if (idNodo->der) {
                prettyPrintAST(idNodo->der, f, 0); // Tamaño del arreglo
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

void printCondicionIf(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    // Detectar si es un nodo combinativo if-else
    // Si nodo->valorLexema es "if-else", nodo->izq es el sub-nodo IF y nodo->der es el ELSE
    if (nodo->valorLexema && strcmp(nodo->valorLexema, "if-else") == 0) {
        // 1. Imprimir la parte del IF
        printCondicionIf(nodo->izq, f, nivel);

        // 2. Imprimir el ELSE
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

        // Cuerpo del ELSE
        prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

        // Cierre de llave del ELSE
        if (estilo == estiloGNU) {
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "}\n");
        } else {
            imprimirIndentacion(f, nivel);
            fprintf(f, "}\n");
        }
        return;
    }

    // --- Caso Normal: IF Simple ---
    imprimirIndentacion(f, nivel);
    fprintf(f, "if (");

    // Imprimir la condición evaluada (en línea recta)
    prettyPrintAST(nodo->izq, f, 0);
    fprintf(f, ")");

    // Apertura de llaves según el estilo
    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, " {\n");
    }

    // Bloque del cuerpo del IF
    prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

    // Cierre de llave del IF
    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}");
    }
}

void printLoopDoWhile(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);
    fprintf(f, "do");

    // Apertura de llaves según el estilo
    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel);
        fprintf(f, "{\n");
    }

    // 1. Cuerpo del do-while (ubicado en nodo->izq)
    prettyPrintAST(nodo->izq, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

    // 2. Cierre de llaves e inicio de la condición while
    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "} while (");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "} while (");
    }

    // 3. Condición del bucle (ubicada en nodo->der)
    prettyPrintAST(nodo->der, f, 0);
    fprintf(f, ");\n");
}

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

void printLoopFor(struct ASTNodo *nodo, FILE *f, int nivel) {
    imprimirIndentacion(f, nivel);
    fprintf(f, "for (");
    // nodo->izq contiene cabeceraFor (inicialización y condición/incremento)
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

// Imprime un caso individual (case X: o default:)
void printCaso(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);

    // Caso 1: case <elemento>:
    if (nodo->izq) {
        fprintf(f, "case ");
        prettyPrintAST(nodo->izq, f, 0);
        fprintf(f, ":\n");
    }
    // Caso 2: default:
    else {
        fprintf(f, "default:\n");
    }

    // Imprimir el bloque de código del caso con un nivel más de sangría
    if (nodo->der) {
        prettyPrintAST(nodo->der, f, nivel + 1);
    }
}

// Recorre la lista de casos dentro del switch
void printListaCasos(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    // Si es un nodo contenedor de lista de casos
    if (nodo->tipo == nodoListaCasos || (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-casos") == 0)) {
        if (nodo->izq) printListaCasos(nodo->izq, f, nivel);
        if (nodo->der) printListaCasos(nodo->der, f, nivel);
    }
    // Si es un caso individual
    else if (nodo->tipo == nodoCaso) {
        printCaso(nodo, f, nivel);
    }
    else {
        prettyPrintAST(nodo, f, nivel);
    }
}

// Función Principal para la sentencia switch
void printCaseSwitch(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);
    fprintf(f, "switch (");

    // Imprimir la expresión dentro del switch (ej. opcion)
    prettyPrintAST(nodo->izq, f, 0);
    fprintf(f, ")");

    // Apertura de llaves según el estilo seleccionado
    if (estilo == estiloBSD) {
        fprintf(f, " {\n");
    } else if (estilo == estiloGNU) {
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "{\n");
    } else { // estilo propio
        fprintf(f, "\n");
        imprimirIndentacion(f, nivel);
        fprintf(f, "{\n");
    }

    // Determinar el nivel de sangría interno para los casos
    int nivelCasos = (estilo == estiloGNU) ? nivel + 2 : nivel + 1;

    // Imprimir los casos contenidos en el hijo derecho
    if (nodo->der) {
        printListaCasos(nodo->der, f, nivelCasos);
    }

    // Cierre de llaves según el estilo
    if (estilo == estiloGNU) {
        imprimirIndentacion(f, nivel + 1);
        fprintf(f, "}\n");
    } else {
        imprimirIndentacion(f, nivel);
        fprintf(f, "}\n");
    }
}

void printExpresion(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "break") == 0) {
        if (nivel > 0) imprimirIndentacion(f, nivel);
        fprintf(f, "break;\n");
        return;
    }

    // Si la expresión está al nivel de instrucción (nivel > 0), se indenta
    if (nivel > 0) {
        imprimirIndentacion(f, nivel);
    }

    if (nodo->izq != NULL && nodo->der != NULL) {
        // Expresión binaria (ej. x = x + 5 o x < y)
        prettyPrintAST(nodo->izq, f, 0);
        fprintf(f, " %s ", nodo->valorLexema ? nodo->valorLexema : "op");
        prettyPrintAST(nodo->der, f, 0);
    } else if (nodo->izq != NULL) {
        // Unaria / Return
        if (nodo->valorLexema && strcmp(nodo->valorLexema, "return") == 0) {
            fprintf(f, "return ");
        } else if (nodo->valorLexema) {
            fprintf(f, "%s ", nodo->valorLexema);
        }
        prettyPrintAST(nodo->izq, f, 0);
    } else {
        // Hoja: Identificador o Literal numérico
        if (nodo->valorLexema != NULL) {
            fprintf(f, "%s", nodo->valorLexema);
        } else {
            fprintf(f, "%.0f", nodo->valorNumerico);
        }
    }

    // Si era una sentencia/instrucción independiente dentro de un bloque, rematar con ';' y '\n'
    if (nivel > 0) {
        fprintf(f, ";\n");
    }
}

void printParametros(struct ASTNodo *nodo, FILE *f) {
    if (!nodo) return;

    // Si es un nodo de unión de lista ("lista-parametros")
    if (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-parametros") == 0) {
        // Imprimir parámetro o sublista izquierda
        printParametros(nodo->izq, f);

        // Imprimir coma separadora
        fprintf(f, ", ");

        // Imprimir parámetro derecho
        printParametros(nodo->der, f);
    }
    else {
        // Caso: Nodo de un parámetro individual
        // nodo->izq contiene el nodoTipo (ej. "int")
        if (nodo->izq) {
            // Si tiene modificador (ej. "const" en nodo->izq->izq)
            if (nodo->izq->izq && nodo->izq->izq->valorLexema) {
                fprintf(f, "%s ", nodo->izq->izq->valorLexema);
            }
            // Tipo de dato (ej. "int")
            if (nodo->izq->valorLexema) {
                fprintf(f, "%s ", nodo->izq->valorLexema);
            }
        }

        // Nombre de la variable/parámetro (ej. "opcion")
        if (nodo->valorLexema) {
            fprintf(f, "%s", nodo->valorLexema);
        }
    }
}

void printFuncion(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (!nodo) return;

    imprimirIndentacion(f, nivel);

    // 1. Cabecera (Tipo, Nombre y Parámetros)
    if (nodo->izq) {
        struct ASTNodo *cabecera = nodo->izq;
        struct ASTNodo *nodoTipo = cabecera->izq;

        if (nodoTipo && nodoTipo->valorLexema) {
            fprintf(f, "%s ", nodoTipo->valorLexema);
        }

        fprintf(f, "%s(", nodo->valorLexema ? nodo->valorLexema : "funcion");

        if (cabecera->der) {
            printParametros(cabecera->der, f);
        }
        fprintf(f, ")");
    } else {
        fprintf(f, "%s()", nodo->valorLexema ? nodo->valorLexema : "funcion");
    }

    // 2. Prototipo vs Cuerpo
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

        // Llamada recursiva al cuerpo (nodoBloqueCodigo)
        prettyPrintAST(nodo->der, f, (estilo == estiloGNU) ? nivel + 2 : nivel + 1);

        // Cierre de llaves
        if (estilo == estiloGNU) {
            imprimirIndentacion(f, nivel + 1);
            fprintf(f, "}\n\n");
        } else {
            imprimirIndentacion(f, nivel);
            fprintf(f, "}\n\n");
        }
    }
}

// Maneja el prettyprint
void prettyPrintAST(struct ASTNodo *nodo, FILE *f, int nivel) {
    if (nodo == NULL || f == NULL){
        return;
    }

    switch (nodo -> tipo) {

        // case nodoPrograma:
        //     prettyPrintAST(nodo->izq, f, nivel);
        //     prettyPrintAST(nodo->der, f, nivel);
        //     break;
        // case nodoBloqueCodigo:
        //     prettyPrintAST(nodo->izq, f, nivel);
        //     prettyPrintAST(nodo->der, f, nivel);
        //     break;
        // case nodoRelleno:
        //     prettyPrintAST(nodo->izq, f, nivel);
        //     prettyPrintAST(nodo->der, f, nivel);
        //     break;

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

        // Recorrido secuencial de listas/bloques
        default:
            prettyPrintAST(nodo -> izq, f, nivel);
            prettyPrintAST(nodo -> der, f, nivel);
            break;
    }
}
