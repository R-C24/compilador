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

    if (yyparse() == 0) {
        printf("\n¡Análisis sintáctico completado con éxito!\n");
        printf("========================================\n");
        printf("         ESTRUCTURA DEL AST             \n");
        printf("========================================\n");

        imprimirAST(raizAST, 0);

    } else {
        fprintf(stderr, "El análisis falló debido a errores sintácticos.\n");
    }

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

/* Traduce el nombre del tipo del token a texto para imprimir.
char* obtenerNombreToken(TipoToken tipo) {
    switch (tipo) {
        case TokenID: return "IDENTIFICADOR";
        case TokenInt: return "INT";
        case TokenFloat: return "FLOAT";
        case TokenChar: return "CHAR";
        case TokenString: return "STRING";
        //case TokenOp: return "OPERADOR";
        case TokenSeparador: return "SEPARADOR";
        case TokenModificador: return "MODIFICADOR";
        case TokenTipoDato: return "TIPO DE DATO";
        case TokenError: return "ERROR";
        default: return "DESCONOCIDO";
    }
}*/

void imprimirAST(struct ASTNodo *nodo, int nivel) {
    if (!nodo) return;
    for (int i = 0; i < nivel; i++) printf("  ");
    printf("Tipo: %d, Lexema: %s, Valor: %.2f\n", nodo->tipo, nodo->valorLexema ? nodo->valorLexema : "N/A", nodo->valorNumerico);
    imprimirAST(nodo->izq, nivel + 1);
    imprimirAST(nodo->der, nivel + 1);
}
