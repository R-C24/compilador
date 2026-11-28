#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "compilador.h"


Macro macros[MaxMacros];
int totalMacros = 0;

LiteralCadena tablaCadenas[MaxString];
int totalCadenas = 0;

LiteralFlotante tablaFlotantes[MaxFloats];
int totalFlotantes = 0;

TablaSimbolos pilaTablas[MaxAmbitos];
int topePila = -1;
int topeLoops = -1;
char pilaBreak[MaxLoops][32];
char pilaContinue[MaxLoops][32];

const char *RegArgs32[6] = {"%edi", "%esi", "%edx", "%ecx", "%r8d", "%r9d"};
const char *RegArgs64[6] = {"%rdi", "%rsi", "%rdx", "%rcx", "%r8", "%r9"};

char funcionActual[256] = "";
int contextoImpreso = 0;

char *tiposUsuario[MaxTipos];
int totalTiposUsuario = 0;

char *tiposEstandar[] = {
    "FILE", "DIR", "size_t", "ssize_t", "ptrdiff_t", "wchar_t", "time_t", "clock_t",
    "off_t", "pid_t", "va_list", "bool", "int8_t", "int16_t", "int32_t", "int64_t",
    "uint8_t", "uint16_t", "uint32_t", "uint64_t", "intptr_t", "uintptr_t", NULL
};

int totalErrores = 0;
char tipoActual[256] = "int";

char archivosEnProceso[MaxProfundidadInclude][MaxNombre];
int profundidadInclude = 0;

FILE *salidaTablas;

extern FILE *yyin;
extern int yyleng;
extern void yyrestart(FILE *new_file);
extern struct ASTNodo *raizAST;
extern int yyparse();

int contadorTemporales = 0;
int contadorEtiquetas = 0;
int offsetPilaActual = 0;

FILE *archivoASM = NULL;
int errorGeneracion = 0;

int offsetFuncionActual = 0;
RegistroSemantico *historicoSimbolos[4096];
int totalHistoricoSimbolos = 0;

const char *archivoFuente = NULL;

int main(int argc, char *argv[]) {

    int opt;
    int activarPreproceso = 0; // El preproceso está desactivado por defecto.
    int ejecutarBinario = 0;
    char *archivoEnsamblador = NULL;

    while ((opt = getopt(argc, argv, "pS:e")) != -1) {
        switch (opt) {
            case 'p':
                activarPreproceso = 1;
                break;
            case 'S':
                archivoEnsamblador = optarg;
                break;
            case 'e':
                ejecutarBinario = 1;
                break;
            default:
                fprintf(stderr, "\nError: no es una opción válida.\n");
                fprintf(stderr, "Uso: %s [-p] [-S archivo.s] [-e] <archivo.c>\n", argv[0]);
                fprintf(stderr, "Opciones:\n");
                fprintf(stderr, "  -p            Activa el preprocesador (desactivado por defecto).\n");
                fprintf(stderr, "  -S <archivo>  Escribe el ensamblador en el archivo indicado.\n");
                fprintf(stderr, "  -e            Ensambla con gcc y ejecuta el código generado.\n");
                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        archivoFuente = argv[optind];
    } else {
        fprintf(stderr, "Error: Falta el archivo fuente .c de entrada.\n");

        fprintf(stderr, "Uso: %s [-p] [-S archivo.s] [-e] <archivo.c>\n", argv[0]);
        fprintf(stderr, "Opciones:\n");
        fprintf(stderr, "  -p            Activa el preprocesador (desactivado por defecto).\n");
        fprintf(stderr, "  -S <archivo>  Escribe el ensamblador en el archivo indicado.\n");
        fprintf(stderr, "  -e            Ensambla con gcc y ejecuta el código generado.\n");
        
        return EXIT_FAILURE;
    }

    if (access(archivoFuente, F_OK) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe o no tiene acceso.\n", archivoFuente);
        return EXIT_FAILURE;
    }

    // Determinar nombre del .s (por defecto <fuente>.s)
    char rutaSalidaS[512];
    if (archivoEnsamblador != NULL) {
        strncpy(rutaSalidaS, archivoEnsamblador, sizeof(rutaSalidaS) - 1);
        rutaSalidaS[sizeof(rutaSalidaS) - 1] = '\0';
    } else {
        obtenerNombreSalidaS(archivoFuente, rutaSalidaS, sizeof(rutaSalidaS));
    }

    const char *archivoTemporal = "archivoTemp.tmp";
    const char *archivoParaEscanear = archivoFuente;

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
    }

    printf("Pasando archivo procesado al Scanner (Flex)...\n");
    yyin = fopen(archivoParaEscanear, "r");
    if (!yyin) {
        perror("Error al abrir el archivo temporal");
        return EXIT_FAILURE;
    }

    printf("Iniciando análisis y construcción de tabla de símbolos...\n");
    inicializarPila();
    int resultadoParse = yyparse();

    popAmbito();

    fclose(yyin);
    if (activarPreproceso) {
        remove(archivoTemporal);
    }

    if (resultadoParse != 0 || totalErrores > 0 || raizAST == NULL) {
        fprintf(stderr, "\nCompilación fallida: se encontraron %d error(es). No se generó código.\n", totalErrores);
        return EXIT_FAILURE;
    }

    if (!generarCodigo(raizAST, rutaSalidaS)) {
        fprintf(stderr, "\nError durante la generación de código. El archivo '%s' fue eliminado.\n", rutaSalidaS);
        return EXIT_FAILURE;
    }
    liberarSimbolos();

    printf("Código ensamblador x86 generado en: %s\n", rutaSalidaS);

    printf("Código ensamblador x86 generado en: %s\n", rutaSalidaS);

    // Bandera -e: Ensamblar con gcc y ejecutar
    if (ejecutarBinario) {
        char comando[1024];
        const char *binarioSalida = "./salida_compilador";

        snprintf(comando, sizeof(comando), "gcc -no-pie %s -o %s", rutaSalidaS, binarioSalida);
        if (system(comando) != 0) {
            fprintf(stderr, "Error: gcc falló al ensamblar '%s'.\n", rutaSalidaS);
            return EXIT_FAILURE;
        }

        int estado = system(binarioSalida);
        printf("\nPrograma ejecutado (código de retorno: %d)\n", WEXITSTATUS(estado));
    }
    
    return EXIT_SUCCESS;

}

// Función auxiliar para agregar una macro simple.
void agregarMacro(char *nombre, char *valor) {
    agregarMacroParam(nombre, "", valor);
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

    while (traducido && limitePasadas < 25) {
        traducido = 0;
        for (int i = 0; i < totalMacros; i++) {
            char *pos = buscarMacroValida(linea, macros[i].nombre);
            if (pos == NULL) continue;

            int antes = (int)(pos - linea);
            size_t lenNombre = strlen(macros[i].nombre);

            // Macro con 1 parámetro
            if (macros[i].parametro[0] != '\0') {
                char *p = pos + lenNombre;
                while (*p == ' ' || *p == '\t') p++;
                if (*p != '(' ) continue; 
                p++;
                char *inicioArg = p;
                int nivelParen = 1;
                while (*p != '\0' && nivelParen > 0) {
                    if (*p == '(') nivelParen++;
                    else if (*p == ')') nivelParen--;
                    if (nivelParen > 0) p++;
                }
                if (nivelParen != 0) continue; // Paréntesis sin cerrar

                char argReal[MaxValor];
                size_t lenArg = (size_t)(p - inicioArg);
                if (lenArg >= sizeof(argReal)) lenArg = sizeof(argReal) - 1;
                strncpy(argReal, inicioArg, lenArg);
                argReal[lenArg] = '\0';

                char expansion[MaxValor];
                sustituirParametroEnCuerpo(macros[i].valor, macros[i].parametro, argReal, expansion, sizeof(expansion));

                snprintf(resultado, sizeof(resultado), "%.*s%s%s",
                         antes, linea, expansion, p + 1);
                strcpy(linea, resultado);
                traducido = 1;
                break;
            }
            // Macro simple sin parámetros
            else {
                snprintf(resultado, sizeof(resultado), "%.*s%s%s",
                         antes, linea, macros[i].valor, pos + lenNombre);
                strcpy(linea, resultado);
                traducido = 1;
                break;
            }
        }
        limitePasadas++;
    }
}

int macroDefinida(const char *nombre) {
    for (int i = 0; i < totalMacros; i++) {
        if (strcmp(macros[i].nombre, nombre) == 0) return 1;
    }
    return 0;
}

// Agrega la información de un nuevo macro al "diccionario" de macros.
void agregarMacroParam(const char *nombre, const char *parametro, const char *valor) {
    // Si ya existía, sobrescribirla
    for (int i = 0; i < totalMacros; i++) {
        if (strcmp(macros[i].nombre, nombre) == 0) {
            strncpy(macros[i].parametro, parametro ? parametro : "", MaxNombre - 1);
            macros[i].parametro[MaxNombre - 1] = '\0';
            strncpy(macros[i].valor, valor ? valor : "", MaxValor - 1);
            macros[i].valor[MaxValor - 1] = '\0';
            return;
        }
    }

    if (totalMacros < MaxMacros) {
        strncpy(macros[totalMacros].nombre, nombre, MaxNombre - 1);
        macros[totalMacros].nombre[MaxNombre - 1] = '\0';
        strncpy(macros[totalMacros].parametro, parametro ? parametro : "", MaxNombre - 1);
        macros[totalMacros].parametro[MaxNombre - 1] = '\0';
        strncpy(macros[totalMacros].valor, valor ? valor : "", MaxValor - 1);
        macros[totalMacros].valor[MaxValor - 1] = '\0';
        totalMacros++;
    }
}

// Reemplaza todas las ocurrencias de 'param' (como palabra completa) en 'cuerpo' por 'arg'
void sustituirParametroEnCuerpo(const char *cuerpo, const char *param, const char *arg, char *salida, size_t maxLen) {
    salida[0] = '\0';
    size_t lenParam = strlen(param);
    size_t outIdx = 0;

    for (size_t i = 0; cuerpo[i] != '\0' && outIdx < maxLen - 1; ) {
        if (strncmp(&cuerpo[i], param, lenParam) == 0) {
            int limIzq = (i == 0) || !esCaracterIdentificador(cuerpo[i - 1]);
            int limDer = !esCaracterIdentificador(cuerpo[i + lenParam]);
            if (limIzq && limDer) {
                size_t lenArg = strlen(arg);
                if (outIdx + lenArg < maxLen - 1) {
                    strcpy(&salida[outIdx], arg);
                    outIdx += lenArg;
                }
                i += lenParam;
                continue;
            }
        }
        salida[outIdx++] = cuerpo[i++];
    }
    salida[outIdx] = '\0';
}

void recortarDerecha(char *str) {
    size_t len = strlen(str);
    while (len > 0 && (str[len - 1] == '\n' || str[len - 1] == '\r' || str[len - 1] == ' ' || str[len - 1] == '\t')) {
        str[--len] = '\0';
    }
}

// Abre el archivo, revisa y procesa recursivamente los include y los define.
void procesarArchivo(char *nombreArchivo, FILE *f_out) {

    if (esInclusionCircular(nombreArchivo)) {
        fprintf(stderr, "Error de preprocesamiento: Inclusión circular detectada con '%s'.\n", nombreArchivo);
        if (f_out) fclose(f_out);
        remove("archivoTemp.tmp");
        exit(EXIT_FAILURE);
    }

    if (profundidadInclude >= MaxProfundidadInclude) {
        fprintf(stderr, "Error de preprocesamiento: Profundidad máxima de #include excedida en '%s'.\n", nombreArchivo);
        if (f_out) fclose(f_out);
        remove("archivoTemp.tmp");
        exit(EXIT_FAILURE);
    }

    FILE *f_in = fopen(nombreArchivo, "r");
    if (!f_in) {
        fprintf(stderr, "Error: No se pudo abrir el archivo fuente '%s'.\n", nombreArchivo);
        if (f_out) fclose(f_out);
        remove("archivoTemp.tmp");
        exit(EXIT_FAILURE);
    }

    strncpy(archivosEnProceso[profundidadInclude], nombreArchivo, MaxNombre - 1);
    archivosEnProceso[profundidadInclude][MaxNombre - 1] = '\0';
    profundidadInclude++;

    char linea[TamBuffer];
    int numLinea = 0;
    int enCondicional = 0;
    int ignorarBloque = 0;

    while (fgets(linea, sizeof(linea), f_in)) {
        numLinea++;

        char *ptr = linea;
        while (*ptr == ' ' || *ptr == '\t') ptr++;

        if (strncmp(ptr, "#ifdef", 6) == 0 && !esCaracterIdentificador(ptr[6])) {
            char sim[MaxNombre] = "";
            sscanf(ptr + 6, " %s", sim);
            enCondicional = 1;
            ignorarBloque = !macroDefinida(sim);
            fputc('\n', f_out);
            continue;
        }
        if (strncmp(ptr, "#ifndef", 7) == 0 && !esCaracterIdentificador(ptr[7])) {
            char sim[MaxNombre] = "";
            sscanf(ptr + 7, " %s", sim);
            enCondicional = 1;
            ignorarBloque = macroDefinida(sim);
            fputc('\n', f_out);
            continue;
        }
        if (strncmp(ptr, "#else", 5) == 0 && !esCaracterIdentificador(ptr[5])) {
            if (enCondicional) {
                ignorarBloque = !ignorarBloque;
            }
            fputc('\n', f_out);
            continue;
        }
        if (strncmp(ptr, "#endif", 6) == 0 && !esCaracterIdentificador(ptr[6])) {
            enCondicional = 0;
            ignorarBloque = 0;
            fputc('\n', f_out);
            continue;
        }

        // Si estamos dentro de una rama falsa de #ifdef/#else, emitir '\n' y saltar la línea
        if (ignorarBloque) {
            fputc('\n', f_out);
            continue;
        }

        if (strncmp(ptr, "#include", 8) == 0 && !esCaracterIdentificador(ptr[8])) {
            char subArchivo[MaxNombre] = "";
            if (sscanf(ptr + 8, " \"%[^\"]\"", subArchivo) == 1) {
                procesarArchivo(subArchivo, f_out);
                fputc('\n', f_out);
                continue;
            } else if (sscanf(ptr + 8, " <%[^>]>", subArchivo) == 1) {
                fputc('\n', f_out);
                continue;
            } else {
                fprintf(stderr, "Error de preprocesamiento: Directiva #include vacía o mal formada en '%s' (línea %d): %s",
                        nombreArchivo, numLinea, linea);
                fclose(f_in);
                exit(EXIT_FAILURE);
            }
        }

        if (strncmp(ptr, "#define", 7) == 0 && !esCaracterIdentificador(ptr[7])) {
            char *p = ptr + 7;
            while (*p == ' ' || *p == '\t') p++;

            char nombre[MaxNombre] = "";
            char param[MaxNombre] = "";
            char valor[MaxValor] = "";

            int k = 0;
            while (esCaracterIdentificador(*p) && k < MaxNombre - 1) {
                nombre[k++] = *p++;
            }
            nombre[k] = '\0';

            if (*p == '(') {
                p++;
                while (*p == ' ' || *p == '\t') p++;
                int j = 0;
                while (esCaracterIdentificador(*p) && j < MaxNombre - 1) {
                    param[j++] = *p++;
                }
                param[j] = '\0';
                while (*p != '\0' && *p != ')') p++;
                if (*p == ')') p++;
            }

            // Lo que queda después de espacios es el cuerpo (puede ser vacío "")
            while (*p == ' ' || *p == '\t') p++;
            strncpy(valor, p, MaxValor - 1);
            valor[MaxValor - 1] = '\0';
            recortarDerecha(valor);

            if (nombre[0] != '\0') {
                agregarMacroParam(nombre, param, valor);
            }

            fputc('\n', f_out);
            continue;
        }

        traducirMacros(linea);
        fputs(linea, f_out);
    }

    profundidadInclude--;
    fclose(f_in);
}


// Manejo de pila

// Inicializa la pila global al arrancar el compilador
void inicializarPila() {
    topePila = -1;
    pushAmbito();
}

// Agrega a la pila una nueva tabla de símbolos al entrar a un nuevo contexto
void pushAmbitoConContexto(const char *funcion, const char *parametros) {
    if (funcion != NULL) {
        offsetFuncionActual = 0;
    }

    if (topePila >= MaxAmbitos - 1) {
        fprintf(stderr, "Error: Desbordamiento de pila de ámbitos.\n");
        return;
    }

    topePila++;
    pilaTablas[topePila].raiz = NULL;
    pilaTablas[topePila].nombreFuncion = funcion ? strdup(funcion) : NULL;
    pilaTablas[topePila].parametrosFuncion = parametros ? strdup(parametros) : NULL;
}

void pushAmbito() {
    pushAmbitoConContexto(NULL, NULL);
}

// Saca de la pila la tabla de símbolos al salir de un contexto
void popAmbito() {
    if (topePila < 0) return;
        
    // Liberar memoria de los registros del ámbito que se cierra
    
    pilaTablas[topePila].raiz = NULL;

    if (pilaTablas[topePila].nombreFuncion) {
        free(pilaTablas[topePila].nombreFuncion);
        pilaTablas[topePila].nombreFuncion = NULL;
    }
    if (pilaTablas[topePila].parametrosFuncion) {
        free(pilaTablas[topePila].parametrosFuncion);
        pilaTablas[topePila].parametrosFuncion = NULL;
    }

    topePila--;
}

// Inserta un nuevo registro semántico en el ámbito actual
RegistroSemantico* insertarSimbolo(char *lexema, char *tipoDato, CategoriaSimbolo categoria, int linea, int columna) {
    if (topePila < 0) {
        return NULL;
    }
    // Verificar si ya existe en el ámbito local actual para evitar duplicados
    RegistroSemantico *actual = pilaTablas[topePila].raiz;
    while (actual != NULL) {
        if (strcmp(actual->lexema, lexema) == 0) {
            return actual; // Error: Ya declarado en este contexto
        }
        actual = actual->siguiente;
    }

    // Crear el nuevo registro
    RegistroSemantico *nuevo = (RegistroSemantico*)malloc(sizeof(RegistroSemantico));
    nuevo->lexema = strdup(lexema);
    nuevo->tipoDato = strdup(tipoDato);
    nuevo->categoria = categoria;
    nuevo->nivelAmbito = topePila;
    nuevo->lineaDeclaracion = linea;
    nuevo->columnaDeclaracion = columna;
    nuevo->esPrototipo = 0;

    // Asignación automática de offset para variables locales y parámetros
    if (topePila > 0 && (categoria == tipoVariable || categoria == tipoParametro)) {
        int es8Bytes = (strchr(tipoDato, '*') != NULL || strstr(tipoDato, "double") != NULL);
        if (es8Bytes) {
            if (offsetFuncionActual % 8 != 0) {
                offsetFuncionActual += (8 - (offsetFuncionActual % 8));
            }
            offsetFuncionActual += 8;
        } else {
            offsetFuncionActual += 4;
        }
        nuevo->offset = offsetFuncionActual;
    } else {
        nuevo->offset = 0;
    }

    if (totalHistoricoSimbolos < 4096) {
    historicoSimbolos[totalHistoricoSimbolos++] = nuevo;
    }
    
    // Insertar al inicio de la lista enlazada del ámbito actual
    nuevo->siguiente = pilaTablas[topePila].raiz;
    pilaTablas[topePila].raiz = nuevo;

    return NULL;
}

// Busca un símbolo en toda la pila activa
RegistroSemantico* buscarSimbolo(char *lexema) {
    // Recorrer la pila desde arriba hacia abajo
    for (int i = topePila; i >= 0; i--) {
        RegistroSemantico *actual = pilaTablas[i].raiz;
        while (actual != NULL) {
            if (strcmp(actual->lexema, lexema) == 0) {
                return actual;
            }
            actual = actual->siguiente;
        }
    }
    return NULL;
}


// Imprime la tabla actual al cerrar un contexto
void imprimirTablaActual() {
    if (topePila < 0) {
        return;
    }

    FILE *f_salida = (salidaTablas != NULL) ? salidaTablas : stdout;

    fprintf(f_salida, "+-------------------------------------------------------------------+\n");
    fprintf(f_salida, "| TABLA DE SIMBOLOS - AMBITO NIVEL %-32d |\n", topePila);

    // 1. Metadatos de función o contexto activo
    if (pilaTablas[topePila].nombreFuncion != NULL) {
        fprintf(f_salida, "+-------------------------------------------------------------------+\n");
        fprintf(f_salida, "| Funcion   : %-53s |\n", pilaTablas[topePila].nombreFuncion);
        fprintf(f_salida, "| Parametros: %-53s |\n", 
                (pilaTablas[topePila].parametrosFuncion && strlen(pilaTablas[topePila].parametrosFuncion) > 0) 
                ? pilaTablas[topePila].parametrosFuncion 
                : "ninguno");
    } 
    else if (strlen(funcionActual) > 0) {
        fprintf(f_salida, "+-------------------------------------------------------------------+\n");
        fprintf(f_salida, "| Contexto  : dentro de %-43s |\n", funcionActual);
    }

    // 2. Recolectar los símbolos visibles para invertir el orden LIFO a cronológico
    RegistroSemantico *visibles[512];
    int totalVisibles = 0;
    RegistroSemantico *actual = pilaTablas[topePila].raiz;

    while (actual != NULL && totalVisibles < 512) {
        // Filtrar símbolos fantasma de recuperación de errores
        if (actual->tipoDato && strcmp(actual->tipoDato, "int_undeclared") == 0) {
            actual = actual->siguiente;
            continue;
        }
        // En el ámbito global (Nivel 0), mostrar únicamente variables
        if (topePila == 0 && actual->categoria == tipoFuncion) {
            actual = actual->siguiente;
            continue;
        }

        visibles[totalVisibles++] = actual;
        actual = actual->siguiente;
    }

    // 3. Imprimir tabla en formato de caja
    fprintf(f_salida, "+--------------------+----------------+---------------+-------------+\n");
    fprintf(f_salida, "| %-18s | %-14s | %-13s | %-11s |\n", "Nombre", "Tipo", "Categoria", "Ambito");
    fprintf(f_salida, "+--------------------+----------------+---------------+-------------+\n");

    if (totalVisibles == 0) {
        fprintf(f_salida, "| %-65s |\n", "(Tabla vacia)");
    } else {
        // Recorrer de atrás hacia adelante para respetar el orden de declaración
        for (int i = totalVisibles - 1; i >= 0; i--) {
            RegistroSemantico *sim = visibles[i];
            char nivelStr[16];
            snprintf(nivelStr, sizeof(nivelStr), "Nivel %d", sim->nivelAmbito);

            fprintf(f_salida, "| %-18s | %-14s | %-13s | %-11s |\n",
                    sim->lexema,
                    sim->tipoDato ? sim->tipoDato : "desconocido",
                    obtenerNombreCategoria(sim->categoria),
                    nivelStr);
        }
    }

    fprintf(f_salida, "+--------------------+----------------+---------------+-------------+\n\n");
}

void reportarRedeclaracion(const char *archivo, const char *lexema, const char *tipoDato,
                              int lineaErr, int colErr, int lenErr,
                              int lineaNota, int colNota, int lenNota) {
                              
    totalErrores++; // Sumar al conteo global

    // 1. Imprimir el contexto de la función (solo una vez por bloque de errores)
    if (strlen(funcionActual) > 0 && !contextoImpreso) {
        fprintf(stderr, "%s: In function ‘%s’:\n", archivo, funcionActual);
        contextoImpreso = 1;
    }

    // 2. Imprimir el ERROR principal
    fprintf(stderr, "%s:%d:%d: \033[1;31merror:\033[0m redeclaration of ‘%s’ with no linkage\n", 
            archivo, lineaErr, colErr, lexema);
    imprimirLineaErrorGCC(archivo, lineaErr, colErr, lenErr); // Usa tu función existente

    // 3. Imprimir la NOTA (ubicación previa)
    fprintf(stderr, "%s:%d:%d: \033[1;36mnote:\033[0m previous definition of ‘%s’ with type ‘%s’\n", 
            archivo, lineaNota, colNota, lexema, tipoDato);
    imprimirLineaErrorGCC(archivo, lineaNota, colNota, lenNota);
}

void validarYRegistrarSimbolo(const char *lexema, const char *tipoDetectado, CategoriaSimbolo cat, int lineaAct, int columnaAct) {
    RegistroSemantico *previo = insertarSimbolo(lexema, tipoDetectado, cat, lineaAct, columnaAct);
    
    if (previo != NULL) {
        int lenActual = strlen(lexema);
        int lenPrevio = strlen(previo->lexema);
        
        reportarRedeclaracion(archivoFuente, lexema, previo->tipoDato,
                             lineaAct, columnaAct, lenActual,
                             previo->lineaDeclaracion, previo->columnaDeclaracion, lenPrevio);
    }
}

void validarYRegistrarFuncion(const char *lexema, const char *tipoRet, int esDefinicion, int linea, int col) {
    if (!lexema || topePila < 0) return;

    const char *tipoSeguro = (tipoRet && tipoRet[0] != '\0') ? tipoRet : "int";
    RegistroSemantico *previo = buscarEnAmbitoActual(lexema);

    if (previo != NULL) {
        const char *tipoPrevio = previo->tipoDato ? previo->tipoDato : "int";

        if (previo->categoria == tipoFuncion && esDefinicion == 0 && strcmp(tipoPrevio, tipoSeguro) == 0) {
            return;
        }

        // Caso válido, ya existía como prototipo y ahora llega su definición con cuerpo
        if (previo->categoria == tipoFuncion && previo->esPrototipo == 1 && esDefinicion == 1) {
            previo->esPrototipo = 0;
            previo->lineaDeclaracion = linea;
            previo->columnaDeclaracion = col;
            return;
        }

        int lenActual = (int)strlen(lexema);
        int lenPrevio = previo->lexema ? (int)strlen(previo->lexema) : lenActual;

        reportarRedeclaracion(archivoFuente ? archivoFuente : "entrada.c",
                              lexema, tipoPrevio,
                              linea, col, lenActual,
                              previo->lineaDeclaracion, previo->columnaDeclaracion, lenPrevio);
        return;
    }

    // No existía: insertarla como tipoFuncion
    insertarSimbolo((char*)lexema, (char*)tipoSeguro, tipoFuncion, linea, col);

    RegistroSemantico *nuevo = buscarEnAmbitoActual(lexema);
    if (nuevo != NULL) {
        nuevo->esPrototipo = (esDefinicion == 0) ? 1 : 0;
    }
}


/* Crea un nodo de operador unario prefijo */
struct ASTNodo *crearNodoUnario(const char *op, struct ASTNodo *operando) {
    char buf[64];
    snprintf(buf, sizeof(buf), "unario:%s", op);
    return crearNodo(nodoExpresion, operando, NULL, buf);
}

struct ASTNodo* crearNodo(TipoNodo tipo, struct ASTNodo *izq, struct ASTNodo *der, char *lexema) {
    struct ASTNodo *nuevo = (struct ASTNodo*) malloc(sizeof(struct ASTNodo));
    if (!nuevo) {
        fprintf(stderr, "Error: Memoria agotada al crear nodo del AST\n");
        exit(1);
    }
    nuevo->tipo = tipo;
    nuevo->izq = izq;
    nuevo->der = der;
    nuevo->valorLexema = lexema ? strdup(lexema) : NULL;
    nuevo->valorNumerico = 0.0;
    nuevo->simbolo = NULL;
    nuevo->bytesLocales = 0;

    if (izq == NULL && der == NULL && lexema != NULL) {
        nuevo->simbolo = buscarSimbolo(lexema);
    }

    if (tipo == nodoFuncion) {
        nuevo->bytesLocales = offsetFuncionActual;
    }

    return nuevo;
}

void imprimirLineaErrorGCC(const char *archivo, int numLinea, int numColumna, int longitudToken) {
    FILE *f = fopen(archivo, "r");
    if (!f) return;

    char lineaBuffer[1024];
    int lineaActual = 1;

    while (fgets(lineaBuffer, sizeof(lineaBuffer), f)) {
        if (lineaActual == numLinea) {
            lineaBuffer[strcspn(lineaBuffer, "\r\n")] = 0;

            fprintf(stderr, " %5d | %s\n", numLinea, lineaBuffer);
            fprintf(stderr, "       | ");

            for (int i = 1; i < numColumna; i++) {
                if (lineaBuffer[i - 1] == '\t') {
                    fputc('\t', stderr);
                } else {
                    fputc(' ', stderr);
                }
            }

            fputc('^', stderr);
            for (int i = 1; i < longitudToken; i++) {
                fputc('~', stderr);
            }
            fputc('\n', stderr);

            break;
        }
        lineaActual++;
    }
    fclose(f);
}

int esTipoUsuario(char *s) {
    for (int i = 0; tiposEstandar[i]; i++){
        if (strcmp(s, tiposEstandar[i]) == 0) {
            return 1;
        }
    }
    for (int i = 0; i < totalTiposUsuario; i++) {
        if (strcmp(s, tiposUsuario[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

void registrarTipo(char *s) {
    if (!s || esTipoUsuario(s) || totalTiposUsuario >= MaxTipos) {
        return;
    }
    tiposUsuario[totalTiposUsuario++] = strdup(s);
}

// Registra los nombres declarados en un typedef (la lista de ids)
void registrarTipoDeclarado(struct ASTNodo *n) {
    if (!n) {
        return;
    }
    if (n->valorLexema && strcmp(n->valorLexema, "lista-ids") == 0) {
        registrarTipoDeclarado(n->izq);
        registrarTipoDeclarado(n->der);
        return;
    }
    struct ASTNodo *h = n;
    while (h && h->izq) h = h->izq; // el nombre esta al fondo de la rama izquierda
    if (h && h->valorLexema) {
        registrarTipo(h->valorLexema);
    }
}

// Función recursiva para extraer el texto legible desde el nodo AST de especificadores
void obtenerTextoTipo(struct ASTNodo *nodo, char *buffer, size_t tam) {
    if (!nodo) {
        snprintf(buffer, tam, "desconocido");
        return;
    }

    const char *tag = nodo->valorLexema;

    // Structs y Unions
    if (tag && (strcmp(tag, "struct") == 0 || strcmp(tag, "def-struct") == 0)) {
        if (nodo->izq && nodo->izq->valorLexema)
            snprintf(buffer, tam, "struct %s", nodo->izq->valorLexema);
        else
            snprintf(buffer, tam, "struct");
        return;
    }
    if (tag && strcmp(tag, "def-struct-anon") == 0) {
        snprintf(buffer, tam, "struct");
        return;
    }
    if (tag && (strcmp(tag, "union") == 0 || strcmp(tag, "def-union") == 0)) {
        if (nodo->izq && nodo->izq->valorLexema)
            snprintf(buffer, tam, "union %s", nodo->izq->valorLexema);
        else
            snprintf(buffer, tam, "union");
        return;
    }
    if (tag && strcmp(tag, "def-union-anon") == 0) {
        snprintf(buffer, tam, "union");
        return;
    }

    // Enums
    if (tag && (strcmp(tag, "enum") == 0 || strcmp(tag, "def-enum") == 0)) {
        if (nodo->izq && nodo->izq->valorLexema)
            snprintf(buffer, tam, "enum %s", nodo->izq->valorLexema);
        else
            snprintf(buffer, tam, "enum");
        return;
    }
    if (tag && strcmp(tag, "def-enum-anon") == 0) {
        snprintf(buffer, tam, "enum");
        return;
    }

    // Nodos compuestos con modificador en izq y tipo real en der
    if (tag && (strcmp(tag, "mod-struct") == 0 || strcmp(tag, "mod-union") == 0 ||
                strcmp(tag, "mod-enum") == 0   || strcmp(tag, "mod-tipo") == 0)) {
        char bufMod[128] = "";
        char bufTipo[128] = "";
        if (nodo->izq) obtenerTextoTipo(nodo->izq, bufMod, sizeof(bufMod));
        if (nodo->der) obtenerTextoTipo(nodo->der, bufTipo, sizeof(bufTipo));
        snprintf(buffer, tam, "%s %s", bufMod, bufTipo);
        return;
    }

    // Modificadores encadenados sobre tipo base
    if (nodo->izq && tag) {
        char bufIzq[128];
        obtenerTextoTipo(nodo->izq, bufIzq, sizeof(bufIzq));
        snprintf(buffer, tam, "%s %s", bufIzq, tag);
        return;
    }

    // Nodo hoja simple
    if (tag) {
        snprintf(buffer, tam, "%s", tag);
    } else {
        snprintf(buffer, tam, "tipo");
    }
}

// Reporta variables no declaradas estilo GCC
void reportarUsoNoDeclarado(const char *archivo, const char *lexema, int lineaErr, int colErr, int lenErr) {
    totalErrores++;

    if (strlen(funcionActual) > 0 && !contextoImpreso) {
        fprintf(stderr, "%s: In function ‘%s’:\n", archivo, funcionActual);
        contextoImpreso = 1;
    }

    fprintf(stderr, "%s:%d:%d: \033[1;31merror:\033[0m ‘%s’ undeclared (first use in this function)\n", 
            archivo, lineaErr, colErr, lexema);
    imprimirLineaErrorGCC(archivo, lineaErr, colErr, lenErr);

    fprintf(stderr, "%s:%d:%d: \033[1;36mnote:\033[0m each undeclared identifier is reported only once for each function it appears in\n", 
            archivo, lineaErr, colErr);
}

// Función auxiliar para validar expresiones
void validarUsoSimbolo(const char *lexema, int lineaAct, int columnaAct) {
    if (esConstanteEstandar(lexema)) {
        return;
    }

    RegistroSemantico *encontrado = buscarSimbolo((char *)lexema);
    if (encontrado == NULL) {
        int lenActual = (int)strlen(lexema);
        reportarUsoNoDeclarado(archivoFuente, lexema, lineaAct, columnaAct, lenActual);

        int nivelFuncion = (topePila >= 1) ? 1 : topePila;
        RegistroSemantico *nuevo = (RegistroSemantico *)malloc(sizeof(RegistroSemantico));
        nuevo->lexema = strdup(lexema);
        nuevo->tipoDato = strdup("int_undeclared");
        nuevo->categoria = tipoVariable;
        nuevo->nivelAmbito = nivelFuncion;
        nuevo->lineaDeclaracion = lineaAct;
        nuevo->columnaDeclaracion = columnaAct;
        nuevo->esPrototipo = 0;
        nuevo->siguiente = pilaTablas[nivelFuncion].raiz;
        pilaTablas[nivelFuncion].raiz = nuevo;
    }
}

// Verifica si la función pertenece a la biblioteca estándar de C
int esFuncionEstandar(const char *lexema) {
    const char *funcs[] = {
        // <stdio.h>
        "printf", "fprintf", "sprintf", "snprintf", "scanf", "fscanf", "sscanf",
        "fopen", "fclose", "fread", "fwrite", "fgets", "fputs", "fgetc", "fputc",
        "getchar", "putchar", "puts", "perror", "fflush", "fseek", "ftell", "rewind",
        // <stdlib.h>
        "malloc", "calloc", "realloc", "free", "exit", "abort", "atoi", "atof", "atol",
        "strtol", "strtod", "rand", "srand", "qsort", "bsearch", "system", "getenv",
        // <string.h>
        "strlen", "strcpy", "strncpy", "strcat", "strncat", "strcmp", "strncmp",
        "strchr", "strrchr", "strstr", "strtok", "memcpy", "memmove", "memset", "memcmp",
        // <ctype.h> / <math.h>
        "isalpha", "isdigit", "isalnum", "isspace", "toupper", "tolower",
        "sqrt", "pow", "sin", "cos", "tan", "abs", "fabs", "ceil", "floor",
        NULL
    };
    for (int i = 0; funcs[i] != NULL; i++) {
        if (strcmp(lexema, funcs[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Verifica constantes y flujos globales estándar
int esConstanteEstandar(const char *lexema) {
    const char *constantes[] = {
        "NULL", "EOF", "EXIT_SUCCESS", "EXIT_FAILURE",
        "true", "false", "stdin", "stdout", "stderr", NULL
    };
    for (int i = 0; constantes[i] != NULL; i++) {
        if (strcmp(lexema, constantes[i]) == 0) {
            return 1;
        }
    }
    return 0;
}

// Valida llamadas a funciones: ignora las estándar y valida las declaradas por el usuario
void validarLlamadaFuncion(const char *lexema, int lineaAct, int columnaAct) {
    if (esFuncionEstandar(lexema)) {
        return; // Válida, pero nunca entra a la tabla de símbolos
    }

    // Buscar si existe como función o prototipo en la pila (Nivel 0)
    RegistroSemantico *encontrado = buscarSimbolo((char *)lexema);
    if (encontrado == NULL) {
        int lenActual = (int)strlen(lexema);
        reportarUsoNoDeclarado(archivoFuente, lexema, lineaAct, columnaAct, lenActual);

        // Insertar fantasma en el ámbito local de la función actual para evitar errores repetidos
        int nivelFuncion = (topePila >= 1) ? 1 : topePila;
        RegistroSemantico *nuevo = (RegistroSemantico *)malloc(sizeof(RegistroSemantico));
        nuevo->lexema = strdup(lexema);
        nuevo->tipoDato = strdup("int_undeclared");
        nuevo->categoria = tipoFuncion;
        nuevo->nivelAmbito = nivelFuncion;
        nuevo->lineaDeclaracion = lineaAct;
        nuevo->columnaDeclaracion = columnaAct;
        nuevo->esPrototipo = 0;
        nuevo->siguiente = pilaTablas[nivelFuncion].raiz;
        pilaTablas[nivelFuncion].raiz = nuevo;
    }
}

void obtenerTextoParametrosAux(struct ASTNodo *nodo, char *buffer, size_t tam) {
    if (!nodo) return;

    // Si es un nodo de unión/lista de parámetros (ej. con lexema "lista-parametros" o ",")
    if (nodo->valorLexema && (strcmp(nodo->valorLexema, "lista-parametros") == 0 || strcmp(nodo->valorLexema, ",") == 0)) {
        obtenerTextoParametrosAux(nodo->izq, buffer, tam);
        if (strlen(buffer) > 0) {
            strncat(buffer, ", ", tam - strlen(buffer) - 1);
        }
        obtenerTextoParametrosAux(nodo->der, buffer, tam);
    } 
    else {
        // Es un parámetro individual
        char tempTipo[128] = "";
        char tempParam[256] = "";

        // Si tiene especificadores de tipo (nodo->izq)
        if (nodo->izq) {
            obtenerTextoTipo(nodo->izq, tempTipo, sizeof(tempTipo));
        }

        // Parámetro puntero con nombre (1 o más '*', con o sin []) 
        if (nodo->valorLexema && strcmp(nodo->valorLexema, "parametro-puntero") == 0 && nodo->der) {
            struct ASTNodo *nodoPtrs = nodo->der;
            int esArray = 0;

            // Si tiene dimensiones (parametro-array), bajar un nivel hacia "param-punteros"
            if (nodoPtrs->valorLexema && strcmp(nodoPtrs->valorLexema, "parametro-array") == 0) {
                esArray = 1;
                nodoPtrs = nodoPtrs->izq;
            }

            int numPtrs = 1;
            const char *nombreReal = "";

            if (nodoPtrs && nodoPtrs->valorLexema && strcmp(nodoPtrs->valorLexema, "param-punteros") == 0) {
                numPtrs = contarPunteros(nodoPtrs->izq);
                if (nodoPtrs->der && nodoPtrs->der->valorLexema) {
                    nombreReal = nodoPtrs->der->valorLexema;
                }
            }

            char asteriscos[32] = "";
            for (int i = 0; i < numPtrs && i < 31; i++) {
                asteriscos[i] = '*';
            }
            asteriscos[numPtrs < 31 ? numPtrs : 31] = '\0';

            snprintf(tempParam, sizeof(tempParam), "%s %s%s%s",
                     tempTipo, asteriscos, nombreReal, esArray ? "[]" : "");
        }
        // Parámetro puntero anónimo (1 o más '*')
        else if (nodo->valorLexema && strcmp(nodo->valorLexema, "parametro-puntero-anon") == 0) {
            int numPtrs = contarPunteros(nodo->der);
            char asteriscos[32] = "";
            for (int i = 0; i < numPtrs && i < 31; i++) {
                asteriscos[i] = '*';
            }
            asteriscos[numPtrs < 31 ? numPtrs : 31] = '\0';

            snprintf(tempParam, sizeof(tempParam), "%s %s", tempTipo, asteriscos);
        }
        // Caso parámetro arreglo sin puntero
        else if (nodo->valorLexema && strcmp(nodo->valorLexema, "parametro") == 0 && nodo->der && nodo->der->izq) {
            snprintf(tempParam, sizeof(tempParam), "%s %s[]", tempTipo, nodo->der->izq->valorLexema);
        }
        // Caso parámetro simple con nombre
        else if (strlen(tempTipo) > 0 && nodo->valorLexema && strcmp(nodo->valorLexema, "parametro-tipo") != 0) {
            snprintf(tempParam, sizeof(tempParam), "%s %s", tempTipo, nodo->valorLexema);
        }
        // Caso parámetro anónimo simple
        else if (strlen(tempTipo) > 0) {
            snprintf(tempParam, sizeof(tempParam), "%s", tempTipo);
        }
        else if (nodo->valorLexema) {
            snprintf(tempParam, sizeof(tempParam), "%s", nodo->valorLexema);
        }

        strncat(buffer, tempParam, tam - strlen(buffer) - 1);
    }
}

// Retorna un string dinámico formateado con los parámetros (el llamador debe liberar la memoria o usarlo en la llamada)
char* obtenerTextoParametros(struct ASTNodo *nodoParams) {
    if (!nodoParams) {
        return strdup("void");
    }
    char buffer[512] = "";
    obtenerTextoParametrosAux(nodoParams, buffer, sizeof(buffer));
    return strdup(strlen(buffer) > 0 ? buffer : "void");
}

// Registra cada parámetro en la tabla de símbolos del ámbito recién abierto
void registrarParametrosEnTabla(struct ASTNodo *nodo) {
    if (!nodo) return;

    if (nodo->valorLexema && (strcmp(nodo->valorLexema, "lista-parametros") == 0 || strcmp(nodo->valorLexema, ",") == 0)) {
        registrarParametrosEnTabla(nodo->izq);
        registrarParametrosEnTabla(nodo->der);
        return;
    }

    // Extrae el tipo base desde nodo->izq
    char tipoParam[128] = "int";
    if (nodo->izq) {
        obtenerTextoTipo(nodo->izq, tipoParam, sizeof(tipoParam));
    }

    if (nodo->valorLexema && strcmp(nodo->valorLexema, "parametro-puntero") == 0 && nodo->der) {
        struct ASTNodo *nodoPtrs = nodo->der;
        if (nodoPtrs->valorLexema && strcmp(nodoPtrs->valorLexema, "parametro-array") == 0) {
            nodoPtrs = nodoPtrs->izq;
        }
        int numPtrs = (nodoPtrs && nodoPtrs->valorLexema && strcmp(nodoPtrs->valorLexema, "param-punteros") == 0)
                      ? contarPunteros(nodoPtrs->izq)
                      : 1;
        if (numPtrs > 0) {
            strncat(tipoParam, " ", sizeof(tipoParam) - strlen(tipoParam) - 1);
            for (int i = 0; i < numPtrs; i++) {
                strncat(tipoParam, "*", sizeof(tipoParam) - strlen(tipoParam) - 1);
            }
        }
    }

    if (nodo->der && nodo->der->valorLexema && strcmp(nodo->der->valorLexema, "parametro-array") == 0) {
        strncat(tipoParam, "[]", sizeof(tipoParam) - strlen(tipoParam) - 1);
    }

    // Extrae el nodo del identificador real, ignora parámetros anónimos
    struct ASTNodo *idNodo = extraerNodoIdParametro(nodo);
    if (idNodo && idNodo->valorLexema) {
        validarYRegistrarSimbolo(idNodo->valorLexema, tipoParam, tipoParametro, idNodo->lineaAct, idNodo->columnaAct);
        
        RegistroSemantico *reg = buscarSimbolo(idNodo->valorLexema);
        idNodo->simbolo = reg;
        nodo->simbolo = reg;
    }
}

// Extrae el identificador real del subárbol de un nodo 
struct ASTNodo* extraerNodoIdParametro(struct ASTNodo *nodo) {
    if (!nodo || !nodo->valorLexema) return NULL;

    // Caso 1: Parámetro simple, el lexema está en la raíz
    if (strcmp(nodo->valorLexema, "parametro") != 0 &&
        strcmp(nodo->valorLexema, "parametro-puntero") != 0 &&
        strcmp(nodo->valorLexema, "parametro-array") != 0 &&
        strcmp(nodo->valorLexema, "parametro-tipo") != 0 &&
        strcmp(nodo->valorLexema, "parametro-puntero-anon") != 0 &&
        strcmp(nodo->valorLexema, "parametro-error") != 0 &&
        strcmp(nodo->valorLexema, "unario:*") != 0 &&
        strcmp(nodo->valorLexema, "*") != 0) {
        return nodo;
    }

    // Caso 2: Parámetro compuesto
    struct ASTNodo *actual = nodo->der;
    while (actual != NULL) {
        // <-- AGREGAR ESTE BLOQUE:
        if (actual->valorLexema && strcmp(actual->valorLexema, "param-punteros") == 0) {
            return actual->der;
        }

        if (actual->valorLexema &&
            strcmp(actual->valorLexema, "parametro-array") != 0 &&
            strcmp(actual->valorLexema, "unario:*") != 0 &&
            strcmp(actual->valorLexema, "*") != 0) {
            return actual; 
        }
        actual = actual->izq;
    }

    return NULL;
}

// Busca un símbolo en el ámbito actual
RegistroSemantico* buscarEnAmbitoActual(const char *lexema) {
    if (!lexema || topePila < 0) return NULL;

    RegistroSemantico *actual = pilaTablas[topePila].raiz;
    while (actual != NULL) {
        if (actual->lexema && strcmp(actual->lexema, lexema) == 0) {
            return actual;
        }
        actual = actual->siguiente;
    }
    return NULL;
}

// Construye el string con el puntero ("int" -> "int *")
char* crearTipoPuntero(const char *tipoBase) {
    char *res = malloc(strlen(tipoBase) + 3);
    if (res) {
        sprintf(res, "%s *", tipoBase);
    }
    return res;
}

// Añade un " *" al último símbolo registrado en el ámbito actual
void agregarPunteroAUltimoSimbolo() {
    if (topePila < 0 || pilaTablas[topePila].raiz == NULL) return;
    RegistroSemantico *ultimo = pilaTablas[topePila].raiz;
    char *nuevoTipo = crearTipoPuntero(ultimo->tipoDato);
    if (nuevoTipo) {
        free(ultimo->tipoDato);
        ultimo->tipoDato = nuevoTipo;
    }
}

// Construye "int[5]" o "int[3][4]" a partir de tipoActual y el nodo listaDimensiones
void formatearTipoConDimensiones(const char *tipoBase, struct ASTNodo *dims, char *salida, size_t tam) {
    snprintf(salida, tam, "%s", tipoBase);
    if (!dims) return;

    if (dims->valorLexema && strcmp(dims->valorLexema, "lista-dimensiones") == 0) {
        formatearTipoConDimensiones(tipoBase, dims->izq, salida, tam);
        char dimActual[32] = "";
        formatearTipoConDimensiones("", dims->der, dimActual, sizeof(dimActual));
        strncat(salida, dimActual, tam - strlen(salida) - 1);
    } else if (dims->valorLexema && strcmp(dims->valorLexema, "dimension") == 0) {
        if (dims->der && dims->der->valorLexema) {
            char buf[32];
            snprintf(buf, sizeof(buf), "[%s]", dims->der->valorLexema);
            strncat(salida, buf, tam - strlen(salida) - 1);
        } else {
            strncat(salida, "[]", tam - strlen(salida) - 1);
        }
    }
}

// Convierte el enum CategoriaSimbolo a texto legible
const char* obtenerNombreCategoria(CategoriaSimbolo cat) {
    switch (cat) {
        case tipoVariable:  return "Variable";
        case tipoFuncion:   return "Funcion";
        case tipoParametro: return "Parametro";
        default:            return "Desconocido";
    }
}

int esInclusionCircular(const char *nombreArchivo) {
    for (int i = 0; i < profundidadInclude; i++) {
        if (strcmp(archivosEnProceso[i], nombreArchivo) == 0) {
            return 1;
        }
    }
    return 0;
}

void limpiarTemporalAlSalir() {
    remove("archivoTemp.tmp");
}

// Helper interno: cuenta cuántos nodos '*' hay en el subárbol 'punteros'
int contarPunteros(struct ASTNodo *nodo) {
    int count = 0;
    while (nodo != NULL && nodo->valorLexema && strcmp(nodo->valorLexema, "*") == 0) {
        count++;
        nodo = nodo->izq;
    }
    return count;
}


// Generación de código

void generate(const char *opcode, const char *arg1, const char *arg2, const char *res) {
    if (!archivoASM) return;

    if (arg1 && strlen(arg1) > 0 && arg2 && strlen(arg2) > 0 && res && strlen(res) > 0) {
        fprintf(archivoASM, "    %s %s, %s, %s\n", opcode, arg1, arg2, res);
    }
    else if (arg1 && strlen(arg1) > 0 && arg2 && strlen(arg2) > 0) {
        fprintf(archivoASM, "    %s %s, %s\n", opcode, arg1, arg2);
    }
    else if (arg1 && strlen(arg1) > 0) {
        fprintf(archivoASM, "    %s %s\n", opcode, arg1);
    }
    else {
        fprintf(archivoASM, "    %s\n", opcode);
    }
}

void resetTemp(void) {
    contadorTemporales = 0;
}

void getTemp(char *dest, size_t tam) {
    contadorTemporales++;
    offsetPilaActual += 4;
    snprintf(dest, tam, "-%d(%%rbp)", offsetPilaActual);
}

void extractExpr(ExprRec e, char *buffer, size_t tam) {
    if (e.kind == LiteralExpr) {
        snprintf(buffer, tam, "$%d", e.valor);
    } else {
        strncpy(buffer, e.direccion, tam - 1);
        buffer[tam - 1] = '\0';
    }
}

const char* extractOp(OpRec op) {
    if (strcmp(op.operador, "+") == 0) return "addl";
    if (strcmp(op.operador, "-") == 0) return "subl";
    if (strcmp(op.operador, "*") == 0) return "imull";
    return "addl";
}

void generateX86(ExprRec e1, OpRec op, ExprRec e2, ExprRec res) {
    char arg1[64], arg2[64], resDir[64];
    extractExpr(e1, arg1, sizeof(arg1));
    extractExpr(e2, arg2, sizeof(arg2));
    extractExpr(res, resDir, sizeof(resDir));

    generate("movl", arg1, "%eax", NULL);

    if (strcmp(op.operador, "/") == 0 || strcmp(op.operador, "%") == 0) {
        generate("cltd", NULL, NULL, NULL);
        if (e2.kind == LiteralExpr) {
            generate("movl", arg2, "%ecx", NULL);
            generate("idivl", "%ecx", NULL, NULL);
        } else {
            generate("idivl", arg2, NULL, NULL);
        }
        generate("movl", (strcmp(op.operador, "%") == 0) ? "%edx" : "%eax", resDir, NULL);
        return;
    }

    const char *opCode = extractOp(op);
    generate(opCode, arg2, "%eax", NULL);
    generate("movl", "%eax", resDir, NULL);
}

ExprRec genInfix(ExprRec e1, OpRec op, ExprRec e2) {
    ExprRec res;
    res.kind = TempExpr;
    res.valor = 0;
    res.esFlotante = 0;
    res.esPuntero = 0;
    snprintf(res.nombre, sizeof(res.nombre), "temp_%d", contadorTemporales + 1);
    getTemp(res.direccion, sizeof(res.direccion));

    generateX86(e1, op, e2, res);
    return res;
}

void assignX86(const char *objDir, RegistroSemantico *simDest, ExprRec fuente) {
    char temp[64];
    extractExpr(fuente, temp, sizeof(temp));

    int destEsDouble = (simDest && simDest->tipoDato && strstr(simDest->tipoDato, "double") != NULL);
    int destEsFloat  = (simDest && simDest->tipoDato && strcmp(simDest->tipoDato, "float") == 0);

    if (destEsDouble || destEsFloat) {
        // 1. Destino flotante (float o double): asegurar que la fuente esté en double (%xmm0)
        ExprRec fDouble = asegurarDouble(fuente);
        char srcFlt[64];
        extractExpr(fDouble, srcFlt, sizeof(srcFlt));
        generate("movsd", srcFlt, "%xmm0", NULL);

        if (destEsFloat) {
            // float en memoria ocupa 4 bytes (32 bits) -> convertir double a float
            generate("cvtsd2ss", "%xmm0", "%xmm0", NULL);
            generate("movss", "%xmm0", objDir, NULL);
        } else {
            // double en memoria ocupa 8 bytes (64 bits)
            generate("movsd", "%xmm0", objDir, NULL);
        }
        return;
    }

    if (fuente.esFlotante) {
        // 2. Destino entero (int) pero fuente flotante -> truncar double a int
        generate("movsd", temp, "%xmm0", NULL);
        generate("cvttsd2si", "%xmm0", "%eax", NULL);
        generate("movl", "%eax", objDir, NULL);
        return;
    }

    // 3. Caso original: destino int y fuente int (32 bits)
    generate("movl", temp, "%eax", NULL);
    generate("movl", "%eax", objDir, NULL);
}

ExprRec processIdNodo(struct ASTNodo *nodo) {
    ExprRec e;
    e.kind = IdExpr;
    e.valor = 0;
    e.esFlotante = 0;
    e.esPuntero = 0;

    RegistroSemantico *info = (nodo != NULL) ? nodo->simbolo : NULL;

    if (info != NULL) {
        strncpy(e.nombre, info->lexema, sizeof(e.nombre) - 1);
        e.nombre[sizeof(e.nombre) - 1] = '\0';

        if (info->tipoDato &&
            (strstr(info->tipoDato, "float") != NULL || strstr(info->tipoDato, "double") != NULL)) {
            e.esFlotante = 1;
        }

        if (info->nivelAmbito == 0) {
            // Variable global en sección .data
            snprintf(e.direccion, sizeof(e.direccion), "%s(%%rip)", info->lexema);
            e.direccion[sizeof(e.direccion) - 1] = '\0';
        } else {
            // Variable local o parámetro en pila respecto a %rbp
            snprintf(e.direccion, sizeof(e.direccion), "-%d(%%rbp)", info->offset);
        }

        if (info->tipoDato &&
            strcmp(info->tipoDato, "float") == 0) {
            ExprRec tmpD;
            tmpD.kind = TempExpr;
            strncpy(tmpD.nombre, "temp_f2d", sizeof(tmpD.nombre) - 1);
            tmpD.nombre[sizeof(tmpD.nombre) - 1] = '\0';
            tmpD.valor = 0;
            tmpD.esFlotante = 1;

            getTempFloat(tmpD.direccion, sizeof(tmpD.direccion));
            generate("cvtss2sd", e.direccion, "%xmm0", NULL);
            generate("movsd", "%xmm0", tmpD.direccion, NULL);
            return tmpD;
        }

    } else {
        fprintf(stderr, "Error en generación: nodo sin símbolo anotado ('%s').\n",
                (nodo && nodo->valorLexema) ? nodo->valorLexema : "desconocido");
        errorGeneracion = 1;
        e.nombre[0] = '\0';
        e.direccion[0] = '\0';
    }

    return e;
}

ExprRec processLiteral(int valor) {
    ExprRec e;
    e.kind = LiteralExpr;
    e.nombre[0] = '\0';
    e.direccion[0] = '\0';
    e.valor = valor;
    e.esFlotante = 0;
    e.esPuntero = 0;
    return e;
}

void readId(ExprRec inVar) {
    generate("leaq", inVar.direccion, "%rsi", NULL);
    generate("leaq", "fmt_in(%rip)", "%rdi", NULL);
    generate("movl", "$0", "%eax", NULL);
    generate("call", "scanf", NULL, NULL);
}

void writeExpr(ExprRec outExpr) {
    char valStr[64];
    extractExpr(outExpr, valStr, sizeof(valStr));

    generate("movl", valStr, "%esi", NULL);
    generate("leaq", "fmt_out(%rip)", "%rdi", NULL);
    generate("movl", "$0", "%eax", NULL);
    generate("call", "printf", NULL, NULL);
}

void start(void) {
    resetTemp();
    fprintf(archivoASM, ".section .data\n");
    fprintf(archivoASM, "    fmt_in:  .string \"%%d\"\n");
    fprintf(archivoASM, "    fmt_out: .string \"%%d\\n\"\n");

    emitirGlobalesAST(raizAST);
    recolectarCadenasAST(raizAST);
    recolectarFlotantesAST(raizAST);

    fprintf(archivoASM, "\n.section .text\n");
    fprintf(archivoASM, ".globl main\n");
}

void finish(struct ASTNodo *raiz) {
    (void)raiz;
    fprintf(archivoASM, "\n.section .note.GNU-stack,\"\",@progbits\n");
}

// Punto de entrada con control de errores y borrado automático del .s
int generarCodigo(struct ASTNodo *raiz, const char *rutaSalidaS) {
    errorGeneracion = 0;

    archivoASM = fopen(rutaSalidaS, "w");
    if (!archivoASM) {
        perror("Error al crear el archivo ensamblador .s");
        return 0;
    }

    start();
    recorrerNodo(raiz);
    finish(raiz);

    fclose(archivoASM);
    archivoASM = NULL;

    // Si ocurrió algún error durante la generación, borramos el .s inmediatamente
    if (errorGeneracion || totalErrores > 0) {
        remove(rutaSalidaS);
        return 0;
    }

    return 1;
}

// Genera "programa.s" a partir de "programa.c" si no se pasó -S
void obtenerNombreSalidaS(const char *fuente, char *destino, size_t tam) {
    strncpy(destino, fuente, tam - 3);
    destino[tam - 3] = '\0';
    char *punto = strrchr(destino, '.');
    if (punto != NULL) {
        *punto = '\0';
    }
    strcat(destino, ".s");
}

// Helper: obtiene el operando listo para ensamblador
void obtenerOperando(ExprRec e, char *dest, size_t tam) {
    if (e.kind == LiteralExpr) {
        snprintf(dest, tam, "$%d", e.valor);
    } else {
        strncpy(dest, e.direccion, tam - 1);
        dest[tam - 1] = '\0';
    }
}

// Helper: evalúa operaciones relacionales (==, !=, <, <=, >, >=) y guarda 0 o 1 en un temporal
ExprRec genRelacional(ExprRec e1, const char *op, ExprRec e2) {
    char arg1[64], arg2[64];
    obtenerOperando(e1, arg1, sizeof(arg1));
    obtenerOperando(e2, arg2, sizeof(arg2));

    ExprRec res;
    res.kind = TempExpr;
    res.valor = 0;
    res.esFlotante = 0;
    res.esPuntero = 0;
    snprintf(res.nombre, sizeof(res.nombre), "temp_rel");
    getTemp(res.direccion, sizeof(res.direccion));

    // En sintaxis AT&T: cmpl arg2, arg1 calcula (arg1 - arg2)
    generate("movl", arg1, "%eax", NULL);
    generate("cmpl", arg2, "%eax", NULL);

    if (strcmp(op, "==") == 0) {
        generate("sete", "%al", NULL, NULL);
    } else if (strcmp(op, "!=") == 0) {
        generate("setne", "%al", NULL, NULL);
    } else if (strcmp(op, "<") == 0) {
        generate("setl", "%al", NULL, NULL);
    } else if (strcmp(op, "<=") == 0) {
        generate("setle", "%al", NULL, NULL);
    } else if (strcmp(op, ">") == 0) {
        generate("setg", "%al", NULL, NULL);
    } else if (strcmp(op, ">=") == 0) {
        generate("setge", "%al", NULL, NULL);
    }

    // Extender %al (8 bits) con ceros a %eax (32 bits) -> resultado exacto 0 o 1
    generate("movzbl", "%al", "%eax", NULL);
    generate("movl", "%eax", res.direccion, NULL);

    return res;
}


// Evalúa recursivamente un nodo de expresión y devuelve dónde quedó el resultado
ExprRec evaluarExpresion(struct ASTNodo *nodo) {
    ExprRec vacio = {LiteralExpr, "", "$0", 0, 0, 0};
    if (!nodo || errorGeneracion) return vacio;

    const char *lex = nodo->valorLexema;

    // 1. Paréntesis: '(' expresion ')' -> nodo->izq
    if (lex && strcmp(lex, "()") == 0) {
        return evaluarExpresion(nodo->izq);
    }

    if (lex && strncmp(lex, "casteo", 6) == 0 && nodo->der != NULL) {
        ExprRec val = evaluarExpresion(nodo->der);
        if (val.esFlotante && nodo->izq && nodo->izq->valorLexema &&
            strcmp(nodo->izq->valorLexema, "int") == 0) {
            char arg[64];
            obtenerOperando(val, arg, sizeof(arg));
            ExprRec resInt = {TempExpr, "temp_d2i", "", 0, 0, 0};
            getTemp(resInt.direccion, sizeof(resInt.direccion));
            generate("movsd", arg, "%xmm0", NULL);
            generate("cvttsd2si", "%xmm0", "%eax", NULL);
            generate("movl", "%eax", resInt.direccion, NULL);
            return resInt;
        }
        return val;
    }

    if (lex && strcmp(lex, "sizeof") == 0) {
        return processLiteral(4);
    }

    // 2. Llamada a función
    if (nodo->tipo == nodoLlamadaFuncion && lex) {
        if (strcmp(lex, "scanf") == 0 && nodo->izq) {
            struct ASTNodo *argVar = (nodo->izq->valorLexema && strcmp(nodo->izq->valorLexema, "arg") == 0)
                                     ? nodo->izq->der : nodo->izq;
            // Si pasaron &x ("unario:&"), extraemos x de argVar->izq
            struct ASTNodo *idNodo = (argVar->valorLexema && strcmp(argVar->valorLexema, "unario:&") == 0)
                                     ? argVar->izq : argVar;
            if (idNodo && idNodo->valorLexema) {
                ExprRec var = processIdNodo(idNodo);
                readId(var);
            }
            return vacio;
        }

        struct ASTNodo *args[32];
        int numArgs = aplanarArgumentos(nodo->izq, args, 32);
        ExprRec valsArgs[32];
        int esCadena[32] = {0};

        for (int i = 0; i < numArgs && i < 32; i++) {
            if (args[i]->izq == NULL && args[i]->der == NULL &&
                args[i]->valorLexema && args[i]->valorLexema[0] == '"') {
                esCadena[i] = 1;
            }
            valsArgs[i] = evaluarExpresion(args[i]);
        }

        int extraArgs = (numArgs > 6) ? (numArgs - 6) : 0;
        int paddingAlineacion = (extraArgs % 2 != 0) ? 8 : 0;

        if (paddingAlineacion > 0) {
            generate("subq", "$8", "%rsp", NULL);
        }

        for (int i = numArgs - 1; i >= 6; i--) {
            char argStr[64];
            obtenerOperando(valsArgs[i], argStr, sizeof(argStr));
            if (esCadena[i]) {
                generate("leaq", argStr, "%rax", NULL);
            } else if (valsArgs[i].kind == LiteralExpr) {
                generate("movq", argStr, "%rax", NULL);   // Literal inmediato ($1, $2, $3) -> movq
            } else {
                generate("movslq", argStr, "%rax", NULL); // Variable o temporal (-4(%rbp)) -> movslq
            }
            generate("pushq", "%rax", NULL, NULL);
        }

        int idxInt = 0;
        int idxFlt = 0;
        
        for (int i = 0; i < numArgs; i++) {
            char argStr[64];
            obtenerOperando(valsArgs[i], argStr, sizeof(argStr));

            if (valsArgs[i].esFlotante) {
                if (idxFlt < 8) {
                    char regXmm[16];
                    snprintf(regXmm, sizeof(regXmm), "%%xmm%d", idxFlt++);
                    generate("movsd", argStr, regXmm, NULL);
                }
            } else if (idxInt < 6) {
              if (esCadena[i]) {
                  generate("leaq", argStr, RegArgs64[idxInt++], NULL);
              } else if (valsArgs[i].esPuntero) {
                  generate("movq", argStr, RegArgs64[idxInt++], NULL);
              } else {
                  generate("movl", argStr, RegArgs32[idxInt++], NULL);
              }
          }
        }

        char numXmmStr[16];
        snprintf(numXmmStr, sizeof(numXmmStr), "$%d", idxFlt);

        generate("movl", numXmmStr, "%eax", NULL);
        generate("call", lex, NULL, NULL);

        if (extraArgs > 0) {
            int bytesLimpiar = (extraArgs * 8) + paddingAlineacion;
            char cleanStr[32];
            snprintf(cleanStr, sizeof(cleanStr), "$%d", bytesLimpiar);
            generate("addq", cleanStr, "%rsp", NULL);
        }
        
        RegistroSemantico *simFunc = buscarSimbolo(lex);
        int retornaFlotante = (simFunc && simFunc->tipoDato &&
                               (strstr(simFunc->tipoDato, "float") != NULL ||
                                strstr(simFunc->tipoDato, "double") != NULL));

        ExprRec retTemp = {TempExpr, "temp_call", "", 0, retornaFlotante};
        if (retornaFlotante) {
            getTempFloat(retTemp.direccion, sizeof(retTemp.direccion));
            generate("movsd", "%xmm0", retTemp.direccion, NULL);
        } else {
            getTemp(retTemp.direccion, sizeof(retTemp.direccion));
            generate("movl", "%eax", retTemp.direccion, NULL);
        }
        return retTemp;
    }

    // 3. Hojas: Literales numéricos o Identificadores
    if (nodo->izq == NULL && nodo->der == NULL) {
        // Caso literal flotante
        if (esLiteralFlotanteNodo(nodo)) {
            const char *etiq = obtenerEtiquetaFlotante(nodo);
            ExprRec fRec = {IdExpr, "flt_lit", "", 0, 1};
            snprintf(fRec.direccion, sizeof(fRec.direccion), "%s(%%rip)", etiq ? etiq : "fmt_flt");
            return fRec;
        }
        // Caso literal numérico (TokenInt / TokenFloat)
        if (lex == NULL || isdigit((unsigned char)lex[0]) ||
            ((lex[0] == '-' || lex[0] == '+') && isdigit((unsigned char)lex[1]))) {
            return processLiteral((int)nodo->valorNumerico);
        }
        // Caso literal carácter 'a'
        if (lex[0] == '\'' && strlen(lex) >= 3) {
            return processLiteral((int)lex[1]);
        }
        if (lex[0] == '"') {
            const char *etiq = obtenerEtiquetaCadena(nodo);
            ExprRec strRec = {IdExpr, "str_lit", "", 0, 0, 0};
            snprintf(strRec.direccion, sizeof(strRec.direccion), "%s(%%rip)", etiq ? etiq : "fmt_out");
            return strRec;
        }
        // Caso identificador (variable)
        return processIdNodo(nodo); 
    }

    // Postfijos
    if (nodo->izq != NULL && nodo->der == NULL && lex && (strcmp(lex, "++") == 0 || strcmp(lex, "--") == 0)) {
        return genIncDec(nodo->izq, lex, 0); // 0 = postfijo
    }

    // 4. Operadores unarios prefijo ("unario:-", "unario:+", "unario:!", "unario:~")
    if (nodo->izq != NULL && nodo->der == NULL && lex && strncmp(lex, "unario:", 7) == 0) {
        const char *opUnario = lex + 7;

        // Prefijos
        if (strcmp(opUnario, "++") == 0 || strcmp(opUnario, "--") == 0) {
            return genIncDec(nodo->izq, opUnario, 1); // 1 = prefijo
        }

        ExprRec operando = evaluarExpresion(nodo->izq);

        if (strcmp(opUnario, "+") == 0) {
            return operando;
        }
        if (strcmp(opUnario, "-") == 0) {
            // 0 - operando
            ExprRec cero = processLiteral(0);
            OpRec opSub;
            strcpy(opSub.operador, "-");
            if (operando.esFlotante) {
                return genInfixFloat(cero, opSub.operador, operando);
            }
            return genInfix(cero, opSub, operando);
        }
        if (strcmp(opUnario, "&") == 0) {
            ExprRec var = processIdNodo(nodo->izq);
            ExprRec res = {TempExpr, "temp_addr", "", 0, 0, 0};
            getTempFloat(res.direccion, sizeof(res.direccion)); // 8 bytes para puntero de 64 bits
            generate("leaq", var.direccion, "%rax", NULL);
            generate("movq", "%rax", res.direccion, NULL);
            res.esPuntero = 1;
            return res;
        }
        if (strcmp(opUnario, "*") == 0) {
            // Lectura por desreferencia: *ptr
            ExprRec ptr = evaluarExpresion(nodo->izq);
            char ptrStr[64];
            obtenerOperando(ptr, ptrStr, sizeof(ptrStr));

            ExprRec res = {TempExpr, "temp_deref", "", 0, 0, 0};
            getTemp(res.direccion, sizeof(res.direccion));
            generate("movq", ptrStr, "%rax", NULL);
            generate("movl", "(%rax)", "%eax", NULL);
            generate("movl", "%eax", res.direccion, NULL);
            res.esPuntero = 1;
            return res;
        }
        if (strcmp(opUnario, "!") == 0) {
            if (operando.esFlotante) {
                char arg[64];
                obtenerOperando(operando, arg, sizeof(arg));
                ExprRec res = {TempExpr, "temp_not", "", 0, 0, 0};
                getTemp(res.direccion, sizeof(res.direccion));
                generate("movsd", arg, "%xmm0", NULL);
                generate("pxor", "%xmm1", "%xmm1", NULL); // 0.0 en xmm1
                generate("ucomisd", "%xmm1", "%xmm0", NULL);
                generate("sete", "%al", NULL, NULL);
                generate("movzbl", "%al", "%eax", NULL);
                generate("movl", "%eax", res.direccion, NULL);
                return res;
            }
            ExprRec cero = processLiteral(0);
            return genRelacional(operando, "==", cero);
        }
        if (strcmp(opUnario, "~") == 0) {
            char arg[64];
            obtenerOperando(operando, arg, sizeof(arg));
            ExprRec res = {TempExpr, "temp_not", "", 0, 0, 0};
            getTemp(res.direccion, sizeof(res.direccion));
            generate("movl", arg, "%eax", NULL);
            generate("notl", "%eax", NULL, NULL);
            generate("movl", "%eax", res.direccion, NULL);
            return res;
        }
    }

    // 5. Operadores binarios (izq != NULL && der != NULL)
    if (nodo->izq != NULL && nodo->der != NULL && lex != NULL) {

        // 5A. Operadores lógicos con cortocircuito real ("&&" y "||")
        if (strcmp(lex, "&&") == 0 || strcmp(lex, "||") == 0) {
            int idLbl = contadorEtiquetas++;
            char lblCorto[64], lblFin[64];
            snprintf(lblCorto, sizeof(lblCorto), ".L_sc_%d", idLbl);
            snprintf(lblFin, sizeof(lblFin), ".L_sc_end_%d", idLbl);

            ExprRec res = {TempExpr, "temp_log", "", 0, 0, 0};
            getTemp(res.direccion, sizeof(res.direccion));

            int esAnd = (strcmp(lex, "&&") == 0);

            // 1. Evaluar únicamente el lado izquierdo
            ExprRec izq = evaluarExpresion(nodo->izq);
            char opIzq[64];
            obtenerOperando(izq, opIzq, sizeof(opIzq));

            if (izq.esFlotante) {
                generate("movsd", opIzq, "%xmm0", NULL);
                generate("pxor", "%xmm1", "%xmm1", NULL);
                generate("ucomisd", "%xmm1", "%xmm0", NULL);
            } else {
                generate("movl", opIzq, "%eax", NULL);
                generate("cmpl", "$0", "%eax", NULL);
            }

            // Salto de cortocircuito sin tocar nodo->der
            if (esAnd) {
                generate("je", lblCorto, NULL, NULL);   // izq == 0 -> falso inmediato
            } else {
                generate("jne", lblCorto, NULL, NULL);  // izq != 0 -> verdadero inmediato
            }

            // 2. Solo si no hubo cortocircuito, evaluar el lado derecho
            ExprRec der = evaluarExpresion(nodo->der);
            char opDer[64];
            obtenerOperando(der, opDer, sizeof(opDer));

            if (der.esFlotante) {
                generate("movsd", opDer, "%xmm0", NULL);
                generate("pxor", "%xmm1", "%xmm1", NULL);
                generate("ucomisd", "%xmm1", "%xmm0", NULL);
            } else {
                generate("movl", opDer, "%eax", NULL);
                generate("cmpl", "$0", "%eax", NULL);
            }

            generate("setne", "%al", NULL, NULL);
            generate("movzbl", "%al", "%eax", NULL);
            generate("movl", "%eax", res.direccion, NULL);
            generate("jmp", lblFin, NULL, NULL);

            // 3. Rama de cortocircuito
            fprintf(archivoASM, "%s:\n", lblCorto);
            generate("movl", esAnd ? "$0" : "$1", res.direccion, NULL);

            // 4. Salida común
            fprintf(archivoASM, "%s:\n", lblFin);
            return res;
        }

        // 5B. Asignación simple: x = expr  o  *ptr = expr
        if (strcmp(lex, "=") == 0) {
            if (nodo->izq && nodo->izq->valorLexema && strcmp(nodo->izq->valorLexema, "unario:*") == 0) {
                ExprRec ptr = evaluarExpresion(nodo->izq->izq);
                ExprRec fuente = evaluarExpresion(nodo->der);
                char ptrStr[64], srcStr[64];
                obtenerOperando(ptr, ptrStr, sizeof(ptrStr));
                obtenerOperando(fuente, srcStr, sizeof(srcStr));

                generate("movl", srcStr, "%edx", NULL);
                generate("movq", ptrStr, "%rax", NULL);
                generate("movl", "%edx", "(%rax)", NULL);
                return fuente;
            }
            RegistroSemantico *simDest = nodo->izq ? nodo->izq->simbolo : NULL;
            char dirDest[64];
            if (simDest) {
                if (simDest->nivelAmbito == 0) snprintf(dirDest, sizeof(dirDest), "%s(%%rip)", simDest->lexema);
                else                           snprintf(dirDest, sizeof(dirDest), "-%d(%%rbp)", simDest->offset);
            } else {
                ExprRec tmp = processIdNodo(nodo->izq);
                strncpy(dirDest, tmp.direccion, sizeof(dirDest));
                dirDest[sizeof(dirDest) - 1] = '\0';
            }

            ExprRec fuente = evaluarExpresion(nodo->der);
            assignX86(dirDest, simDest, fuente);

            ExprRec resAsig = {IdExpr, "", "", 0, 0, 0};
            strncpy(resAsig.direccion, dirDest, sizeof(resAsig.direccion));
            if (simDest && simDest->tipoDato &&
                (strstr(simDest->tipoDato, "float") != NULL || strstr(simDest->tipoDato, "double") != NULL)) {
                resAsig.esFlotante = 1;
            }
            return resAsig;
        }

        // 5C. Asignaciones compuestas: +=, -=, *=, /=, %=
        if (strcmp(lex, "+=") == 0 || strcmp(lex, "-=") == 0 ||
            strcmp(lex, "*=") == 0 || strcmp(lex, "/=") == 0 || strcmp(lex, "%=") == 0) {
            RegistroSemantico *simDest = nodo->izq ? nodo->izq->simbolo : NULL;
            char dirDest[64];
            if (simDest) {
                if (simDest->nivelAmbito == 0) snprintf(dirDest, sizeof(dirDest), "%s(%%rip)", simDest->lexema);
                else                           snprintf(dirDest, sizeof(dirDest), "-%d(%%rbp)", simDest->offset);
            } else {
                ExprRec tmp = processIdNodo(nodo->izq);
                strncpy(dirDest, tmp.direccion, sizeof(dirDest) - 1);
                dirDest[sizeof(dirDest) - 1] = '\0';
            }

            ExprRec valActual = processIdNodo(nodo->izq);
            ExprRec derVal = evaluarExpresion(nodo->der);

            char opStr[2] = { lex[0], '\0' };
            ExprRec calc;
            if (valActual.esFlotante || derVal.esFlotante) {
                calc = genInfixFloat(valActual, opStr, derVal);
            } else {
                OpRec opBase;
                strcpy(opBase.operador, opStr);
                calc = genInfix(valActual, opBase, derVal);
            }

            assignX86(dirDest, simDest, calc);

            ExprRec resAsig = {IdExpr, "", "", 0, (valActual.esFlotante || derVal.esFlotante)};
            strncpy(resAsig.direccion, dirDest, sizeof(resAsig.direccion) - 1);
            resAsig.direccion[sizeof(resAsig.direccion) - 1] = '\0';
            return resAsig;
        }

        // 5D. Operador ternario: c ? a : b
        if (strcmp(lex, "?") == 0 && nodo->der != NULL) {
            int idTern = contadorEtiquetas++;
            char etiqFalso[32], etiqFin[32];
            snprintf(etiqFalso, sizeof(etiqFalso), ".Ltern_falso_%d", idTern);
            snprintf(etiqFin, sizeof(etiqFin), ".Ltern_fin_%d", idTern);

            ExprRec res = {TempExpr, "temp_tern", "", 0, 0, 0};
            getTemp(res.direccion, sizeof(res.direccion));

            // 1. Evaluar condición c
            ExprRec cond = evaluarExpresion(nodo->izq);
            char condStr[64];
            obtenerOperando(cond, condStr, sizeof(condStr));
            if (cond.esFlotante) {
                generate("movsd", condStr, "%xmm0", NULL);
                generate("pxor", "%xmm1", "%xmm1", NULL);
                generate("ucomisd", "%xmm1", "%xmm0", NULL);
            } else {
                generate("movl", condStr, "%eax", NULL);
                generate("cmpl", "$0", "%eax", NULL);
            }
            generate("je", etiqFalso, NULL, NULL);

            // 2. Rama verdadera (a = nodo->der->izq)
            ExprRec valTrue = evaluarExpresion(nodo->der->izq);
            char trueStr[64];
            obtenerOperando(valTrue, trueStr, sizeof(trueStr));
            generate("movl", trueStr, "%eax", NULL);
            generate("movl", "%eax", res.direccion, NULL);
            generate("jmp", etiqFin, NULL, NULL);

            // 3. Rama falsa (b = nodo->der->der)
            fprintf(archivoASM, "%s:\n", etiqFalso);
            ExprRec valFalse = evaluarExpresion(nodo->der->der);
            char falseStr[64];
            obtenerOperando(valFalse, falseStr, sizeof(falseStr));
            generate("movl", falseStr, "%eax", NULL);
            generate("movl", "%eax", res.direccion, NULL);

            fprintf(archivoASM, "%s:\n", etiqFin);
            return res;
        }

        // 5E. Evaluar ambos operandos (para operadores estrictos)
        ExprRec e1 = evaluarExpresion(nodo->izq);
        ExprRec e2 = evaluarExpresion(nodo->der);

        // 5F. Aritméticos básicos (+, -, *, /, %)
        if (strcmp(lex, "+") == 0 || strcmp(lex, "-") == 0 ||
            strcmp(lex, "*") == 0 || strcmp(lex, "/") == 0 || strcmp(lex, "%") == 0) {
            if (e1.esFlotante || e2.esFlotante) {
                return genInfixFloat(e1, lex, e2);
            }
            OpRec op;
            strncpy(op.operador, lex, sizeof(op.operador) - 1);
            op.operador[sizeof(op.operador) - 1] = '\0';
            return genInfix(e1, op, e2);
        }

        // 5G. Relacionales e igualdad (==, !=, <, <=, >, >=)
        if (strcmp(lex, "==") == 0 || strcmp(lex, "!=") == 0 ||
            strcmp(lex, "<") == 0  || strcmp(lex, "<=") == 0 ||
            strcmp(lex, ">") == 0  || strcmp(lex, ">=") == 0) {
            return genRelacional(e1, lex, e2);
        }
    }

    return vacio;
}

// Helper para recorrer listas de declaración (incluyendo "declaracion-multiple" y "lista-ids")
void procesarDeclaracion(struct ASTNodo *nodo) {
    if (!nodo || errorGeneracion) {
        return;
    }
    char *lex = nodo->valorLexema;
    if (!lex){
        return;
    }

    if (strcmp(lex, "declaracion-multiple") == 0) {
        // nodo->izq son los especificadores ("int", etc.): NO se evalúan como variables
        procesarDeclaracion(nodo->der);
        return;
    }
    if (strcmp(lex, "lista-ids") == 0) {
        procesarDeclaracion(nodo->izq);
        procesarDeclaracion(nodo->der);
        return;
    }
    if (strcmp(lex, "var-init") == 0 || strcmp(lex, "var-init-bloque") == 0) {
        struct ASTNodo *asigNodo = nodo->izq; // Nodo "="
        if (asigNodo && asigNodo->izq && asigNodo->der) {
            struct ASTNodo *idNodo = asigNodo->izq;
            RegistroSemantico *simDest = idNodo->simbolo;

            char dirDest[64];
            if (simDest) {
                if (simDest->nivelAmbito == 0) snprintf(dirDest, sizeof(dirDest), "%s(%%rip)", simDest->lexema);
                else                           snprintf(dirDest, sizeof(dirDest), "-%d(%%rbp)", simDest->offset);
            } else {
                ExprRec tmp = processIdNodo(idNodo);
                strncpy(dirDest, tmp.direccion, sizeof(dirDest) - 1);
                dirDest[sizeof(dirDest) - 1] = '\0';
            }

            ExprRec fuente = evaluarExpresion(asigNodo->der);
            assignX86(dirDest, simDest, fuente);
        }
        return;
    }
    
    if (strcmp(lex, "=") == 0 && nodo->izq && nodo->der) {
        RegistroSemantico *simDest = nodo->izq->simbolo;
        char dirDest[64];
        if (simDest) {
            if (simDest->nivelAmbito == 0) {
                snprintf(dirDest, sizeof(dirDest), "%s(%%rip)", simDest->lexema);
            } else {
                snprintf(dirDest, sizeof(dirDest), "-%d(%%rbp)", simDest->offset);
            }
        } else {
            ExprRec destino = processIdNodo(nodo->izq);
            strncpy(dirDest, destino.direccion, sizeof(dirDest));
            dirDest[sizeof(dirDest) - 1] = '\0';
        }
        ExprRec valor = evaluarExpresion(nodo->der);
        assignX86(dirDest, simDest, valor);
        return;
    }
}

// Calcula dinámicamente cuántos bytes de pila necesita la función
int calcularEspacioPila(struct ASTNodo *nodoFuncion) {
    int bytesLocales = nodoFuncion ? nodoFuncion->bytesLocales : 0;
    // Sumamos un margen para temporales intermedios de expresiones complejas 
    int total = bytesLocales + 512;
    // Redondeo al siguiente múltiplo de 16 bytes (ABI x86-64 System V)
    return ((total + 15) / 16) * 16;
}

void recorrerNodo(struct ASTNodo *nodo) {
    if (!nodo || errorGeneracion) return;

    switch (nodo->tipo) {
        case nodoPrograma:
            if (nodo->izq && nodo->izq->tipo != nodoDeclaracionVariable) {
                recorrerNodo(nodo->izq);
            }
            if (nodo->der && nodo->der->tipo != nodoDeclaracionVariable) {
                recorrerNodo(nodo->der);
            }
            break;

        case nodoFuncion: {

            if (nodo->der == NULL) break;
            offsetPilaActual = nodo->bytesLocales;
            int espacioPila = calcularEspacioPila(nodo);

            fprintf(archivoASM, "%s:\n", nodo->valorLexema);
            generate("pushq", "%rbp", NULL, NULL);
            generate("movq", "%rsp", "%rbp", NULL);

            char subRsp[32];
            snprintf(subRsp, sizeof(subRsp), "$%d", espacioPila);
            generate("subq", subRsp, "%rsp", NULL);

            struct ASTNodo *cabecera = nodo->izq;
            struct ASTNodo *listaParams = cabecera ? cabecera->der : NULL;
            struct ASTNodo *params[32];
            int numParams = aplanarParametros(listaParams, params, 32);

            for (int i = 0; i < numParams; i++) {
                struct ASTNodo *pNodo = params[i];
                if (pNodo && pNodo->simbolo != NULL) {
                    char destPila[32];
                    snprintf(destPila, sizeof(destPila), "-%d(%%rbp)", pNodo->simbolo->offset);
                    int esPtr = (pNodo->simbolo->tipoDato && strchr(pNodo->simbolo->tipoDato, '*') != NULL);
                    
                    if (i < 6) {
                        if (esPtr) {
                            generate("movq", RegArgs64[i], destPila, NULL);
                        } else {
                            generate("movl", RegArgs32[i], destPila, NULL);
                        }
                    } else {
                        int offsetCaller = 16 + (i - 6) * 8;
                        char srcCaller[32];
                        snprintf(srcCaller, sizeof(srcCaller), "%d(%%rbp)", offsetCaller);

                        if (esPtr) {
                            generate("movq", srcCaller, "%rax", NULL);
                            generate("movq", "%rax", destPila, NULL);
                        } else {
                            generate("movl", srcCaller, "%eax", NULL);
                            generate("movl", "%eax", destPila, NULL);
                        }
                    }
                }
            }

            recorrerNodo(nodo->der); // Cuerpo de la función

            // Epílogo de respaldo por si la función es void o no tiene return explícito
            generate("movq", "%rbp", "%rsp", NULL);
            generate("popq", "%rbp", NULL, NULL);
            generate("ret", NULL, NULL, NULL);
            fprintf(archivoASM, "\n");
            break;
        }
        case nodoBloqueCodigo:
        case nodoRelleno:
            recorrerNodo(nodo->izq);
            recorrerNodo(nodo->der);
            break;

        case nodoDeclaracionVariable:
            procesarDeclaracion(nodo);
            break;

        case nodoExpresion:
            if (nodo->valorLexema && strcmp(nodo->valorLexema, "break") == 0) {
            if (topeLoops >= 0) {
                generate("jmp", pilaBreak[topeLoops], NULL, NULL);
            }
            break;
            }
            if (nodo->valorLexema && strcmp(nodo->valorLexema, "continue") == 0) {
                if (topeLoops >= 0 && pilaContinue[topeLoops][0] != '\0') {
                    generate("jmp", pilaContinue[topeLoops], NULL, NULL);
                }
                break;
            }
        case nodoLlamadaFuncion:
            // Caso especial: sentencia return expr; (lexema == "return", izq == expr)[cite: 3, 4]
            if (nodo->valorLexema && strcmp(nodo->valorLexema, "return") == 0) {
                if (nodo->izq != NULL) {
                    ExprRec retVal = evaluarExpresion(nodo->izq);
                    char retStr[64];
                    obtenerOperando(retVal, retStr, sizeof(retStr));
                    if (retVal.esFlotante) {
                        generate("movsd", retStr, "%xmm0", NULL);
                        generate("cvttsd2si", "%xmm0", "%eax", NULL);
                    } else {
                        generate("movl", retStr, "%eax", NULL);
                    }
                } else {
                    generate("movl", "$0", "%eax", NULL);
                }
                generate("movq", "%rbp", "%rsp", NULL);
                generate("popq", "%rbp", NULL, NULL);
                generate("ret", NULL, NULL, NULL);
            } else {
                // Cualquier expresión suelta como sentencia (ej: x = x + 5; o printf(...);)[cite: 4]
                evaluarExpresion(nodo);
            }
            break;
        case nodoCondicionIf:
            genIf(nodo);
            break;

        case nodoLoopWhile:
            genWhile(nodo);
            break;

        case nodoLoopDoWhile:
            genDoWhile(nodo);
            break;

        case nodoLoopFor:
            genFor(nodo);
            break;

        default:
            recorrerNodo(nodo->izq);
            recorrerNodo(nodo->der);
            break;
    }
}

void liberarSimbolos() {
    for (int i = 0; i < totalHistoricoSimbolos; i++) {
        if (historicoSimbolos[i] != NULL) {
            free(historicoSimbolos[i]->lexema);
            free(historicoSimbolos[i]->tipoDato);
            free(historicoSimbolos[i]);
            historicoSimbolos[i] = NULL;
        }
    }
    totalHistoricoSimbolos = 0;
}

// Genera código ensamblador para ++ / -- (prefijo y postfijo)
ExprRec genIncDec(struct ASTNodo *operandoNodo, const char *op, int esPrefijo) {
    ExprRec var = evaluarExpresion(operandoNodo);
    const char *inst = (strcmp(op, "++") == 0) ? "addl" : "subl";

    if (esPrefijo) {
        // ++x o --x: modifica la variable primero y retorna la variable con su valor nuevo
        generate(inst, "$1", var.direccion, NULL);
        return var;
    } else {
        // x++ o x--: guarda el valor anterior en un temporal, modifica la variable, y retorna el temporal
        ExprRec tempViejo;
        tempViejo.kind = TempExpr;
        tempViejo.valor = 0;
        tempViejo.esFlotante = 0;
        tempViejo.esPuntero = 0;
        snprintf(tempViejo.nombre, sizeof(tempViejo.nombre), "temp_post");
        getTemp(tempViejo.direccion, sizeof(tempViejo.direccion));

        generate("movl", var.direccion, "%eax", NULL);
        generate("movl", "%eax", tempViejo.direccion, NULL);
        generate(inst, "$1", var.direccion, NULL);

        return tempViejo;
    }
}

// Recorre las declaraciones globales del AST y las emite en .section .data
void emitirGlobalesAST(struct ASTNodo *nodo) {
    if (!nodo) return;

    // 1. Nivel raíz del programa: solo bajamos por nodos de programa y declaraciones globales
    if (nodo->tipo == nodoPrograma) {
        emitirGlobalesAST(nodo->izq);
        emitirGlobalesAST(nodo->der);
        return;
    }

    // Si es una función, no contiene declaraciones globales (sus variables son locales)
    if (nodo->tipo == nodoFuncion) {
        return;
    }

    // 2. Declaraciones globales (nodoDeclaracionVariable)
        if (nodo->tipo == nodoDeclaracionVariable) {
            const char *lex = nodo->valorLexema;

            // Caso A: Contenedor "var-init" -> el nodo "=" está en nodo->izq
            if (lex && strcmp(lex, "var-init") == 0 && nodo->izq) {
                struct ASTNodo *asig = nodo->izq;
                if (asig->izq && asig->izq->valorLexema) {
                    const char *nombre = asig->izq->valorLexema;
                    int valorInit = 0;
                    if (asig->der && asig->der->izq == NULL && asig->der->der == NULL) {
                        valorInit = (int)asig->der->valorNumerico;
                    }
                    fprintf(archivoASM, "    %s: .long %d\n", nombre, valorInit);
                }
                return;
            }

            // Caso B: Asignación directa en declaracionVariable (nodo "=")
            if (lex && strcmp(lex, "=") == 0 && nodo->izq && nodo->izq->valorLexema) {
                const char *nombre = nodo->izq->valorLexema;
                int valorInit = 0;
                if (nodo->der && nodo->der->izq == NULL && nodo->der->der == NULL) {
                    valorInit = (int)nodo->der->valorNumerico;
                }
                fprintf(archivoASM, "    %s: .long %d\n", nombre, valorInit);
                return;
            }

            // Caso C: Variable global sin inicialización ("var-simple")
            if (lex && strcmp(lex, "var-simple") == 0 && nodo->izq && nodo->izq->valorLexema) {
                fprintf(archivoASM, "    %s: .long 0\n", nodo->izq->valorLexema);
                return;
            }

            // Caso D: Contenedores ("declaracion-multiple", "lista-ids")
            emitirGlobalesAST(nodo->izq);
            emitirGlobalesAST(nodo->der);
        }
}

void pushLoop(const char *etiqBreak, const char *etiqContinue){
    if (topeLoops < MaxLoops - 1) {
        topeLoops++;
        strncpy(pilaBreak[topeLoops], etiqBreak, 31);
        pilaBreak[topeLoops][31] = '\0';
        if (etiqContinue) {
            strncpy(pilaContinue[topeLoops], etiqContinue, 31);
            pilaContinue[topeLoops][31] = '\0';
        } else {
            pilaContinue[topeLoops][0] = '\0';
        }
    }
}

void popLoop(){
    if (topeLoops >= 0) {
        topeLoops--;
    }
}

void saltarSiFalso(struct ASTNodo *condNodo, const char *etiquetaFalso){
    if (!condNodo) {
        return; 
    }
    ExprRec cond = evaluarExpresion(condNodo);
    char condStr[64];
    obtenerOperando(cond, condStr, sizeof(condStr));

    generate("movl", condStr, "%eax", NULL);
    generate("cmpl", "$0", "%eax", NULL);
    generate("je", etiquetaFalso, NULL, NULL);
}

// Recorre recursivamente cualquier cadena de if / else if / else saltando a 'etiqFinGlobal' al terminar la rama verdadera
void genIfCadena(struct ASTNodo *nodo, const char *etiqFinGlobal) {
    if (!nodo || errorGeneracion) return;
    const char *lex = nodo->valorLexema;
    if (!lex || strcmp(lex, "if-error") == 0) return;

    int id = contadorEtiquetas++;
    char etiqFalso[32];
    snprintf(etiqFalso, sizeof(etiqFalso), ".Lelse_%d", id);

    // 1. Nodo simple: "if" o "else-if" (izq = condición, der = cuerpo)
    if (strcmp(lex, "if") == 0 || strcmp(lex, "else-if") == 0) {
        saltarSiFalso(nodo->izq, etiqFalso);
        recorrerNodo(nodo->der);
        generate("jmp", etiqFinGlobal, NULL, NULL);
        fprintf(archivoASM, "%s:\n", etiqFalso);
        return;
    }

    // 2. Nodo compuesto: "if-else", "if-elseif", "if-elseif-else" o "cadena-else-if"
    //    nodo->izq es la rama (o subcadena) anterior; nodo->der es el siguiente else-if o el bloque else final
    if (strcmp(lex, "if-else") == 0 || strcmp(lex, "if-elseif") == 0 ||
        strcmp(lex, "if-elseif-else") == 0 || strcmp(lex, "cadena-else-if") == 0) {
        genIfCadena(nodo->izq, etiqFinGlobal);

        // Si nodo->der es otro condicional ("else-if" o "cadena-else-if"), seguimos encadenando;
        // si es el bloque 'else' final (nodoBloqueCodigo, nodoExpresion, etc.), lo ejecutamos directamente
        if (nodo->der && nodo->der->tipo == nodoCondicionIf) {
            genIfCadena(nodo->der, etiqFinGlobal);
        } else {
            recorrerNodo(nodo->der);
        }
    }
}

void genIf(struct ASTNodo *nodo) {
    if (!nodo || errorGeneracion) return;
    int id = contadorEtiquetas++;
    char etiqFinGlobal[32];
    snprintf(etiqFinGlobal, sizeof(etiqFinGlobal), ".Lendif_%d", id);

    genIfCadena(nodo, etiqFinGlobal);
    fprintf(archivoASM, "%s:\n", etiqFinGlobal);
}

void genWhile(struct ASTNodo *nodo){
    int id = contadorEtiquetas++;
    char etiqInicio[32], etiqFin[32];
    snprintf(etiqInicio, sizeof(etiqInicio), ".Lwhile_inicio_%d", id);
    snprintf(etiqFin, sizeof(etiqFin), ".Lwhile_fin_%d", id);

    pushLoop(etiqFin, etiqInicio);

    fprintf(archivoASM, "%s:\n", etiqInicio);
    saltarSiFalso(nodo->izq, etiqFin);
    recorrerNodo(nodo->der);
    generate("jmp", etiqInicio, NULL, NULL);
    fprintf(archivoASM, "%s:\n", etiqFin);

    popLoop();
}

void genDoWhile(struct ASTNodo *nodo){
    int id = contadorEtiquetas++;
    char etiqInicio[32], etiqCond[32], etiqFin[32];
    snprintf(etiqInicio, sizeof(etiqInicio), ".Ldo_inicio_%d", id);
    snprintf(etiqCond, sizeof(etiqCond), ".Ldo_cond_%d", id);
    snprintf(etiqFin, sizeof(etiqFin), ".Ldo_fin_%d", id);

    pushLoop(etiqFin, etiqCond);

    fprintf(archivoASM, "%s:\n", etiqInicio);
    recorrerNodo(nodo->izq); // Cuerpo primero

    fprintf(archivoASM, "%s:\n", etiqCond);
    if (nodo->der) {
        ExprRec cond = evaluarExpresion(nodo->der);
        char condStr[64];
        obtenerOperando(cond, condStr, sizeof(condStr));
        generate("movl", condStr, "%eax", NULL);
        generate("cmpl", "$0", "%eax", NULL);
        generate("jne", etiqInicio, NULL, NULL); // Si es != 0, repite
    }
    fprintf(archivoASM, "%s:\n", etiqFin);

    popLoop();
}

void genFor(struct ASTNodo *nodo){
    if (!nodo || (nodo->valorLexema && strcmp(nodo->valorLexema, "for-error") == 0)) {
        return;
    }

    int id = contadorEtiquetas++;
    char etiqCond[32], etiqInc[32], etiqFin[32];
    snprintf(etiqCond, sizeof(etiqCond), ".Lfor_cond_%d", id);
    snprintf(etiqInc, sizeof(etiqInc), ".Lfor_inc_%d", id);
    snprintf(etiqFin, sizeof(etiqFin), ".Lfor_fin_%d", id);

    struct ASTNodo *cabecera = nodo->izq;
    struct ASTNodo *initNodo = cabecera ? cabecera->izq : NULL;
    struct ASTNodo *condInc  = cabecera ? cabecera->der : NULL;
    struct ASTNodo *condNodo = condInc ? condInc->izq : NULL;
    struct ASTNodo *incNodo  = condInc ? condInc->der : NULL;

    // 1. Inicialización
    if (initNodo) {
        recorrerNodo(initNodo);
    }

    pushLoop(etiqFin, etiqInc);

    // 2. Evaluación de condición
    fprintf(archivoASM, "%s:\n", etiqCond);
    if (condNodo) {
        saltarSiFalso(condNodo, etiqFin);
    }

    // 3. Cuerpo del for
    recorrerNodo(nodo->der);

    // 4. Incremento (aquí salta 'continue')
    fprintf(archivoASM, "%s:\n", etiqInc);
    if (incNodo) {
        evaluarExpresion(incNodo);
    }
    generate("jmp", etiqCond, NULL, NULL);

    // 5. Salida (aquí salta 'break')
    fprintf(archivoASM, "%s:\n", etiqFin);

    popLoop();
}

void recolectarCadenasAST(struct ASTNodo *nodo) {
    if (!nodo) return;
    if (nodo->tipo == nodoExpresion && nodo->izq == NULL && nodo->der == NULL &&
        nodo->valorLexema && nodo->valorLexema[0] == '"') {
        if (totalCadenas < MaxString) {
            snprintf(tablaCadenas[totalCadenas].etiqueta, 32, ".Lstr_%d", totalCadenas);
            tablaCadenas[totalCadenas].nodo = nodo;
            fprintf(archivoASM, "    %s: .string %s\n",
                    tablaCadenas[totalCadenas].etiqueta, nodo->valorLexema);
            totalCadenas++;
        }
        return;
    }
    recolectarCadenasAST(nodo->izq);
    recolectarCadenasAST(nodo->der);
}

const char *obtenerEtiquetaCadena(const struct ASTNodo *nodo) {
    for (int i = 0; i < totalCadenas; i++) {
        if (tablaCadenas[i].nodo == nodo) return tablaCadenas[i].etiqueta;
    }
    return NULL;
}

int aplanarParametros(struct ASTNodo *nodo, struct ASTNodo **params, int maxParams) {
    if (!nodo) return 0;
    if (nodo->valorLexema && strcmp(nodo->valorLexema, "lista-parametros") == 0) {
        int n = aplanarParametros(nodo->izq, params, maxParams);
        if (n < maxParams && nodo->der) params[n++] = nodo->der;
        return n;
    }
    if (maxParams > 0) {
        params[0] = nodo;
        return 1;
    }
    return 0;
}

int aplanarArgumentos(struct ASTNodo *nodo, struct ASTNodo **args, int maxArgs) {
    if (!nodo) return 0;
    if (nodo->valorLexema && strcmp(nodo->valorLexema, "arg") == 0) {
        int n = aplanarArgumentos(nodo->izq, args, maxArgs);
        if (n < maxArgs && nodo->der) args[n++] = nodo->der;
        return n;
    }
    if (maxArgs > 0) {
        args[0] = nodo;
        return 1;
    }
    return 0;
}

void recolectarFlotantesAST(struct ASTNodo *nodo) {
    if (!nodo) return;

    if (esLiteralFlotanteNodo(nodo)) {
        if (totalFlotantes < MaxFloats) {
            snprintf(tablaFlotantes[totalFlotantes].etiqueta, 32, ".Lflt_%d", totalFlotantes);
            tablaFlotantes[totalFlotantes].nodo = nodo;
            fprintf(archivoASM, "    %s: .double %.17g\n",
                    tablaFlotantes[totalFlotantes].etiqueta, nodo->valorNumerico);
            totalFlotantes++;
        }
        return;
    }
    
    recolectarFlotantesAST(nodo->izq);
    recolectarFlotantesAST(nodo->der);
}

const char *obtenerEtiquetaFlotante(const struct ASTNodo *nodo) {
    for (int i = 0; i < totalFlotantes; i++) {
        if (tablaFlotantes[i].nodo == nodo) return tablaFlotantes[i].etiqueta;
    }
    return NULL;
}

void getTempFloat(char *buf, size_t size) {
    // Alinear a 8 bytes antes de reservar
    if (offsetPilaActual % 8 != 0) {
        offsetPilaActual += (8 - (offsetPilaActual % 8));
    }
    offsetPilaActual += 8;
    snprintf(buf, size, "-%d(%%rbp)", offsetPilaActual);
}

ExprRec asegurarDouble(ExprRec e) {
    if (e.esFlotante) return e;
    char arg[64];
    obtenerOperando(e, arg, sizeof(arg));
    ExprRec res = {TempExpr, "temp_i2d", "", 0, 1};
    getTempFloat(res.direccion, sizeof(res.direccion));
    if (e.kind == LiteralExpr) {
        generate("movl", arg, "%eax", NULL);
        generate("cvtsi2sd", "%eax", "%xmm0", NULL);
    } else {
        generate("cvtsi2sd", arg, "%xmm0", NULL);
    }
    generate("movsd", "%xmm0", res.direccion, NULL);
    return res;
}

ExprRec genInfixFloat(ExprRec e1, const char *op, ExprRec e2) {
    ExprRec d1 = asegurarDouble(e1);
    ExprRec d2 = asegurarDouble(e2);
    char arg1[64];
    char arg2[64];
    obtenerOperando(d1, arg1, sizeof(arg1));
    obtenerOperando(d2, arg2, sizeof(arg2));

    ExprRec res = {TempExpr, "temp_flt", "", 0, 1, 0};
    getTempFloat(res.direccion, sizeof(res.direccion));

    generate("movsd", arg1, "%xmm0", NULL);
    if (strcmp(op, "+") == 0)      generate("addsd", arg2, "%xmm0", NULL);
    else if (strcmp(op, "-") == 0) generate("subsd", arg2, "%xmm0", NULL);
    else if (strcmp(op, "*") == 0) generate("mulsd", arg2, "%xmm0", NULL);
    else if (strcmp(op, "/") == 0) generate("divsd", arg2, "%xmm0", NULL);

    generate("movsd", "%xmm0", res.direccion, NULL);
    return res;
}

int esLiteralFlotanteNodo(const struct ASTNodo *nodo) {
    if (!nodo || nodo->tipo != nodoExpresion || nodo->izq != NULL || nodo->der != NULL) return 0;
    const char *lex = nodo->valorLexema;
    if (!lex) return 0;
    int i = (lex[0] == '-' || lex[0] == '+') ? 1 : 0;
    if (!isdigit((unsigned char)lex[i]) && lex[i] != '.') return 0;
    return (strchr(lex, '.') != NULL || strchr(lex, 'e') != NULL || strchr(lex, 'E') != NULL);
}