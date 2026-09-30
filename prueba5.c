#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// --- MACROS CON IDENTIFICADORES Y VALORES DE LONGITUD EXTREMA ---
#define MACRO_CON_NOMBRE_EXTREMADAMENTE_LARGO_PARA_EVALUAR_EL_BUFFER_DEL_PREPROCESADOR_Y_DEL_ANALIZADOR_LEXICO_EN_C_SIN_PROVOCAR_SEGFAULT 99999
#define MACRO_CADENA_MASIVA "Esta es una cadena insertada mediante macro que busca verificar la asignacion de memoria dinamica en strdup"
#define SUMAR_TRES_PARAMETROS_CON_EXPANSION_LARGA(x, y, z) (((x) + (y)) * (z) + MACRO_CON_NOMBRE_EXTREMADAMENTE_LARGO_PARA_EVALUAR_EL_BUFFER_DEL_PREPROCESADOR_Y_DEL_ANALIZADOR_LEXICO_EN_C_SIN_PROVOCAR_SEGFAULT)

// --- ESTRUCTURA CON CAMPOS EXTENSOS ---
struct EstructuraDePruebaDeEstrésParaValidarBuffersYMemoriaEnElScanner {
    int id;
    double datos_procesados[100];
    char buffer_local[512];
};

void funcion_de_estres_identificadores() {
    // 1. Identificador de variable con longitud extrema (>120 caracteres)
    int variable_con_nombre_extremadamente_largo_que_supera_los_limites_habituales_de_los_compiladores_simples_para_verificar_reserva_de_memoria = 1234;

    int valor_normal = 50;
    int suma = variable_con_nombre_extremadamente_largo_que_supera_los_limites_habituales_de_los_compiladores_simples_para_verificar_reserva_de_memoria + valor_normal;

    printf("Suma de identificadores masivos: %d\n", suma);
}

void funcion_de_estres_linea_gigante() {
    // 2. LÍNEA EXTREMADAMENTE LARGA (Más de 650 caracteres en una sola instrucción sin saltos de línea)
    int a1=1, a2=2, a3=3, a4=4, a5=5, a6=6, a7=7, a8=8, a9=9, a10=10, a11=11, a12=12, a13=13, a14=14, a15=15, a16=16, a17=17, a18=18, a19=19, a20=20, a21=21, a22=22, a23=23, a24=24, a25=25, a26=26, a27=27, a28=28, a29=29, a30=30, a31=31, a32=32, a33=33, a34=34, a35=35, a36=36, a37=37, a38=38, a39=39, a40=40, a41=41, a42=42, a43=43, a44=44, a45=45, a46=46, a47=47, a48=48, a49=49, a50=50;

    int acumulado = a1 + a10 + a20 + a30 + a40 + a50;
    printf("Acumulado de variables en linea gigante: %d\n", acumulado);
}

void funcion_de_estres_cadenas_y_comentarios() {
    // 3. Cadena de texto extremadamente larga en un solo literal (>400 caracteres)
    char cadena_gigante[] = "Esta es una cadena extremadamente larga disenada especificamente para evaluar el comportamiento de los buffers temporales en Flex y en la funcion de lectura del preprocesador fgets o fread. Si el buffer es pequenio o no esta bien delimitado por un caracter nulo final, podria ocurrir un desbordamiento de memoria buffer overflow o un Segmentation Fault al procesar el archivo. Continuamos agregando texto para asegurar la prueba.";

    // 4. Comentario monolitico de gran longitud
    // AAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAA

    /* 5. Comentario de bloque extenso
       BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB
    */

    printf("Cadena gigante procesada con exito: %s\n", cadena_gigante);
}

void funcion_de_estres_anidamiento_profundo() {
    // 6. Anidamiento masivo de paréntesis y operadores (25 niveles de profundidad)
    int resultado_anidado = (((((((((((((((((((((((((1 + 2) * 3) - 4) + 5) * 6) - 7) + 8) * 9) - 10) + 11) * 12) - 13) + 14) * 15) - 16) + 17) * 18) - 19) + 20) * 21) - 22) + 23) * 24);

    printf("Resultado de expresiones profundamente anidadas: %d\n", resultado_anidado);
}

// --- SECUENCIA DE OPERACIONES REPETITIVAS PARA EVALUAR CONSUMO DE TABLA DE SÍMBOLOS ---
void evaluar_reserva_simbolos_1() { int x1 = 1; int x2 = 2; int x3 = 3; printf("Paso 1: %d\n", x1+x2+x3); }
void evaluar_reserva_simbolos_2() { int y1 = 1; int y2 = 2; int y3 = 3; printf("Paso 2: %d\n", y1+y2+y3); }
void evaluar_reserva_simbolos_3() { int z1 = 1; int z2 = 2; int z3 = 3; printf("Paso 3: %d\n", z1+z2+z3); }

// --- ESTRUCTURA CON ARREGLOS Y ACCESO CONTINUO ---
struct DatosEstrés {
    int v1; int v2; int v3; int v4; int v5;
    double d1; double d2; double d3;
};

void procesar_estrellamiento_memoria() {
    struct DatosEstrés array[10];
    for (int i = 0; i < 10; i++) {
        array[i].v1 = i * 10;
        array[i].v2 = i * 20;
        array[i].v3 = i * 30;
        array[i].v4 = i * 40;
        array[i].v5 = i * 50;
        array[i].d1 = (double)i * 1.5;
        array[i].d2 = (double)i * 2.5;
        array[i].d3 = (double)i * 3.5;
    }
    printf("Estructura de prueba iterada correctamente. Elemento 9: %d\n", array[9].v5);
}

// --- EVALUACIÓN DE MACROS CON PARÁMETROS MÚLTIPLES ---
#define CALC_COMPLEJO(a, b, c, d, e) ((a)*(b) + (c)*(d) - (e))
#define REPETIR_MACRO_1 CALC_COMPLEJO(1, 2, 3, 4, 5)
#define REPETIR_MACRO_2 (REPETIR_MACRO_1 + REPETIR_MACRO_1)
#define REPETIR_MACRO_3 (REPETIR_MACRO_2 * REPETIR_MACRO_2)

void probar_macros_estres() {
    int val = REPETIR_MACRO_3;
    int val2 = SUMAR_TRES_PARAMETROS_CON_EXPANSION_LARGA(10, 20, 30);
    printf("Valores de macro expandidos: %d, %d\n", val, val2);
}

// --- FUNCIÓN PRINCIPAL ---
int main() {
    printf("========================================================\n");
    printf("INICIANDO PRUEBA DE ESTRÉS DE 130 LÍNEAS EN EL SCANNER\n");
    printf("========================================================\n");

    funcion_de_estres_identificadores();
    funcion_de_estres_linea_gigante();
    funcion_de_estres_cadenas_y_comentarios();
    funcion_de_estres_anidamiento_profundo();

    evaluar_reserva_simbolos_1();
    evaluar_reserva_simbolos_2();
    evaluar_reserva_simbolos_3();

    procesar_estrellamiento_memoria();
    probar_macros_estres();

    struct EstructuraDePruebaDeEstrésParaValidarBuffersYMemoriaEnElScanner inst;
    inst.id = 1000;
    inst.datos_procesados[0] = 99.9;
    strcpy(inst.buffer_local, MACRO_CADENA_MASIVA);

    printf("Prueba de instanciacion completada: ID %d, Cadena: %s\n", inst.id, inst.buffer_local);
    printf("========================================================\n");
    printf("PRUEBA DE ESTRÉS COMPLETADA SIN SEGMENTATION FAULT\n");
    printf("========================================================\n");

    return 0;
}