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

    char ejemplo = 'h';

    char *palabra = "hola";

    unsigned long long grande = 18446744073709551615ULL;

    if (contador < MAX) {
        contador = <contador + 1;
    }

    return 0;
}