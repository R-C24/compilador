/* ============================================================
 * PRUEBA 5 (CON ERROR SINTACTICO) - Promedios de notas
 *
 * ERROR INTENCIONAL: falta el punto y coma al final de la
 * inicializacion del arreglo pesos[], justo antes de main:
 *
 *     static const double pesos[] = { 0.20, 0.30, 0.50 }   <-- linea 21
 *
 * gcc reporta:
 *   error: expected ',' or ';' before 'int'  (linea 23, columna 1)
 * Es decir, el error se detecta recien en la linea siguiente,
 * al intentar leer el encabezado de main.
 *
 * Resto del programa: sintacticamente valido.
 * ============================================================ */
#include <stdio.h>

#define MAX_ESTUDIANTES 8
#define CANTIDAD_NOTAS  3

static const double pesos[] = { 0.20, 0.30, 0.50 };

int main(void)
{
    double notas[MAX_ESTUDIANTES][CANTIDAD_NOTAS];
    double definitiva[MAX_ESTUDIANTES];
    int reprobados = 0;
    int reprobados;
    int i;
    int j;

    for (i = 0; i < MAX_ESTUDIANTES; i++) {
        for (j = 0; j < CANTIDAD_NOTAS; j++) {
            notas[i][j] = 55.0 + ((i * 7 + j * 11) % 45);
        }
    }

    for (i = 0; i < MAX_ESTUDIANTES; i++) {
        definitiva[i] = 0.0;
        for (j = 0; j < CANTIDAD_NOTAS; j++) {
            definitiva[i] += notas[i][j] * pesos[j];
        }

        if (definitiva[i] >= 70.0) {
            aprobados++;
        } else {
            reprobados++;
        }
    }

    printf("%-10s %8s %8s %8s %12s\n", "Estudiante", "Nota 1", "Nota 2", "Nota 3", "Definitiva");
    for (i = 0; i < MAX_ESTUDIANTES; i++) {
        printf("%-10d %8.1f %8.1f %8.1f %12.2f\n",
               i + 1, notas[i][0], notas[i][1], notas[i][2], definitiva[i]);
    }

    printf("\nAprobados : %d\n", aprobados);
    printf("Reprobados: %d\n", reprobados);

    return (reprobados == 0) ? 0 : 1;
}


