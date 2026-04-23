
#ifndef   IMAGEN_H
#define  IMAGEN_H

/**
 * imagen_bmp.h - use un estilo doxygen (a mano)
 * Definiciones y funciones relacionadas con la manipulación de imágenes BMP monocromas.
 *
 * Este archivo proporciona la estructura imagen_t para almacenar imágenes.
 * y las funciones asociadas a la carga y manipulación de estas imágenes.
 */

#include<stdbool.h>
#include<stddef.h>
#include<stdio.h>

/**
 * imagen_bmp
 * Estructura que representa una imagen.
 *
 * Esta estructura contiene los datos necesarios para almacenar una imagen.
 */
typedef struct imagen imagen_t;

/**
 * Carga una imagen BMP desde un archivo.
 * param f Nombre del archivo de la imagen BMP a cargar.
 *
 * Retorna la imagen contenida en él, o NULL en caso contrario.
 */
imagen_t *imagen_desde_archivo_BMP(FILE *f);

/**
 * Libera la memoria asignada para los datos de la imagen.
 * param im Puntero a la estructura imagen_t cuya memoria será liberada.
 */
void imagen_destruir(imagen_t *im);

/**
 * Devuelve el ancho de la imagen en pixeles.
 * param im Puntero a la estructura imagen_t.
 *
 * Retorna el valor del ancho de la imagen.
 */
size_t imagen_ancho(const imagen_t *im);

/**
 * Devuelve el alto de la imagen en pixeles.
 * param im Puntero a la estructura imagen_t.
 *
 * Retorna el valor del alto de la imagen.
 */
size_t imagen_alto(const imagen_t *im);

/**
 * Devuelve el valor del pixel en una fila-columna determinada.
 * param im Puntero a la estructura imagen_t.
 * param fila Fila de la imagen (coordenada y).
 * param columna Columna de la imagen (coordenada x).
 *
 * retorna true si el píxel está encendido (valor 1), false si está apagado (valor 0) o si el valor esta fuera de los límites.
 */
bool imagen_get_pixel(const imagen_t *im, size_t fila, size_t columna); 

/**
 * Imprime la imagen por stdout.
 * param im Puntero a la estructura imagen_t.
 */
void imprimir_imagen_stdout(imagen_t *im);

#endif