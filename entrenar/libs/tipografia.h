#ifndef   TIPOGRAFIA_H
#define   TIPOGRAFIA_H
#include<stdio.h>
#include "matriz.h"
#include "imagen.h"

typedef struct tipografia tipografia_t;
//Devuelve el ancho en pixeles del caracter.
size_t tipografia_ancho_caracter(tipografia_t *tipografia);

//Devuelve el alto en pixeles del caracter.
size_t tipografia_alto_caracter(tipografia_t *tipografia);

//Devuelve la cantidad de pixeles contenidos en un caracter de la imagen BMP. Es la cantidad de entradas de la red neuronal.
size_t tipografia_cantidad_pixeles_caracter(tipografia_t *tipografia);

//Devuelve la cantidad de caracteres del archivo txt. Es la cantidad de caracteres que habran contenidos en el archivo.
size_t tipografia_cantidad_caracteres(tipografia_t *tipografia);

//Obtiene el índice del carácter correspondiente.
int tipografia_caracter(tipografia_t *tipografia, size_t i);

//Extrae un caracter de la imagen BMP. Devuelve una matriz_t de 1xpixeles totales.
matriz_t *matriz_extraer_caracter(imagen_t *im, tipografia_t *tipografia, size_t numero_caracter_x, size_t numero_caracter_y);

//Devuelve el numero de representaciones de un caracter. Es la cantidad de salidas que tendrá la red neuronal.
size_t tipografia_cantidad_representaciones(tipografia_t *tipografia);

//Obtiene la representación gráfica del carácter.
char *tipografia_representacion_caracter(tipografia_t *tipografia, size_t i);

//Codifica la representacion del caracter en una matriz_t de 1xcolumnas. Columnas es igual a la cantidad de caracteres en la imagen.
matriz_t *codificar_salida(tipografia_t *tipografia, const int caracter);

//Devuelve el índice con mayor grado de activación. 
int decodificar_salida(matriz_t *salida_red, tipografia_t *tipografia);

//Lee un archivo de texto txt en el formato dado y devuelve un tipografia_t.
tipografia_t *tipografia_crear_desde_txt(FILE *f);

//Libera la memoria asignada para tipografia_t.
void tipografia_destruir(tipografia_t *tipografia);


#endif


/*
4 10 20         tipografia_cantidad_caracteres=4, tipografia_ancho_caracteres=10, tipografia_alto_caracteres=20
0               Tipografia_caracter(0)
1               Tipografia_caracter(1)
2               Tipografia_caracter(2)
3               Tipografia_caracter(3)  
A               tipografia_representacion_caracter(0)
a               tipografia_representacion_caracter(1)
Ñ               tipografia_representacion_caracter(2)
ñ               tipografia_representacion_caracter(3)
tipografia_cantidad_representaciones=4 

4 10 20         tipografia_cantidad_caracteres=4, tipografia_ancho_caracteres=10, tipografia_alto_caracteres=20
0               Tipografia_caracter(0)
0               Tipografia_caracter(1)
1               Tipografia_caracter(2)
1               Tipografia_caracter(3)  
A               tipografia_representacion_caracter(0)
Ñ               tipografia_representacion_caracter(1)
tipografia_cantidad_representaciones = 2 
 */