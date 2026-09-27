// prueba.c
#include <stdio.h>

#define MAX 100

int main() {
    int contador = 10;
    float valor = 25.5;

    double otroValor = 45.5;
    int hexa = 0x21F;

    // Simulación de error léxico con caracteres no válidos
    char @caracterInvalido = '$';

    if (contador < MAX) {
        contador = <contador + 1;
    }

    return 0;
}