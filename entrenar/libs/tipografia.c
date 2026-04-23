#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include "tipografia.h"

#define MAX_BUFFER 100

struct tipografia{
    size_t datos_cabecera[3]; //datos_cabecera[0] = cantidad de caracteres, [1] = ancho en pixeles, [2] = alto en pixeles
    int *caracter;            //Indica que caracter es.
    size_t cantidad_representacion_caracter;        //Cuantas representaciones de un caracter.
    char **representacion_caracter;                 //Representacion del caracter
};

size_t tipografia_cantidad_caracteres(tipografia_t *tipografia){
    return tipografia->datos_cabecera[0];
}
size_t tipografia_ancho_caracter(tipografia_t *tipografia){
    return tipografia->datos_cabecera[1];
}
size_t tipografia_alto_caracter(tipografia_t *tipografia){
    return tipografia->datos_cabecera[2];
}

size_t tipografia_cantidad_pixeles_caracter(tipografia_t *tipografia){
    return tipografia_ancho_caracter(tipografia)*tipografia_alto_caracter(tipografia);
}

int tipografia_caracter(tipografia_t *tipografia, size_t i){
    if(i >= tipografia_cantidad_caracteres(tipografia))return -1;
    return tipografia->caracter[i];
}

size_t tipografia_cantidad_representaciones(tipografia_t *tipografia){
    return tipografia->cantidad_representacion_caracter;
}

char *tipografia_representacion_caracter(tipografia_t *tipografia, size_t i){
    if(i >= tipografia_cantidad_representaciones(tipografia))return NULL;
    return tipografia->representacion_caracter[i];
}

tipografia_t *tipografia_crear_desde_txt(FILE *f){
    tipografia_t *tipografia = (tipografia_t *)malloc(sizeof(tipografia_t));
    if (tipografia == NULL){
        return NULL;
    }

    char buffer[MAX_BUFFER];
    size_t aux = 0;
    char ch;

    for (size_t i = 0; i < 3; i++){  
        aux = 0;
        while ((ch = fgetc(f)) != EOF && ch != ' ' && ch != '\n') {
            buffer[aux++] = ch;
        }
        buffer[aux] = '\0';

        char *ptr;
        tipografia->datos_cabecera[i] = strtol(buffer, &ptr, 10);
        if (ptr == buffer) {
            free(tipografia);
            return NULL;
        }
        if (ch == '\n') break;
    }
   
    size_t cantidad_caracteres = tipografia->datos_cabecera[0];
    tipografia->caracter = malloc(cantidad_caracteres * sizeof(int));
    if (tipografia->caracter == NULL){
        free(tipografia);
        return NULL;
    }

    for (size_t i = 0; i < cantidad_caracteres; i++){
        if (fgets(buffer, MAX_BUFFER, f) == NULL) {
            free(tipografia->caracter);
            free(tipografia);
            return NULL;
        }
        tipografia->caracter[i] = atoi(buffer);
    }

    tipografia->representacion_caracter = malloc(sizeof(char*));
    if (tipografia->representacion_caracter == NULL){
        free(tipografia->caracter);
        free(tipografia);
        return NULL;
    }

    //La cantidad representaciones de un caracter puede ser menor que la cantidad caracteres
     size_t leidos = 0;
     size_t capacidad_actual = 1;

    while (leidos < cantidad_caracteres){

        if (leidos >= capacidad_actual) {
            capacidad_actual++;
            char **aux = realloc(tipografia->representacion_caracter, capacidad_actual * sizeof(char*));
            if (aux == NULL) {
                printf("Error al extender filas de la matriz\n");
                for (size_t j = 0; j < leidos; j++) {
                    free(tipografia->representacion_caracter[j]);
                }
                free(tipografia->representacion_caracter);
                free(tipografia->caracter);
                free(tipografia);
                return NULL;
            }
            tipografia->representacion_caracter = aux;
        }


        if (fgets(buffer, sizeof(buffer), f) == NULL){
            break;
        }

        buffer[strcspn(buffer, "\n")] = '\0';

        tipografia->representacion_caracter[leidos] = malloc(strlen(buffer) + 1);
        if (tipografia->representacion_caracter[leidos] == NULL) {
            for (size_t j = 0; j < leidos; j++) {
                free(tipografia->representacion_caracter[j]);
            }
            free(tipografia->representacion_caracter);
            free(tipografia->caracter);
            free(tipografia);
            return NULL;
        }

        strcpy(tipografia->representacion_caracter[leidos], buffer);
        leidos++;
    }
    tipografia->cantidad_representacion_caracter= leidos;

    return tipografia;
}

void tipografia_destruir(tipografia_t *tipografia){
    free(tipografia->caracter);
    for (size_t i = 0; i < tipografia->cantidad_representacion_caracter; i++){
        free(tipografia->representacion_caracter[i]);
    }
    free(tipografia->representacion_caracter);
    free(tipografia);
}

matriz_t *matriz_extraer_caracter(imagen_t *im, tipografia_t *tipografia, size_t numero_caracter_x, size_t numero_caracter_y){
    size_t columna_inicio = numero_caracter_x * tipografia_ancho_caracter(tipografia);
    size_t fila_inicio = numero_caracter_y * tipografia_alto_caracter(tipografia);

    matriz_t *matriz = _matriz_crear(1, tipografia_cantidad_pixeles_caracter(tipografia));
    if (matriz == NULL){
            return NULL; 
    }

    size_t indice = 0;
    for (size_t i = 0; i < tipografia_alto_caracter(tipografia); i++){
        for (size_t j = 0; j < tipografia_ancho_caracter(tipografia); j++) {
            bool pixel = imagen_get_pixel(im, fila_inicio + i, columna_inicio + j);
            matriz_setter(matriz, 0, indice, (float)pixel);
            indice++;
        }
    }
    return matriz;
}

matriz_t *codificar_salida(tipografia_t *tipografia, const int caracter){
    size_t cantidad_caracteres = tipografia_cantidad_representaciones(tipografia);
    matriz_t *matriz = _matriz_crear(1, cantidad_caracteres);

    if (matriz == NULL){
        return NULL; 
    }

    for (size_t i = 0; i < cantidad_caracteres; i++){
        matriz_setter(matriz, 0, i, 0.0f);
    }
    for (size_t i = 0; i < tipografia_cantidad_caracteres(tipografia); i++){
        if (tipografia_caracter(tipografia, i) == caracter) {
            matriz_setter(matriz, 0, caracter, 1.0f);
            break;
        }
    }

    return matriz;
}

int decodificar_salida(matriz_t *salida_red, tipografia_t *tipografia){
    size_t cantidad_caracteres = tipografia_cantidad_representaciones(tipografia);
    int indice_max = -1;
    float max_activacion = -1.0f;

    for (size_t i = 0; i < cantidad_caracteres; i++){
        float activacion = matriz_obtener(salida_red, 0, i);
        if (activacion > max_activacion){
            max_activacion = activacion;
            indice_max = i; 
        }
    }

    return indice_max;
}