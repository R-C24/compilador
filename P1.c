/* ============================================================
 * PRUEBA 4 (CON ERROR SINTACTICO) - Facturacion sencilla
 *
 * ERROR INTENCIONAL: en la funcion total_facturado, el
 * encabezado del ciclo for usa una COMA en lugar del primer
 * punto y coma:
 *
 *     for (i = 0, i < n; i++)    <-- linea 36, la coma esta en la columna 15
 *
 * Es un unico caracter equivocado. gcc reporta:
 *   error: expected ';' before ')' token  (linea 36, columna 27)
 *
 * Resto del programa: sintacticamente valido.
 * ============================================================ */
#include <stdio.h>
#include <string.h>

#define MAX_ITEMS 20

static double subtotal_item(const struct item *it)
{
    return it->cantidad * it->precio;
}

double total_facturado(const struct item *items, int n)
{
    double acumulado = 0.0;
    int i;

    for (i = 0, i < n; i++) {
        acumulado += subtotal_item(&items[i]);
    }

    return acumulado;
}

int main(void)
{
    int n = 4;
    int i;
    double total;
    double impuesto;

    for (i = 0; i < n; i++) {
        snprintf(factura[i].descripcion, sizeof(factura[i].descripcion),
                 "Articulo %d", i + 1);
        factura[i].cantidad = i + 1;
        factura[i].precio = 1250.0 + (i * 300.0);
    }

    total = total_facturado(factura, n);
    impuesto = (total > 10000.0) ? total * 0.13 : 0.0;

    printf("%-12s %8s %10s %12s\n", "Descripcion", "Cant.", "Precio", "Subtotal");
    for (i = 0; i < n; i++) {
        printf("%-12s %8d %10.2f %12.2f\n",
               factura[i].descripcion,
               factura[i].cantidad,
               factura[i].precio,
               subtotal_item(&factura[i]));
    }

    printf("\nSubtotal : %10.2f\n", total);
    printf("Impuesto : %10.2f\n", impuesto);
    printf("Total    : %10.2f\n", total + impuesto);

    return 0;
}


