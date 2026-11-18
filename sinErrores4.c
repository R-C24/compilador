// ============================================================================
// CASO DE PRUEBA 4: Estructuras (Structs), Campos y Ámbitos Anidados
// Descripción: Valida el reconocimiento, declaración e instanciación de 
// estructuras (structs) en la tabla de símbolos, así como el acceso a sus 
// campos dentro de diferentes contextos y ámbitos anidados sin errores.
// ============================================================================

struct Coordenada {
    int x;
    int y;
    int z;
};

struct Rectangulo {
    int ancho;
    int alto;
    int area;
};

struct ObjetoEspacial {
    int id_objeto;
    int activo;
    int codigo_estado;
};

struct ConfiguracionSistema {
    int modo_operacion;
    int nivel_depuracion;
    int limite_memoria;
};

int contador_global_structs = 0;
int estado_global_sistema = 1;

int calcular_area_rectangulo(int base, int altura) {
    int producto_total = 0;
    producto_total = base * altura;
    return producto_total;
}

void procesar_coordenadas() {
    struct Coordenada coord_local;
    coord_local.x = 10;
    coord_local.y = 20;
    coord_local.z = 30;

    {
        int desplazamiento_extra = 5;
        coord_local.x = coord_local.x + desplazamiento_extra;
        coord_local.y = coord_local.y + desplazamiento_extra;
        
        {
            int factor_multiplicador = 2;
            coord_local.z = coord_local.z * factor_multiplicador;
            contador_global_structs = contador_global_structs + coord_local.x + coord_local.y + coord_local.z;
        } // Fin bloque interno 2
    } // Fin bloque interno 1
}

int evaluar_rectangulo_personalizado(int p_ancho, int p_alto) {
    struct Rectangulo rect_local;
    rect_local.ancho = p_ancho;
    rect_local.alto = p_alto;
    rect_local.area = calcular_area_rectangulo(rect_local.ancho, rect_local.alto);

    {
        int margen_error = 2;
        if (rect_local.area > 50) {
            int ajuste_positivo = 10;
            rect_local.area = rect_local.area + ajuste_positivo - margen_error;
        } else {
            int ajuste_negativo = 5;
            rect_local.area = rect_local.area + ajuste_negativo + margen_error;
        }
    }

    return rect_local.area;
}

void inicializar_espacio_trabajo() {
    struct ObjetoEspacial obj_principal;
    obj_principal.id_objeto = 999;
    obj_principal.activo = 1;
    obj_principal.codigo_estado = 77;

    {
        struct ObjetoEspacial obj_secundario;
        obj_secundario.id_objeto = 888;
        obj_secundario.activo = 0;
        obj_secundario.codigo_estado = 44;

        if (obj_principal.activo == 1) {
            int acumulador_temp = 0;
            acumulador_temp = obj_principal.codigo_estado + obj_secundario.codigo_estado;
            contador_global_structs = contador_global_structs + acumulador_temp;
        } else {
            contador_global_structs = contador_global_structs + obj_principal.id_objeto;
        }
    }
}

void configurar_parametros_sistema() {
    struct ConfiguracionSistema config_local;
    config_local.modo_operacion = 1;
    config_local.nivel_depuracion = 3;
    config_local.limite_memoria = 1024;

    {
        int incremento_memoria = 512;
        config_local.limite_memoria = config_local.limite_memoria + incremento_memoria;
        
        {
            int verificador_limite = 2000;
            if (config_local.limite_memoria < verificador_limite) {
                estado_global_sistema = config_local.modo_operacion + config_local.nivel_depuracion;
            } else {
                estado_global_sistema = 0;
            }
        }
    }
}

void main() {
    int resultado_final_structs = 0;
    configurar_parametros_sistema();
    procesar_coordenadas();
    resultado_final_structs = evaluar_rectangulo_personalizado(12, 8);
    inicializar_espacio_trabajo();
    contador_global_structs = contador_global_structs + resultado_final_structs;
}