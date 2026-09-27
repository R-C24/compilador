#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "beamer.h"


int compilarTexPdf(char *archivoTex) {
    char comando[512];
    char archivoPdf[512];

    printf("Compilando LaTeX a PDF mediante pdflatex...\n");

    snprintf(comando, sizeof(comando), "pdflatex -interaction=nonstopmode %s", archivoTex);

    int estado1 = system(comando);
    int estado2 = system(comando);

    //printf("Estado1: %d.\n", estado1);
    //printf("Estado2: %d.\n", estado2);

    strncpy(archivoPdf, archivoTex, sizeof(archivoPdf) - 1);
    char *dot = strrchr(archivoPdf, '.');
    if (dot) {
        strcpy(dot, ".pdf");
    } else {
        strcat(archivoPdf, ".pdf");
    }

    int pdfExiste = (access(archivoPdf, F_OK) == 0);

    if (estado1 == 0 && estado2 == 0 || pdfExiste) {
        printf("PDF generado correctamente.\n");
        return 1;
    } else {
        fprintf(stderr, "ERROR: La compilación con pdflatex ha fallado.\n");
        return 0;
    }
}

int desplegarPdf(char *archivoPdf) {
    char comando[512];
    printf("Desplegando presentación en visor evince...\n");

    snprintf(comando, sizeof(comando), "evince %s &", archivoPdf);

    int resultado = system(comando);
    return (resultado == 0);
}