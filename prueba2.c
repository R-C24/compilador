#include <stdio.h>
#include <stdlib.h>

#define Max_Buffer 100
#define FACTOR 2

struct Datos {
    int id;
    float valor;
};

// --- FUNCIÓN 1: SÍMBOLOS Y TILDE (ERRORES 1 Y 2) ---
void funcion_uno() {
    int a = 10;
    int b = 20;
    int c = 30;
    int d = 40;

    int @variable_invalida = 50; // [ERROR 1]: Símbolo '@' no permitido

    int número_con_tilde = 100; // [ERROR 2]: Carácter con tilde (no ASCII)

    int suma = a + b + c;
    printf("Suma: %d\n", suma);
}

// --- FUNCIÓN 2: IDENTIFICADORES (ERROR 3) ---
void funcion_dos() {

    int 1er_contador = 1; // [ERROR 3]: Identificador inicia con número

    float resultado = 45.5f;
    printf("Resultado: %f\n", resultado);
}

// --- FUNCIÓN 3: LITERALES NUMÉRICOS (ERRORES 4, 5 Y 6) ---
void funcion_tres() {

    float pi_malo = 3.14.15; // [ERROR 4]: Flotante con múltiples puntos decimales

    int hex_malo = 0x12G4; // [ERROR 5]: Hexadecimal con dígito inválido 'G'

    double exp_malo = 1.5e++; // [ERROR 6]: Formato de exponente incorrecto

    int valido = 100;
    printf("Valido: %d\n", valido);
}

// --- FUNCIÓN 4: CADENAS Y CARACTERES (ERRORES 7 Y 8) ---
void funcion_cuatro() {

    char char_vacio = ''; // [ERROR 7]: Literal de carácter vacío

    // [ERROR 8]: Cadena de texto sin cerrar
    char *texto_malo = "Cadena sin cerrar al final de linea;

    char valido_c = 'A';
    printf("Caracter: %c\n", valido_c);
}

// --- FUNCIÓN 5: OPERADORES Y COMENTARIOS (ERRORES 9 Y 10) ---
void funcion_cinco() {
    int x = 5;
    int y = 10;

    int z = x ::: y;     // [ERROR 9]: Operador ':::' no existente en C

    printf("Fin de funcion cinco\n");
}

int main() {
    printf("Iniciando prueba de 100 lineas con 10 errores...\n");

    struct Datos d;
    d.id = 1;
    d.valor = 99.9f;

    funcion_uno();
    funcion_dos();
    funcion_tres();
    funcion_cuatro();
    funcion_cinco();

    for (int i = 0; i < 5; i++) {
        printf("Iteracion: %d\n", i);
    }

    // [ERROR 10]: Comentario de bloque sin cerrar
    /* Inicio de comentario sin cerrar al final...
    return 0;
}