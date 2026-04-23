 
#ifndef MATRIZ_H 
#define MATRIZ_H

#include<stdlib.h>
#include<stdbool.h>
#include<stdio.h>

   typedef struct matriz matriz_t;
   
    //FUNCIONES

    //CREAR Y DESTRUIR

    void matriz_destruir(matriz_t *matriz);

    matriz_t *_matriz_crear(size_t n, size_t m);
    
    matriz_t *_matriz_crear_inicializada(size_t n, size_t m);
    
    //ACCESO A DATOS

    size_t matriz_filas(const matriz_t *matriz);

    size_t matriz_columnas(const matriz_t *matriz);

    void matriz_dimensiones(const matriz_t *matriz, size_t *filas, size_t *columnas);

    float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna);

    bool matriz_copiar(const matriz_t *matrizDestino , const matriz_t *matrizOrigen);
   
    //SETEAR
    void matriz_setter(matriz_t *matriz, size_t fila, size_t columna, float valor);

    //GENERACION

    matriz_t *matriz_leer(size_t n, size_t m); 

    //EXTENSION

    bool matriz_extender_filas(matriz_t *matriz);

    bool matriz_extender_columnas(matriz_t *matriz);

    //APLICACIONES SUELTAS

    matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b);

    void matriz_aplicar(matriz_t *m, float (*funcion)(float));

    void inicializar_random(matriz_t *m);

    void matriz_imprimir(const matriz_t *m);

    //ARCHIVOS BINARIOS

    matriz_t* matriz_leerBN(FILE *f);

    bool matriz_escribirBN(matriz_t *m, FILE *f);
    
    void matriz_escribirtxt(matriz_t *m, FILE *f);

    #endif // matriz.h
