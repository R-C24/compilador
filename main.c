#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "compilador.h"


Macro macros[MaxMacros];
int totalMacros = 0;

extern FILE *yyin;
extern void yyrestart(FILE *new_file);
extern struct ASTNodo *raizAST;
extern int yyparse();
const char *archivoFuente = NULL;

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 0; // El preproceso está desactivado por defecto.
    int reemplazarOriginal = 0; // Está desactivado por defecto.
    while ((opt = getopt(argc, argv, "prgb")) != -1) {
        switch (opt) {
            case 'p':
                activarPreproceso = 1;
                break;
            case 'r':
                reemplazarOriginal = 1;
                break;
                break;
            default:
                fprintf(stderr, "\nError: no es una opción válida.\n");
                fprintf(stderr, "Uso correcto: %s [-p] [-r] [-g|-b] <archivo.c>\n\n", argv[0]);
                fprintf(stderr, "Opciones disponibles:\n");
                fprintf(stderr, "  -p    Activa el preprocesador opcional.\n");
                fprintf(stderr, "  -r    Reemplaza el archivo fuente original con el prettyprint.\n");
                fprintf(stderr, "  -g    Aplica el formato de Estilo GNU.\n");
                fprintf(stderr, "  -b    Aplica el formato de Estilo BSD.\n\n");
                return EXIT_FAILURE;
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
                fputc('\n', f_out);
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
