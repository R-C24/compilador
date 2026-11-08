/* ============================================================
 * PRUEBA 1 (VALIDA) - Cola circular de clientes de un banco
 * Debe compilar con gcc sin ningun diagnostico y el parser
 * debe aceptarla sin errores sintacticos.
 * ============================================================ */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_CLIENTES 32
#define NOMBRE_MAX   40

enum tipo_gestion {
    DEPOSITO,
    RETIRO,
    CAMBIO_DIVISA,
    CONSULTA
};

struct cliente {
    int              turno;
    enum tipo_gestion gestion;
    char             nombre[NOMBRE_MAX];
    double           monto;
};

struct cola {
    struct cliente datos[MAX_CLIENTES];
    int            frente;
    int            final;
    int            cantidad;
};

static const char *nombre_gestion(enum tipo_gestion g)
{
    switch (g) {
    case DEPOSITO:      return "deposito";
    case RETIRO:        return "retiro";
    case CAMBIO_DIVISA: return "cambio de divisa";
    case CONSULTA:      return "consulta";
    default:            return "desconocida";
    }
}

void cola_inicializar(struct cola *c)
{
    c->frente = 0;
    c->final = 0;
    c->cantidad = 0;
}

int cola_vacia(const struct cola *c)
{
    return c->cantidad == 0;
}

int cola_llena(const struct cola *c)
{
    return c->cantidad == MAX_CLIENTES;
}

int cola_encolar(struct cola *c, const struct cliente *nuevo)
{
    int siguiente;

    if (cola_llena(c)) {
        return -1;
    }

    c->datos[c->final] = *nuevo;
    siguiente = (c->final + 1) % MAX_CLIENTES;
    c->final = siguiente;
    c->cantidad++;
    return 0;
}

int cola_desencolar(struct cola *c, struct cliente *salida)
{
    if (cola_vacia(c)) {
        return -1;
    }

    *salida = c->datos[c->frente];
    c->frente = (c->frente + 1) % MAX_CLIENTES;
    c->cantidad--;
    return 0;
}

double total_por_gestion(const struct cola *c, enum tipo_gestion g)
{
    double total = 0.0;
    int i;
    int pos;

    for (i = 0; i < c->cantidad; i++) {
        pos = (c->frente + i) % MAX_CLIENTES;
        if (c->datos[pos].gestion == g) {
            total += c->datos[pos].monto;
        }
    }

    return total;
}

static void imprimir_cliente(const struct cliente *cl)
{
    printf("  Turno %02d | %-12s | %-15s | %9.2f\n",
           cl->turno,
           cl->nombre,
           nombre_gestion(cl->gestion),
           cl->monto);
}

int main(int argc, char *argv[])
{
    struct cola c;
    struct cliente temp;
    int i;
    int atendidos = 0;
    int opcion = 0;
    int continuar = 1;
    double acumulado[4] = { 0.0, 0.0, 0.0, 0.0 };

    if (argc > 2) {
        fprintf(stderr, "Uso: %s [cantidad_de_clientes]\n", argv[0]);
        return EXIT_FAILURE;
    }

    cola_inicializar(&c);

    for (i = 0; i < MAX_CLIENTES; i++) {
        temp.turno = i + 1;
        temp.gestion = (enum tipo_gestion) (i % 4);
        snprintf(temp.nombre, NOMBRE_MAX, "Cliente_%02d", i + 1);
        temp.monto = 1000.0 + (i * 137.5);
        if (cola_encolar(&c, &temp) != 0) {
            break;
        }
    }

    printf("Clientes en espera: %d\n\n", c.cantidad);

    while (!cola_vacia(&c) && continuar) {
        if (cola_desencolar(&c, &temp) != 0) {
            break;
        }

        imprimir_cliente(&temp);
        acumulado[temp.gestion] += temp.monto;
        atendidos++;

        if (atendidos >= 10) {
            continuar = 0;
        }

        opcion = atendidos % 3;
        if (opcion == 0) {
            printf("  -- pausa de caja --\n");
        } else if (opcion == 1) {
            printf("  -- caja rapida --\n");
        } else {
            printf("  -- caja normal --\n");
        }
    }

    printf("\nResumen por gestion\n");
    for (i = 0; i < 4; i++) {
        printf("  %-15s : %10.2f\n",
               nombre_gestion((enum tipo_gestion) i),
               total_por_gestion(&c, (enum tipo_gestion) i) + acumulado[i]);
    }

    printf("\nTotal atendido en esta corrida: %d\n", atendidos);
    printf("Clientes restantes: %d\n", c.cantidad);

    return (atendidos > 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}

