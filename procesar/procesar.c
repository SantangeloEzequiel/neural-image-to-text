#include "libs/imagen.h"
#include "libs/tipografia.h"
#include "libs/redNeuronal.h"
#include <string.h>
#define MAX_BUFFER 256

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "uso: <./programa> <nombre_tipografia> <imagen>\n");
        return 1;
    }
    char *nombre_tipografia = argv[1];  //Para leer los datos de la tipografia.
    char *bmp_reconocer_texto = argv[2];         //Es el .bmp del que se va a extraer el texto.
    
    char txt_file[MAX_BUFFER];
    char bin_file_red[MAX_BUFFER];
    char bin_file_entrada[MAX_BUFFER];
    //Tanto el archivo txt, el bmp y .bin tienen el mismo nombre

    strcpy(txt_file, nombre_tipografia);
    strcat(txt_file, ".txt");

    strcpy(bin_file_red, nombre_tipografia);
    strcat(bin_file_red, ".bin");

    strcpy(bin_file_entrada, bmp_reconocer_texto);
    strcat(bin_file_entrada, ".bmp");

    FILE *entrada_texto=fopen(txt_file,"r");                      //Abre el archivo de texto que acompaña las tipografias
    if(entrada_texto==NULL){

        fprintf(stderr, "No pudo abrirse: %s\n", txt_file);
        return 1;
        }

    tipografia_t *tipografia = tipografia_crear_desde_txt(entrada_texto);   //Lee datos de la tipografia (SE PODRAN GUARDAR ESTOS DATOS EN EL PROCESO DE ENTRENAMIENTO??)
    if(tipografia == NULL){
        fprintf(stderr, "No pudo generarse la tipografia.");
        fclose(entrada_texto);
        return 1;
    }

    FILE *entrada_imagen=fopen(bin_file_entrada,"r");            //Abre el archivo bmp con el texto a reconocer
    if(entrada_imagen==NULL){
        fprintf(stderr, "No pudo abrirse: %s\n",bin_file_entrada);
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        return 1;
    }    

    imagen_t *imagen = imagen_desde_archivo_BMP(entrada_imagen);   //Carga la imagen bmp con el texto a reconocer
    if(imagen == NULL){
        fprintf(stderr, "No pudo leerse la imagen, ¿es un BMP monocromático?\n.");
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        fclose(entrada_imagen);
        return 1;
    }

    printf("Cargando la red desde el archivo binario: %s\n", bin_file_red);
        FILE *redneuronal_bin=fopen(bin_file_red, "rb");
        if(redneuronal_bin==NULL){
            fprintf(stderr, "Error al abrir archivo bin: %s\n", bin_file_red);
            imagen_destruir(imagen);
            tipografia_destruir(tipografia);
            fclose(entrada_texto);
            fclose(entrada_imagen);
            return 1;
        }
    
    redNeuronal_t *red_neuronal = redNeuronal_cargar(redneuronal_bin); //Cargamos los datos de la red desde el .bin
   
    if(red_neuronal==NULL){
        fprintf(stderr, "No se pudo crear la red neuronal");
        fclose(redneuronal_bin);
        imagen_destruir(imagen);
        tipografia_destruir(tipografia);
        fclose(entrada_texto);
        fclose(entrada_imagen);
        return 1;
    }

    size_t cantidad_caracteres_linea = imagen_ancho(imagen) / tipografia_ancho_caracter(tipografia);
    size_t cantidad_filas = imagen_alto(imagen) / tipografia_alto_caracter(tipografia);

    for(size_t y = 0; y < cantidad_filas; y++){
        matriz_t *pixeles_caracter;
        matriz_t *matriz_salida_red_obtenida;
        for (size_t x = 0; x < cantidad_caracteres_linea; x++) {
            pixeles_caracter = matriz_extraer_caracter(imagen, tipografia, x, y);    //En este caso estoy analizando imagenes de una sola fila
            if (pixeles_caracter == NULL) {
                redNeuronal_destruir(red_neuronal);
                fclose(redneuronal_bin);
                imagen_destruir(imagen);
                tipografia_destruir(tipografia);
                fclose(entrada_texto);
                fclose(entrada_imagen);
                return 1;
            }

            matriz_salida_red_obtenida = redNeuronal_evaluar(red_neuronal, pixeles_caracter);
            
            if(matriz_salida_red_obtenida == NULL){
                    fprintf(stderr, "Error al aplicar red neuronal");
                    matriz_destruir(pixeles_caracter);
                    redNeuronal_destruir(red_neuronal);
                    fclose(redneuronal_bin);
                    imagen_destruir(imagen);
                    tipografia_destruir(tipografia);
                    fclose(entrada_texto);
                    fclose(entrada_imagen);
                    return 1;
            }
        int indice_reconocido = decodificar_salida(matriz_salida_red_obtenida, tipografia);
        char *caracter_reconocido = tipografia_representacion_caracter(tipografia, indice_reconocido);
        printf("%s", caracter_reconocido);     
        matriz_destruir(pixeles_caracter);
        }
        printf("\n");
    }

    redNeuronal_destruir(red_neuronal);
    fclose(redneuronal_bin);
    imagen_destruir(imagen);
    tipografia_destruir(tipografia);
    fclose(entrada_texto);
    fclose(entrada_imagen);
    return 0;
}

