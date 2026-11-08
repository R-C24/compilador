/* ============================================================
 * PRUEBA 3 (VALIDA) - Analizador lexico minimo con registros
 * de bits, uniones, directivas de preproceso y salto goto.
 * Debe pasar gcc y el parser sin diagnosticos.
 * ============================================================ */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define TAM_BUFFER     128
#define TAM_TABLA       16
#define ES_DIGITO(c)   ((c) >= '0' && (c) <= '9')
#define ES_LETRA(c)    (((c) >= 'a' && (c) <= 'z') || ((c) >= 'A' && (c) <= 'Z'))
#define MAYUSCULA(c)   (ES_LETRA(c) ? ((c) >= 'a' ? (c) - 32 : (c)) : (c))

#ifdef DEPURACION
#define TRAZA(msg)  fprintf(stderr, "[traza] %s\n", msg)
#else
#define TRAZA(msg)
#endif


static void lexer_iniciar(struct lexer *lx, const char *texto)
{
    strncpy(lx->fuente, texto, TAM_BUFFER - 1);
    lx->fuente[TAM_BUFFER - 1] = '\0';
    lx->posicion = 0;
    lx->columna = 1;
    lx->errores = 0;
}

static int leer_identificador(struct lexer *lx, struct lexema *salida)
{
    int inicio = lx->posicion;
    int largo;

    while (ES_LETRA(lx->fuente[lx->posicion]) || ES_DIGITO(lx->fuente[lx->posicion])
           || lx->fuente[lx->posicion] == '_') {
        lx->posicion++;
        lx->columna++;
    }

    largo = lx->posicion - inicio;
    if (largo >= TAM_BUFFER) {
        largo = TAM_BUFFER - 1;
    }

    memcpy(salida->valor.texto, lx->fuente + inicio, (size_t) largo);
    salida->valor.texto[largo] = '\0';
    salida->tipo = LEX_IDENTIFICADOR;
    return largo;
}

static int leer_numero(struct lexer *lx, struct lexema *salida)
{
    long acumulado = 0;
    int hay_punto = 0;
    double decimal = 0.0;
    double escala = 0.1;

    while (ES_DIGITO(lx->fuente[lx->posicion]) || lx->fuente[lx->posicion] == '.') {
        if (lx->fuente[lx->posicion] == '.') {
            hay_punto = 1;
        } else if (!hay_punto) {
            acumulado = acumulado * 10 + (lx->fuente[lx->posicion] - '0');
        } else {
            decimal += (lx->fuente[lx->posicion] - '0') * escala;
            escala /= 10.0;
        }
        lx->posicion++;
        lx->columna++;
    }

    salida->tipo = LEX_NUMERO;
    salida->banderas.es_constante = 1;

    if (hay_punto) {
        salida->valor.real = (double) acumulado + decimal;
        return 1;
    }

    salida->valor.entero = acumulado;
    return 0;
}

static void tokenizar(struct lexer *lx)
{
    struct lexema actual;
    char c;
    int i = 0;

    while (lx->fuente[lx->posicion] != '\0' && i < TAM_TABLA) {
        c = lx->fuente[lx->posicion];

        memset(&actual, 0, sizeof(actual));
        actual.columna = lx->columna;

        if (c == ' ' || c == '\t' || c == '\n') {
            lx->posicion++;
            lx->columna++;
            continue;
        }

        if (ES_LETRA(c) || c == '_') {
            leer_identificador(lx, &actual);
            actual.banderas.es_global = (actual.valor.texto[0] == 'g');
            actual.banderas.es_arreglo = 0;
            lx->tabla[i++] = actual;
            continue;
        }

        if (ES_DIGITO(c)) {
            if (leer_numero(lx, &actual)) {
                TRAZA("literal real detectado");
            }
            lx->tabla[i++] = actual;
            continue;
        }

        if (strchr("+-*/%(){};,=", c) != NULL) {
            actual.tipo = LEX_OPERADOR;
            actual.valor.texto[0] = c;
            actual.valor.texto[1] = '\0';
            lx->posicion++;
            lx->columna++;
            lx->tabla[i++] = actual;
            continue;
        }

        actual.tipo = LEX_ERROR;
        actual.valor.texto[0] = c;
        actual.valor.texto[1] = '\0';
        lx->errores++;
        lx->posicion++;
        lx->columna++;
        lx->tabla[i++] = actual;
    }

    memset(&actual, 0, sizeof(actual));
    actual.tipo = LEX_FIN;
    actual.columna = lx->columna;
    lx->tabla[i] = actual;
}

static const char *nombre_tipo(tipo_lexema t)
{
    switch (t) {
    case LEX_IDENTIFICADOR: return "identificador";
    case LEX_NUMERO:        return "numero";
    case LEX_OPERADOR:      return "operador";
    case LEX_ERROR:         return "error";
    case LEX_FIN:
    default:                return "fin";
    }
}

int main(void)
{
    struct lexer lx;
    int i = 0;
    struct lexema *lx_actual;
    char acumulador[TAM_BUFFER];
    int longitud = 0;

    lexer_iniciar(&lx, "g_total = suma + 42 * factor_3; /* 3.5 */ resultado");

    tokenizar(&lx);

    printf("%-4s %-8s %-15s %s\n", "#", "col", "tipo", "valor");
    while (lx.tabla[i].tipo != LEX_FIN && i < TAM_TABLA) {
        lx_actual = &lx.tabla[i];
        printf("%-4d %-8d %-15s %s\n",
               i, lx_actual->columna, nombre_tipo(lx_actual->tipo),
               lx_actual->valor.texto);
        i++;
    }

    if (lx.errores > 0) {
        fprintf(stderr, "\n%d caracteres no reconocidos\n", lx.errores);
        goto terminar_con_error;
    }

    for (i = 0; lx.tabla[i].tipo != LEX_FIN && i < TAM_TABLA; i++) {
        if (lx.tabla[i].tipo != LEX_IDENTIFICADOR) {
            continue;
        }
        if (longitud + (int) strlen(lx.tabla[i].valor.texto) + 2 >= TAM_BUFFER) {
            break;
        }
        if (longitud > 0) {
            acumulador[longitud++] = ',';
            acumulador[longitud++] = ' ';
        }
        strcpy(acumulador + longitud, lx.tabla[i].valor.texto);
        longitud += (int) strlen(lx.tabla[i].valor.texto);
    }
    acumulador[longitud] = '\0';

    printf("\nIdentificadores: %s\n", acumulador);
    printf("Primer caracter en mayuscula: %c\n", MAYUSCULA(acumulador[0]));

    return EXIT_SUCCESS;

terminar_con_error:
    printf("\nProceso detenido por errores lexicos\n");
    return EXIT_FAILURE;
}

