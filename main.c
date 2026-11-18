#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>
#include "compilador.h"


Macro macros[MaxMacros];
int totalMacros = 0;

TablaSimbolos pilaTablas[MaxAmbitos];
int topePila = -1;

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

int mostrarTablas;

const char *archivoFuente = NULL;

int main(int argc, char *argv[]) {
    atexit(limpiarTemporalAlSalir);

    int opt;
    int activarPreproceso = 0; // El preproceso está desactivado por defecto.
    char *archivoTablas = NULL;
    mostrarTablas = 1;

    while ((opt = getopt(argc, argv, "pso:")) != -1) {
        switch (opt) {
            case 'p':
                activarPreproceso = 1;
                break;
            case 's':
                mostrarTablas = 1;
                break;
            case 'o':
                archivoTablas = optarg;
                break;
            default:
                fprintf(stderr, "\nError: no es una opción válida.\n");
                fprintf(stderr, "Uso correcto: %s [-p] [-s] [-o <archivo>] <archivo.c>\n\n", argv[0]);
                fprintf(stderr, "Opciones disponibles:\n");
                fprintf(stderr, "  -p    Activa el preprocesador opcional.\n");
                fprintf(stderr, "  -s    Despliega cada tabla de símbolos al cerrar contexto.\n");
                fprintf(stderr, "  -o <archivo>  Redirige la salida de las tablas hacia <archivo>.\n\n");

                return EXIT_FAILURE;
        }
    }

    if (optind < argc) {
        archivoFuente = argv[optind];
    } else {
        fprintf(stderr, "Error: Falta el archivo fuente .c de entrada.\n");

        fprintf(stderr, "Uso correcto: %s [-p] [-s] [-o <archivo>] <archivo.c>\n\n", argv[0]);
        fprintf(stderr, "Opciones disponibles:\n");
        fprintf(stderr, "  -p    Activa el preprocesador opcional.\n");
        fprintf(stderr, "  -s    Despliega cada tabla de símbolos al cerrar contexto.\n");
        fprintf(stderr, "  -o <archivo>  Redirige la salida de las tablas hacia <archivo>.\n\n");

        return EXIT_FAILURE;
    }

    if (access(archivoFuente, F_OK) != 0) {
        fprintf(stderr, "Error: El archivo '%s' no existe o no tiene acceso.\n", archivoFuente);
        return EXIT_FAILURE;
    }

    if (mostrarTablas && archivoTablas != NULL) {
        salidaTablas = fopen(archivoTablas, "w");
        if (!salidaTablas) {
            perror("Error al abrir el archivo de salida de tablas");
            return EXIT_FAILURE;
        }
    } else {
        salidaTablas = stdout;
    }

    const char *archivoTemporal = "archivoTemp.tmp";
    const char *archivoParaEscanear = NULL;

    if (activarPreproceso) {
        printf("Ejecutando Preprocesador...\n");

        FILE *f_temp = fopen(archivoTemporal, "w");
        if (!f_temp) {
            perror("Error al crear el archivo temporal");
            if (salidaTablas != stdout) fclose(salidaTablas);
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

    printf("Iniciando análisis y construcción de tabla de símbolos...\n");
    inicializarPila();
    yyparse();

    if (salidaTablas == stdout) {
        printf("\n--- Cierre del Contexto Global ---\n");
    }
    popAmbito();

    fclose(yyin);
    if (activarPreproceso) {
        remove(archivoTemporal);
    }
    if (salidaTablas != NULL && salidaTablas != stdout) {
        fclose(salidaTablas);
    }

    if (totalErrores > 0) {
        fprintf(stderr, "\nCompilación fallida: se encontraron %d error(es).\n", totalErrores);
        return EXIT_FAILURE;
    }

    printf("Compilación exitosa sin errores.\n");
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
    if (topePila < 0 || !mostrarTablas) return;
    
    imprimirTablaActual(); // Desplegar antes de destruir
    
    // Liberar memoria de los registros del ámbito que se cierra
    RegistroSemantico *actual = pilaTablas[topePila].raiz;
    while (actual != NULL) {
        RegistroSemantico *temp = actual;
        actual = actual->siguiente;
        free(temp->lexema);
        free(temp->tipoDato);
        free(temp);
    }
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