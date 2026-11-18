// ============================================================================
// CASO DE PRUEBA 1: Alcances Anidados Extensos y Sombreado de Variables (Shadowing)
// Descripción: Valida el comportamiento de la pila de Tablas de Símbolos ante 
// múltiples niveles de bloques anidados con variables homónimas en distintos ámbitos.
// Al finalizar cada bloque, se debe desplegar la tabla de símbolos correspondiente.
// ============================================================================

int var_global_principal = 5000;
int estado_sistema = 1;
int acumulador_global = 0;

void registrar_inicio() {
    int codigo_inicio = 100;
    acumulador_global = acumulador_global + codigo_inicio;
}

int calcular_operacion_compleja(int parametro_a, int parametro_b) {
    int resultado_local = 0;
    int factor_multiplicador = 3;

    // --- NIVEL DE BLOQUE 0 (Función) ---
    {
        int variable_bloque_0_a = 10;
        int variable_bloque_0_b = 20;
        int variable_bloque_0_c = 30;

        resultado_local = variable_bloque_0_a + variable_bloque_0_b + variable_bloque_0_c;

        // --- NIVEL DE BLOQUE 1 ---
        {
            int variable_bloque_0_a = 99; // Sombreado de variable del bloque padre
            int variable_bloque_1_a = 40;
            int variable_bloque_1_b = 50;

            resultado_local = resultado_local + variable_bloque_0_a + variable_bloque_1_a + variable_bloque_1_b;

            // --- NIVEL DE BLOQUE 2 ---
            {
                int variable_bloque_0_a = 888; // Sombreado profundo
                int variable_bloque_1_a = 777; // Sombreado intermedio
                int variable_bloque_2_a = 60;
                int variable_bloque_2_b = 70;

                resultado_local = resultado_local + variable_bloque_0_a + variable_bloque_1_a + variable_bloque_2_a + variable_bloque_2_b;

                // --- NIVEL DE BLOQUE 3 ---
                {
                    int variable_bloque_0_a = 1111;
                    int variable_bloque_1_a = 2222;
                    int variable_bloque_2_a = 3333;
                    int variable_bloque_3_a = 80;
                    int variable_bloque_3_b = 90;

                    resultado_local = resultado_local + variable_bloque_3_a + variable_bloque_3_b;
                    acumulador_global = acumulador_global + resultado_local;
                } // Fin Bloque 3 - Dump de Tabla de Símbolos

                resultado_local = resultado_local + 5;
            } // Fin Bloque 2 - Dump de Tabla de Símbolos

            resultado_local = resultado_local + 10;
        } // Fin Bloque 1 - Dump de Tabla de Símbolos

        resultado_local = resultado_local * factor_multiplicador;
    } // Fin Bloque 0 - Dump de Tabla de Símbolos

    return resultado_local;
}

void main() {
    int resultado_ejecucion = 0;
    registrar_inicio();
    resultado_ejecucion = calcular_operacion_compleja(15, 25);
    var_global_principal = var_global_principal + resultado_ejecucion;
}