// ============================================================================
// PROGRAMA DE PRUEBA: CASO DE 120 LÍNEAS CON 7 ERRORES LÉXICOS
// Archivo: test_120_lineas_7_errores.c
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define TAMANIO 50
#define TITULO "Analizador Lexico"

struct Config {
    int id;
    double umbral;
    char simbolo;
};

// --- SECCIÓN 1: DEMOSTRACIÓN DE LITERALES VÁLIDOS ---
void probar_literales_validos() {
    // Enteros decimales con y sin sufijos
    int dec1 = 100;
    long dec2 = 50000L; //este
    unsigned int dec3 = 250U; //este
    unsigned long dec4 = 100000UL; //este

    // Enteros hexadecimales
    int hex1 = 0x1A3F; //este
    int hex2 = 0xFF; //este
    unsigned int hex3 = 0xABC12U; //este

    // Flotantes estándar, con sufijo f/L
    float f1 = 3.14159f;
    double d1 = 2.7182818284;
    long double ld1 = 1.41421356L;

    // Flotantes
    double exp1 = 1.2;
    double exp2 = 5.67;
    float exp3 = 2.5;

    // Literales de carácter simples y escapados
    char c1 = 'Z';
    char c2 = '\n';
    char c3 = '\t';
    char c4 = '\\';
    char c5 = '\'';

    // Literales de cadena simples y escapadas
    char s1[] = "Hola mundo C";
    char s2[] = "Texto con \"comillas\" y \t tabulacion\n";
    printf("%s %c %d\n", s1, c1, dec1);
}

// --- SECCIÓN 2: PRIMERAS FUNCIONES CON ERRORES LÉXICOS ---
void funcion_errores_parte1() {
    int a = 15;
    int b = 30;

    // [ERROR 1]: Carácter acento grave '`' no permitido en C
    int `variable_invalida = 42;

    // [ERROR 2]: Literal de carácter con múltiples elementos ('EX')
    char caracter_multicomp = 'EX';

    int total = a + b;
    printf("Total parte 1: %d\n", total);
}

void funcion_errores_parte2() {
    // [ERROR 3]: Prefijo hexadecimal '0x' sin dígitos
    int hex_incompleto = 0x;

    // [ERROR 4]: Literal de carácter sin comilla de cierre antes del salto de línea
    char char_sin_cerrar = 'K;

    float temp = 36.6f;
    printf("Temperatura: %.1f\n", temp);
}

// --- SECCIÓN 3: MÁS ERRORES Y OPERADORES ---
void funcion_errores_parte3() {
    bool x = true;
    bool y = false;

    // [ERROR 5]: Operador lógico de tres barras '|||' no existente en C
    if (x ||| y) {
        printf("Evaluacion invalida\n");
    }

    // [ERROR 6]: Flotante exponencial incompleto sin dígitos tras la 'e'
    double exp_incompleto = 4.2e;

    // [ERROR 7]: Carácter especial Unicode '¿' no permitido en identificadores
    int ¿pregunta = 100;
}

// --- SECCIÓN 4: FUNCIÓN PRINCIPAL ---
int main() {
    printf("=== Inicio de prueba de 120 lineas ===\n");

    probar_literales_validos();
    funcion_errores_parte1();
    funcion_errores_parte2();
    funcion_errores_parte3();

    struct Config conf;
    conf.id = 1;
    conf.umbral = 0.85;
    conf.simbolo = 'S';

    for (int i = 0; i < 3; i++) {
        printf("Iterador del bucle principal: %d\n", i);
    }

    printf("Configuracion ID: %d, Simbolo: %c\n", conf.id, conf.simbolo);
    printf("=== Fin de la prueba ===\n");
    return 0;
}
// Fin del archivo de prueba de 120 lineas