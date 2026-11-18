// ============================================================================
// CASO SEMÁNTICO 1: Errores de redeclaración y no declarados
// ============================================================================

int variableGlobal = 100;
int variableGlobal = 200; // [ERROR SEMÁNTICO]: Redeclaración en el mismo ámbito global

int calcular(int x, int y) {
    int resultado = x + y;
    int resultado = 50;   // [ERRORSEMÁNTICO]: Redeclaración en el ámbito local de la función

    if (resultado > 10) {
        int temporal = x * 2;
        int temporal = x * 3; // [ERROR SEMÁNTICO]: Redeclaración en el ámbito del bloque 'if'
        
        // Uso de variable no declarada
        resultado = resultado + variableInexistente; // [ERROR SEMÁNTICO]: Variable no declarada
    }

    // Uso de función no declarada
    funcionFantasma(resultado); // [ERROR SEMÁNTICO]: Función no declarada

    return resultado;
}

int main() {
    int a = 5;
    int b = 10;
    int total = calcular(a, b);

    // Uso de variable no declarada en el main
    total = total + saldoAcumulado; // [ERROR SEMÁNTICO]: Variable no declarada

    return 0;
}