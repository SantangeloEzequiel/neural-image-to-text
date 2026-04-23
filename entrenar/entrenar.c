#include "libs/imagen.h"
#include "libs/tipografia.h"
#include "libs/redNeuronal.h"
#include <string.h>
#define MAX_BUFFER 256

bool archivo_bin_existe(const char *f) {
    FILE *archivo = fopen(f, "rb");
    if (archivo==NULL) {
        return false;
    }
    fclose(archivo);
    return true;
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "uso: <./programa> <nombre_tipografia> <iteraciones> <grado_aprendizaje>\n");
        return 1;
    }
    char *nombre_tipografia = argv[1];
    int iteraciones = atoi(argv[2]);
    float eta = atof(argv[3]);
    
    char bmp_file[MAX_BUFFER];
    char txt_file[MAX_BUFFER];
    char bin_file[MAX_BUFFER];
    //Tanto el archivo txt, el bmp y .bin tienen el mismo nombre

    strcpy(txt_file, nombre_tipografia);
    strcat(txt_file, ".txt");

    strcpy(bmp_file, nombre_tipografia);
    strcat(bmp_file, ".bmp");

    strcpy(bin_file, nombre_tipografia);
    strcat(bin_file, ".bin");

    FILE *entrada_texto=fopen(txt_file,"r");                      //Abre el archivo de texto que acompaña las tipografias
    if(entrada_texto==NULL){
        fprintf(stderr, "No pudo abrirse: %s\n", txt_file);
        return 1;
        }

    tipografia_t *tipografia = tipografia_crear_desde_txt(entrada_texto);
    if(tipografia == NULL){
        fprintf(stderr, "No pudo generarse la tipografia.");
        fclose(entrada_texto);
        return 1;
    }
    
    FILE *entrada_imagen=fopen(bmp_file,"r");                      //Abre el archivo bmp con las tipografias
    if(entrada_imagen==NULL){
        fprintf(stderr, "No pudo abrirse: %s\n", bmp_file);
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        return 1;
    }    

    imagen_t *imagen = imagen_desde_archivo_BMP(entrada_imagen);   //Carga la imagen bmp con las tipografias
    if(imagen == NULL){
        fprintf(stderr, "No pudo leerse la imagen, ¿es un BMP monocromático?\n.");
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        fclose(entrada_imagen);
        return 1;
    }
    redNeuronal_t *red_neuronal; 
    FILE *salida_bin;
    if (!archivo_bin_existe(bin_file)) {
        size_t numero_capas;
        size_t *neuronas;
        float (**funciones_activacion)(float);
        printf("Red no entrenada, configurar la red:\n");
        printf("Numero de capas: ");
        numero_capas = 0;
        char aux[MAX_BUFFER];
        while (fgets(aux, 100, stdin) != NULL) {
            numero_capas = atoi(aux);
            if (numero_capas >= 3) break;
            else printf("Error, el numero de capas debe ser de al menos 3. Intente nuevamente: ");
        }

        neuronas = malloc(numero_capas * sizeof(size_t));
        if (neuronas == NULL) {
            fprintf(stderr, "Error al asignar memoria para las neuronas.\n");
            tipografia_destruir(tipografia);
            fclose(entrada_texto);
            fclose(entrada_imagen);
            return 1;
        }

        for (int i = 1; i < numero_capas - 1; i++) {
            printf("Numero de neuronas para la capa I^%d: ",i);
            if (fgets(aux, MAX_BUFFER, stdin) != NULL) {
                neuronas[i] = atoi(aux);
            }
        }

        printf("Activaciones: 0 -> Sigmoidea, 1 -> ReLU, 2 -> Identidad, 3 -> Tanh\n");
        funciones_activacion= malloc(sizeof(float (*)(float)) * (numero_capas - 1));
            if (funciones_activacion == NULL) {
                fprintf(stderr, "Error al asignar memoria para funciones de activación.\n");
                free(neuronas);
                tipografia_destruir(tipografia);
                fclose(entrada_texto);
                fclose(entrada_imagen);
                return 1;
            }

    for (size_t i = 0; i < numero_capas - 1; i++) {
        int opcion;
        printf("Activación para la capa I^%zu -> I^%zu: ", i, i + 1);


        if (fgets(aux, MAX_BUFFER, stdin) != NULL) {
            opcion = atoi(aux);

            if (opcion == 0) {
                funciones_activacion[i] = sigmoidea;
            } else if (opcion == 1) {
                funciones_activacion[i] = relu;
            } else if (opcion == 2) {
                funciones_activacion[i] = identidad;
            } else if (opcion == 3) {
                funciones_activacion[i] = tanh_activacion;
            } else {
                printf("Opción no válida. Usamos Sigmoidea por defecto.\n");
                funciones_activacion[i] = sigmoidea;
            }
        } else {
            printf("Entrada no válida. Usando Sigmoidea por defecto.\n");
            funciones_activacion[i] = sigmoidea;
        }
    }
    
    salida_bin=fopen(bin_file, "wb");
    if(salida_bin == NULL){
        fprintf(stderr, "Error al crear archivo bin: %s\n", bin_file);
        free(funciones_activacion);
        free(neuronas);
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        fclose(entrada_imagen);
        return 1;
    } 

    neuronas[0]=tipografia_cantidad_pixeles_caracter(tipografia);       //cantidad de entradas de la red
    neuronas[numero_capas-1]=tipografia_cantidad_representaciones(tipografia); //cantidad de salidas de la red

    red_neuronal = redNeuronal_construir(numero_capas, neuronas, funciones_activacion);
    if(red_neuronal==NULL){
        fprintf(stderr, "No se pudo crear la red neuronal");
        fclose(salida_bin);
        free(funciones_activacion);
        free(neuronas);
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        fclose(entrada_imagen);
        return 1;
    }
    free(funciones_activacion);
    free(neuronas);

    }else{
        printf("Cargando la red desde el archivo binario: %s\n", bin_file);
        salida_bin=fopen(bin_file, "rb");
        if(salida_bin == NULL){
            fprintf(stderr, "Error al abrir archivo bin: %s\n", bin_file);
            return 1;
        }
        red_neuronal = redNeuronal_cargar(salida_bin);
        fclose(salida_bin);
        salida_bin=fopen(bin_file, "wb");
        if(salida_bin == NULL){
            fprintf(stderr, "Error al abrir archivo bin: %s\n", bin_file);
            return 1;
        }
    }

    for(size_t epoch=0; epoch < iteraciones; epoch ++){
        float l = 0.0f; 
        for (size_t x = 0; x < tipografia_cantidad_caracteres(tipografia); x++) {
    
            char caracter = tipografia_caracter(tipografia, x); 
            if(caracter==-1){
                fclose(salida_bin);
                tipografia_destruir(tipografia);
                imagen_destruir(imagen);
                fclose(entrada_texto);
                fclose(entrada_imagen);
                redNeuronal_destruir(red_neuronal);
                return 1;
            }

            matriz_t *matriz_salida_red_esperada = codificar_salida(tipografia, caracter);

            if (matriz_salida_red_esperada == NULL) {
                fclose(salida_bin);
                tipografia_destruir(tipografia);
                imagen_destruir(imagen);
                fclose(entrada_texto);
                fclose(entrada_imagen);
                redNeuronal_destruir(red_neuronal);
                return 1;
            }

            matriz_t *pixeles_caracter = matriz_extraer_caracter(imagen, tipografia, x, 0);    //En este caso estoy analizando imagenes de una sola fila
            if (pixeles_caracter == NULL) {
                fclose(salida_bin);
                tipografia_destruir(tipografia);
                imagen_destruir(imagen);
                fclose(entrada_texto);
                fclose(entrada_imagen);
                redNeuronal_destruir(red_neuronal);
                matriz_destruir(matriz_salida_red_esperada);
                return 1;
            }

            l += entrenar(red_neuronal, matriz_salida_red_esperada, pixeles_caracter, eta);
            matriz_destruir(matriz_salida_red_esperada);
            matriz_destruir(pixeles_caracter);
        }
        printf("L= %f\n", l);
    }
    
    if(redNeuronal_guardar(red_neuronal, salida_bin)){
        printf("Red neuronal guardada!\n");
    }

    fclose(salida_bin);
    tipografia_destruir(tipografia);
    imagen_destruir(imagen);
    fclose(entrada_texto);
    fclose(entrada_imagen);
    redNeuronal_destruir(red_neuronal);
    return 0;
}

