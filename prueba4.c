#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "prueba4Config.h"
#include "prueba4Geo.h"

// --- MACROS NIVELES ANIDADOS (Expansión multinivel) ---
#define VALOR_BASE 100
#define FACTOR_MULTI VALOR_BASE
#define ESCALA_TOTAL (FACTOR_MULTI * 2)
#define CAPACIDAD_MAXIMA ESCALA_TOTAL

// --- MACROS PARA TIPOS Y ESTRUCTURAS ---
#define ENTERO int
#define DECIMAL double
#define TEXTO const char*
#define ESTRUCTURA struct

// --- MACROS PARA OPERACIONES Y EXPRESIONES ---
#define SUMAR(a, b) ((a) + (b))
#define CUADRADO(x) ((x) * (x))
#define AREA_RECT(w, h) (CUADRADO(w) + CUADRADO(h))

// --- MACROS RECURSIVAS SIMULADAS / ENCADENADAS ---
#define NIVEL_1 10
#define NIVEL_2 (NIVEL_1 + 20)
#define NIVEL_3 (NIVEL_2 * 3)
#define CONFIG_FINAL NIVEL_3

ESTRUCTURA Nodo {
    ENTERO id;
    DECIMAL valor;
    TEXTO etiqueta;
};

ESTRUCTURA ConfigSistema {
    ENTERO puerto;
    ENTERO limite_conexiones;
    DECIMAL tiempo_espera;
};

#define PUERTO_DEFAULT 8080
#define MAX_CONEXIONES CAPACIDAD_MAXIMA
#define TIMEOUT 1.5

void inicializar_config(ESTRUCTURA ConfigSistema *cfg) {
    cfg->puerto = PUERTO_DEFAULT;
    cfg->limite_conexiones = MAX_CONEXIONES;
    cfg->tiempo_espera = TIMEOUT;
}

void imprimir_config(ESTRUCTURA ConfigSistema *cfg) {
    printf("Puerto: %d\n", cfg->puerto);
    printf("Limite Conexiones: %d\n", cfg->limite_conexiones);
    printf("Timeout: %.2f\n", cfg->tiempo_espera);
}

// --- PRUEBA DE EXPANSION DE MACROS EN CADENAS Y NOMBRES ---
void probar_expansiones() {
    ENTERO a = NIVEL_1;
    ENTERO b = NIVEL_2;
    ENTERO c = NIVEL_3;
    ENTERO resultado = CONFIG_FINAL;

    // La macro NIVEL_1 NO debe sustituirse dentro del texto literal
    TEXTO msj = "El valor de NIVEL_1 es constante";
    printf("%s\n", msj);

    // La macro VALOR_BASE NO debe sustituirse en VALOR_BASE_TMP
    ENTERO VALOR_BASE_TMP = 500;
    printf("Tmp: %d, Base: %d\n", VALOR_BASE_TMP, VALOR_BASE);

    printf("a: %d, b: %d, c: %d, res: %d\n", a, b, c, resultado);
}

#define MSJ_INICIO "Calculando transformaciones..."
#define MSJ_FIN "Proceso completado exitosamente."

DECIMAL calcular_metrica(DECIMAL x, DECIMAL y) {
    printf("%s\n", MSJ_INICIO);
    DECIMAL term1 = CUADRADO(x);
    DECIMAL term2 = CUADRADO(y);
    DECIMAL suma = SUMAR(term1, term2);
    DECIMAL escalado = suma * ESCALA_TOTAL;
    printf("%s\n", MSJ_FIN);
    return escalado;
}

// --- MATRIZ Y TABLA DE VALORES ---
#define FILAS 4
#define COLUMNAS 4
#define MATRIZ_TAM (FILAS * COLUMNAS)

void procesar_tabla() {
    ENTERO datos[MATRIZ_TAM];
    for (ENTERO i = 0; i < MATRIZ_TAM; i++) {
        datos[i] = i * CAPACIDAD_MAXIMA;
    }
    printf("Primer elemento: %d, Ultimo: %d\n", datos[0], datos[MATRIZ_TAM - 1]);
}

int main() {
    printf("=== PRUEBA DE PREPROCESO Y MACROS ANIDADAS ===\n");

    ESTRUCTURA ConfigSistema mi_cfg;
    inicializar_config(&mi_cfg);
    imprimir_config(&mi_cfg);

    probar_expansiones();
    DECIMAL res = calcular_metrica(3.0, 4.0);
    printf("Resultado metrica: %.2f\n", res);
    procesar_tabla();

    return 0;
}