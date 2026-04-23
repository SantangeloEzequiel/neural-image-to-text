#ifndef REDNEURONAL_H 
#define REDNEURONAL_H

#include "matriz.h"
#include <stdio.h>

/**
 * Esta estructura contiene los datos necesarios para almacenar una red neuronal multicapa:
 * Numero de capas, pesos, neuronas intermedias, funciones de activación.
*/
typedef struct redNeuronal redNeuronal_t;

/**
 * Dada una entrada, devuelve la salida de la red neuronal en una matriz_t. Caso de error devuelve NULL.
*/
matriz_t *redNeuronal_evaluar(redNeuronal_t *redNeuronal, matriz_t *entrada);

/**
 * Permite entrenar una red neuronal. Cada llamada a la función aplica el algoritmo de Backpropagation.
 * Devuelve el error cuadrático medio entre la entrada obtenida y la deseada. Caso de error devuelve 0.
*/
float entrenar(redNeuronal_t *red, const matriz_t *salida_deseada, matriz_t *entrada, float eta);

/**
 * Permite guardar una red neuronal ya entrenada en un archivo .bin. Devuelve true si fue exitoso.  
*/
bool redNeuronal_guardar(redNeuronal_t *red, FILE *f);

/**
 * Permite cargar una red neuronal ya entrenada en un archivo .bin. Devuelve true si fue exitoso.  
*/
redNeuronal_t* redNeuronal_cargar(FILE *f);

/**
 * Retorna una nueva red neuronal. El numero de capas y neuronas es el de las capas intermedias. Caso de error devuelve NULL.
*/
redNeuronal_t* redNeuronal_construir(size_t numero_capas, size_t *neuronas,  float (**funcion)(float));

/**
 * Libera la memoria asignada para los datos de la red neuronal.
*/
void redNeuronal_destruir(redNeuronal_t *redNeuronal);

//FUNCIONES DE ACTIVACION

float sigmoidea(float x);

float relu(float x);

float identidad(float x);

float tanh_activacion(float x);

float sigmoidea_derivada(float x);

float relu_derivada(float x);

float identidad_derivada(float x);

float tanh_activacion_derivada(float x);

#endif