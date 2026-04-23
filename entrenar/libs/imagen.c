#include<stdio.h>
#include<stdlib.h>
#include<stdint.h>
#include <stddef.h>
#include "imagen.h"

struct imagen{
    /*Invariante de representación: 
    -El ancho de la imagen debe ser positivo.
    -El alto de la imagen debe ser distinto de cero. 
    */
    int32_t ancho;              // Ancho de la imagen en pixeles.
    int32_t alto;               // Alto de la imagen en pixeles.
    uint8_t **pixeles;          // Matriz dinámica de bytes. Cada fila de pixeles contiene (ancho / 8) bytes.
};

bool leer_int16_little_endian(FILE *f, int16_t *v){
    uint8_t bytes[2];                                           
    if(fread(bytes, sizeof(uint8_t), 2, f) !=2 )
        return false;
    *v = bytes[0] | (bytes[1] << 8);    //Bytes[0] es el byte menos significativo (LSB).
    return true;
}

bool leer_int32_little_endian(FILE *f, int32_t *v){
    uint8_t bytes[4];
    if(fread(bytes, sizeof(uint8_t), 4, f) != 4) 
        return false;
    *v = bytes[0] | bytes[1] << 8 | bytes[2] << 16 | bytes[3] << 24;    //bytes[0] es el byte menos significativo (LSB).
    return true;
}

void imagen_destruir(imagen_t *im) {
    for (size_t i = 0; i < abs(im->alto); i++) {
        free(im->pixeles[i]);
    }
    free(im->pixeles);
    free(im);
}

imagen_t *imagen_desde_archivo_BMP(FILE *f) {
    /*-------Encabezado de archivo. 14 bytes.-------*/
    //Los dos primeros bytes, 0 y 1, deben ser "BM" (0x42 0x4D).
    unsigned char tipo[2];
    if(fread(tipo, sizeof(unsigned char), 2, f) != 2 || tipo[0] != 'B' || tipo[1] != 'M')
        return NULL;

    //Tamanio del archivo. Byte 2 a 5.
    int32_t tamanio_archivo;
    if (!leer_int32_little_endian(f, &tamanio_archivo))
        return NULL;

    //Reservados. Byte 6 a 9.
    int16_t reservado1, reservado2;
    if (!leer_int16_little_endian(f, &reservado1) || !leer_int16_little_endian(f, &reservado2)) 
        return NULL;

    //Offset. Como solo usaremos una paleta que de dos colores por pixel, siempre valdra 62 (0x3E). Byte 10 a 13.
    int32_t offset;
    if (!leer_int32_little_endian(f, &offset) || offset != 62)
        return NULL;

    /*-------Encabezado de imagen. 40 bytes fijos por el tipo de imagen a procesar.-------*/
    //Tamanio encabezado imagen. Byte 14 a 17.
    int32_t tamanio_encabezado_imagen;
     if (!leer_int32_little_endian(f, &tamanio_encabezado_imagen) || tamanio_encabezado_imagen != 40)
        return NULL;

    //Ancho de la imagen en pixeles. Byte 18 a 21.
    int32_t ancho;
    if (!leer_int32_little_endian(f, &ancho) || ancho < 0)
        return NULL;   

    //Alto de la imagen en pixeles. Byte 22 a 25. 
    int32_t alto;
    if (!leer_int32_little_endian(f, &alto) || alto == 0)
        return NULL; 
    bool leer_invertido = false;
    if (alto < 0) {
        leer_invertido = true;  // Indica que las filas se leen de abajo hacia arriba
        alto = -alto;
    }

    //Planos. Byte 26 a 27.
    int16_t planos;
    if (!leer_int16_little_endian(f, &planos) || planos != 1)
        return NULL;

    //Bits de color. Byte 28 a 29.
    int16_t bits_de_color;
    if (!leer_int16_little_endian(f, &bits_de_color) || bits_de_color != 1) 
        return NULL;

    //Comrpresion. Byte 30 a 33.
    int32_t compresion;
    if (!leer_int32_little_endian(f, &compresion) || compresion != 0) return NULL;

    //Tamanio de imagen. Byte 34 a 37.
    int32_t tamanio_imagen;
    if(!leer_int32_little_endian(f, &tamanio_imagen))
        return NULL;
    
    //Resolucion x, resolucion y. Byte 38 a 45.
    int32_t resolucion_x, resolucion_y;
    if(!leer_int32_little_endian(f, &resolucion_x) || !leer_int32_little_endian(f, &resolucion_y))
        return NULL;

    //Tablas de color. Byte 46 a 50.
    int32_t tablas_color;
    if (!leer_int32_little_endian(f, &tablas_color) || tablas_color != 2)
        return NULL;

    //Colores importantes. Byte 51 a 54.
    int32_t colores_importantes;
    if (!leer_int32_little_endian(f, &colores_importantes) || (colores_importantes != 0 && colores_importantes != 2))
        return NULL; 

    /*-------Tabla de colores. 8 bytes (2 colores x 4 bytes por color).-------*/
    uint8_t tabla_colores_bytes[2][4]; 
    for (size_t i = 0; i < 2; i++) {
        if (fread(tabla_colores_bytes[i], 1, 4, f) != 4) 
            return NULL;
    }

    imagen_t *imagen = malloc(sizeof(imagen_t));   //Creamos la imagen.
    if (imagen == NULL) return NULL;
    imagen->alto = alto;                           //Originalmente puede ser negativo o positivo. Aca siempre pasamos el valor absoluto.
    imagen->ancho = ancho;                         //Positivo y multiplo de 8.       
    imagen->pixeles = malloc(alto * sizeof(uint8_t *));   //Reservamos memoria para las filas de pixeles.
    if (imagen->pixeles == NULL) {
        free(imagen);
        return NULL;
    }

    int32_t bytes_por_fila = ancho / 8;

    int32_t padding = (4 - (bytes_por_fila % 4)) % 4;     //Si bytes_por_fila ya es multiplo de 4 => padding sera 0. Si no es multiplo, dara la cantidad de bytes que faltan para que si sea.

    for (size_t i = 0; i < alto; i++) {
        imagen->pixeles[i] = malloc((bytes_por_fila+padding) * sizeof(uint8_t));  // Cada scan line puede tener padding, pero no le asigno memoria !(bytes_por_fila + padding).
        if (imagen->pixeles[i] == NULL) {
            imagen_destruir(imagen);
            return NULL;
        }
    }
    for(size_t i = 0; i < alto; i++){
        size_t fila = leer_invertido ? i : alto - 1 - i;       //leer_invertido==true (alto negativo) => lee de arriba hacia abajo. De lo contrario, lee de abajo hacia arriba.
        if (fread(imagen->pixeles[fila], sizeof(uint8_t), bytes_por_fila+padding, f) != bytes_por_fila+padding) {   //Empiezo leyendo hasta el padding.
            imagen_destruir(imagen);
            return NULL;
        }  

        /*if (padding > 0) {                  //Si padding es mayor que 0, lo leemos solo para desplazar el puntero de archivo.
            uint8_t buffer[padding];
            if (fread(buffer, sizeof(uint8_t), padding, f) != padding){
                imagen_destruir(imagen);
                return NULL;
            }
        }*/
    }

    return imagen;
}

size_t imagen_ancho(const imagen_t *im) {
    return (size_t)im->ancho;
}

size_t imagen_alto(const imagen_t *im) {
    return (size_t)abs(im->alto);
}

bool imagen_get_pixel(const imagen_t *im, size_t fila, size_t columna) {
    if (fila >= imagen_alto(im) || columna >= imagen_ancho(im))
        return false;

    /*Primero queremos ingresar al byte ubicado en "columna". Luego queremos ingresar al bit en ese byte, recordando que cada bit es un pixel.
    Empezamos contando de la fila[0] columna[0]. Si queremos saber en que byte se encuentra el pixel dentro de la columna, sabiendo que cada byte contiene 8 pixeles:*/
    size_t indice_byte = columna / 8;  //Por ejemplo, si queremos el pixel ubicado en la columna 20 sabremos que tenemos que ver el byte 3.
    size_t indice_bit = columna % 8;   // El indice del bit dentro del byte. de 0 a 7.
    uint8_t byte = im->pixeles[fila][indice_byte];
    //Tengo un byte, por ejemplo 0020 0110 y quiero obtener el tercer bit de izquierda a derecha. El bit mas a la izquierda es el mas significativo por ser formato little-endian.
    bool valor_pixel = (byte >> (7 - indice_bit)) & 0x01;
    return valor_pixel;
}

void imprimir_imagen_stdout(imagen_t *im){
    for (size_t i = 0; i < imagen_alto(im); i++) {
        for (size_t j = 0 ; j < imagen_ancho(im); j++) {
           printf("%c", imagen_get_pixel(im, i, j) ? ' ' : '*');
        }
       printf("\n");
    }
}

