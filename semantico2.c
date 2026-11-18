// ============================================================================
// CASO SEMÁNTICO 2: Errores en bucles y ámbitos anidados
// ============================================================================

int factorEscala = 5;

int transformar(int limite) {
    int acumulador = 0;
    int acumulador = 1; // [ERROR SEMÁNTICO]: Redeclaración de 'acumulador' en la función

    int i = 0;
    while (i < limite) {
        int paso = 2;
        int paso = 4; // [ERROR SEMÁNTICO]: Redeclaración en el ámbito del while

        acumulador = acumulador + paso + factorDesconocido; // [ERROR SEMÁNTICO]: 'factorDesconocido' no declarada
        i = i + 1;
    }

    // Intentar usar 'paso' fuera de su ámbito (el while)
    // Nota: Como 'paso' no está en la tabla, esto debe disparar error de no declarado:
    acumulador = acumulador + paso; // [ERROR SEMÁNTICO]: 'paso' no declarada fuera del bucle

    return acumulador;
}

int main() {
    int limite = 10;
    int res = transformar(limite);

    do {
        int iter = 0;
        int iter = 1; // [ERROR SEMÁNTICO]: Redeclaración en el do-while
        res = res - iter;
    } while (res > 0);

    return 0;
}