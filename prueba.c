// prueba.c
#include <stdio.h>

#define MAX 100

int main() {
    int contador = 10;
    float valor = 25.5;

    // Simulación de error léxico con caracteres no válidos
    char @caracter_invalido = '$';

    if (contador < MAX) {
        contador = contador + 1;
    }

    return 0;
}