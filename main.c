#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "compilador.h"

Macro macros[MaxMacros];
int totalMacros = 0;

FILE *yyin;

int main(int argc, char *argv[]) {
    int opt;
    int activarPreproceso = 1; // El preproceso está activado por defecto.
    char *archivoFuente = NULL;
    char *archivoSalida = NULL;

    while ((opt = getopt(argc, argv, "po:"))) {
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

            if (token.lexema && strcmp(token.lexema, "EOF") != 0) {
                free(token.lexema);
            }
        }
    } while (token.tipo != TokenEOF);

    fclose(yyin);

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
        case TokenString: return "STRING";
        case TokenChar: return "CHAR";
        case TokenOp: return "OPERADOR";
        case TokenSeparador: return "SEPARADOR";
        case TokenError: return "ERROR";
        default: return "DESCONOCIDO";
    }
}