#include<stdio.h>
#include<stdlib.h>
#include<stdbool.h>
#include<math.h>
#include<stdint.h>
#include "redNeuronal.h"
#define SIGMOIDEA_LIMIT 30

struct redNeuronal{ 
    matriz_t **capas;    
    matriz_t **pesos;
    size_t numero_capas; //numero de capas
    size_t *neuronas;   //arreglo de los tamanios de neuronas
    float (**funcion)(float); //arreglo de punteros a funciones
};

typedef enum {
    SIGMOIDEA = 0,
    RELU,
    IDENTIDAD,
    TANH
} funcion_activacion_t;

typedef float (*funcion_t)(float);

funcion_t funciones_activacion[] = {
    sigmoidea,      
    relu,             
    identidad,       
    tanh_activacion 
};

redNeuronal_t* redNeuronal_construir (size_t numero_capas, size_t *neuronas, float (**funcion)(float)){
    redNeuronal_t *red = malloc (sizeof(redNeuronal_t));
        if(red==NULL){
            fprintf(stderr,"fallo al crear red");
            return NULL;
        }

    red->numero_capas = numero_capas;

    red->neuronas = malloc(sizeof(size_t)*numero_capas);
    if (red->neuronas==NULL){
        fprintf(stderr, "fallo al crear neuronas");
        redNeuronal_destruir(red);
        return NULL;
        }

    for (size_t i = 0; i < numero_capas; i++){
        red->neuronas[i]=neuronas[i];
    }

    red->funcion= malloc(sizeof(float(*)(float)) * (numero_capas-1));
    if (red->funcion==NULL){
        fprintf(stderr,"fallo al crear funciones");
        redNeuronal_destruir(red); 
        return NULL;
    }

    for (size_t i = 0; i < numero_capas-1; i++){ //Siempre son uno menos que el numero de capas
        red->funcion[i]=funcion[i];
    }

    red->capas = malloc(sizeof(matriz_t*) * numero_capas);
    if (red->capas==NULL){
        printf("error en creacion de capas");
        redNeuronal_destruir(red); 
        return NULL;
    }

    red->pesos = malloc(sizeof(matriz_t*) * (numero_capas-1));
    if (red->pesos==NULL){
        fprintf(stderr, "error en creacion de pesos");
        redNeuronal_destruir(red);
        return NULL;
    }

    for (size_t i = 0; i < numero_capas; i++){
       red->capas[i] =_matriz_crear(1,neuronas[i]); 
        if(red->capas[i]==NULL){
            fprintf(stderr, "fallo al crear la capa %zu",i);
            redNeuronal_destruir(red);
            return NULL;
        }

        if (i<(numero_capas-1)){ //La salida no la extiendo
            if(!matriz_extender_columnas(red->capas[i])){
                redNeuronal_destruir(red);
                return NULL;
            }
            red->neuronas[i]++; //Neurona extra para el sesgo.
        }
   
    }

    for (size_t i = 0; i < numero_capas-1; i++){ //El numero de pesos sera el numero de capas -1
        red->pesos[i] =_matriz_crear_inicializada(red->neuronas[i],red->neuronas[i+1]);  
        //Los pesos tendran filas igual al numero de capas anterior, y columnas igual al numero de capas posterior
        //Las matrices de pesos ya se encuentran inicializadas. 
        if(red->pesos[i]==NULL){
            printf("fallo al crear la peso %zu",i);
            redNeuronal_destruir(red);
            return NULL;
        }
    }

    return red;
}

void redNeuronal_destruir(redNeuronal_t *red) {

    if (red->capas != NULL) {
        for (size_t i = 0; i < red->numero_capas; i++) {
            if (red->capas[i] != NULL) {
                matriz_destruir(red->capas[i]);
            }
        }
        free(red->capas); 
    }

    if (red->pesos != NULL) {
        for (size_t i = 0; i < red->numero_capas - 1; i++) {
            if (red->pesos[i] != NULL) {
                matriz_destruir(red->pesos[i]);
            }
        }
        free(red->pesos);
    }

    if (red->funcion != NULL) {
        free(red->funcion);
    }

    if (red->neuronas != NULL) {
        free(red->neuronas); 

    free(red);
    }
} 

matriz_t *redNeuronal_evaluar(redNeuronal_t *red, matriz_t *entrada){
    //Entrada es una matriz columnas. Tiene tanas columnas como pixeles tiene el caracter en la imagen.
    //Antes de ingresarla a la red, la extiendo y le agrego el sesgo. 
    size_t capa_salida = red->numero_capas - 1;
    if(!matriz_extender_columnas(entrada)){
        return NULL;
    } 

    if (!matriz_copiar(red->capas[0], entrada)){ 
        return NULL;
    }

    for (size_t i = 0; i < capa_salida; i++) {  
        matriz_setter(red->capas[i], matriz_filas(red->capas[i]) - 1, matriz_columnas(red->capas[i]) - 1, 1.0f);
        matriz_t *resultado = matriz_multiplicar(red->capas[i], red->pesos[i]);
        if (resultado == NULL) {
            return NULL;
        }
        matriz_aplicar(resultado, red->funcion[i]);
        matriz_t *aux = red->capas[i + 1];
        red->capas[i + 1] = resultado;
        matriz_destruir(aux);
    }
    return red->capas[red->numero_capas-1];
}



float calcular_error_cuadratico(const matriz_t *salida_deseada, const matriz_t *salida_obtenida){
    float error_cuadratico = 0.0f;
    for (size_t i = 0; i < matriz_filas(salida_deseada); i++) {
        for (size_t j = 0; j < matriz_columnas(salida_deseada); j++) {
            float salida_deseada_valor = matriz_obtener(salida_deseada, i, j);
            float salida_obtenida_valor = matriz_obtener(salida_obtenida, i, j);
            float diferencia = salida_obtenida_valor - salida_deseada_valor;
            error_cuadratico += (diferencia * diferencia);
        }
    }
    return 0.5 * error_cuadratico;
}

float derivada(float (*funcion)(float), float x) {
    if (funcion == sigmoidea) {
        return sigmoidea_derivada(x);
    } else if (funcion == relu) {
        return relu_derivada(x);
    } else if (funcion == tanh_activacion) {
        return tanh_activacion_derivada(x);
    } else if (funcion == identidad) {
        return identidad_derivada(x);
    }
    return 0.0f;
}

float entrenar(redNeuronal_t *red, const matriz_t *salida_deseada, matriz_t *entrada, float eta) {

    matriz_t *matriz_salida_red_obtenida = redNeuronal_evaluar(red, entrada); 

    if(matriz_salida_red_obtenida == NULL)return 0.0f;

    matriz_t **errores;
    errores = malloc(sizeof(matriz_t*) * (red->numero_capas-1));
    if (errores == NULL) {
        fprintf(stderr,"Error en la creación de errores\n");
        matriz_destruir(matriz_salida_red_obtenida);
        redNeuronal_destruir(red);
        return 0.0f;
    }

    for (size_t i = 0; i < red->numero_capas - 1; i++) { 
        errores[i] = _matriz_crear(1, red->neuronas[i+1]);
            if (errores[i] == NULL) {
                fprintf(stderr, "Error al crear la matriz de errores para la capa %zu\n", i);
                matriz_destruir(matriz_salida_red_obtenida);
                redNeuronal_destruir(red);
                return 0.0f; 
            }
    }

    size_t capa_salida = red->numero_capas - 1;         // N, es - 1 para los indices que empiezan en 0
    // Cálculo del error en la capa de salida
    size_t num_neuronas_salida = red->neuronas[capa_salida];
    for (size_t j = 0; j < num_neuronas_salida; j++) {
        float o_j = matriz_obtener(red->capas[capa_salida], 0, j);
        float o_j_esperada = matriz_obtener(salida_deseada, 0, j);
        float derivada_activacion = derivada(red->funcion[capa_salida -1], o_j);  //capa_salida-1 = f^(N-1)(o_j)'
        float epsilon_j = (o_j - o_j_esperada) * derivada_activacion;
    
        matriz_setter(errores[capa_salida-1], 0, j, epsilon_j);      //Guardamos el error de la capa de salida. para la matriz W n-1.
        size_t num_neuronas_entrada = red->neuronas[capa_salida - 1];  //Numero de neuronas en la capa I^N-1
        for (size_t i = 0; i < num_neuronas_entrada; i++) { //Para cada neurona de la capa anterior
            float salida_i = matriz_obtener(red->capas[capa_salida - 1], 0, i);
            float gradiente = salida_i * epsilon_j;
            float peso_actual = matriz_obtener(red->pesos[capa_salida - 1], i, j);
            matriz_setter(red->pesos[capa_salida - 1], i, j, peso_actual - eta * gradiente);
        }
    } //matriz de pesos W^N-1 actualizada.

    //errores[capa+1]=errores[capa_salida] tiene 1xnum_neuronas_salidas valores 
    //pesos[capa]=pesos[capa_salida-1] tiene num_neuronas_capa_anteriorxnum_neuronas_salidas 

    // Retropropagación del error a las capas ocultas.
    for (int capa = capa_salida - 1; capa >= 0; capa--) {   //Empiezo de la capa anteultima y bajo. aca puse int porque size_t nunca va a ser menor que 0. 
        size_t num_neuronas_capa = red->neuronas[capa];
        size_t num_neuronas_siguiente = red->neuronas[capa + 1]; 
        if (capa > 0) {
            for (size_t j = 0; j < num_neuronas_capa; j++) {
                float epsilon_j = 0.0f;
                for (size_t k = 0; k < num_neuronas_siguiente; k++) {
                    float epsilon_k = matriz_obtener(errores[capa], 0, k); //E(n+1,k)  ..deberian de tener el mismo indice. revisar.
                    float peso_jk = matriz_obtener(red->pesos[capa], j, k);         //(w_jk)^(n+1)
                    epsilon_j += epsilon_k * peso_jk;
                } //Esta seria la sumatoria para toda k que esta en la formula.
                
                    float salida_j = matriz_obtener(red->capas[capa], 0, j);        //(I_j)^n+1
                    float derivada_activacion = derivada(red->funcion[capa-1], (salida_j));
        
                    epsilon_j *= derivada_activacion;
                    matriz_setter(errores[capa-1], 0, j, epsilon_j);

                    for (size_t i = 0; i < red->neuronas[capa - 1]; i++) {
                        float salida_i = matriz_obtener(red->capas[capa - 1], 0, i);
                        float gradiente = salida_i * epsilon_j; 

                        // Actualizamos el peso
                        float peso_actual = matriz_obtener(red->pesos[capa - 1], i, j);
                        matriz_setter(red->pesos[capa - 1], i, j, peso_actual - eta * gradiente);
                    }
                
            }
        }
    }
        if (errores != NULL) {
        for (size_t i = 0; i < red->numero_capas - 1; i++) {
            if (errores[i] != NULL) {
                matriz_destruir(errores[i]);
            }
        }
        free(errores);
        }

    return calcular_error_cuadratico(salida_deseada, matriz_salida_red_obtenida);    
}

redNeuronal_t* redNeuronal_cargar(FILE *f) {

    redNeuronal_t *red = malloc(sizeof(redNeuronal_t));
    if (red == NULL) {
        fprintf(stderr, "Error al crear la estructura de la red neuronal\n");
        return NULL;
    }

    fread(&(red->numero_capas), sizeof(size_t), 1, f);

    red->neuronas = malloc(red->numero_capas * sizeof(size_t));
    if (red->neuronas == NULL) {
        fprintf(stderr, "Error al leer el arreglo de neuronas\n");
        free(red);
        return NULL;
    }
    fread(red->neuronas, sizeof(size_t), red->numero_capas, f);

    red->capas = malloc(red->numero_capas * sizeof(matriz_t*));
    red->pesos = malloc((red->numero_capas - 1) * sizeof(matriz_t*));

    if (red->capas == NULL || red->pesos == NULL) {
        fprintf(stderr, "Error al crear las matrices de la red neuronal\n");
        free(red->neuronas);
        free(red);
        return NULL;
    }

    for (size_t i = 0; i < red->numero_capas; i++) {
        red->capas[i] = matriz_leerBN(f);
        printf("Capa %zu = %zu neuronas\n", i, i == red->numero_capas-1 ? red->neuronas[i] : red->neuronas[i] - 1);
        if (red->capas[i] == NULL) {
            fprintf(stderr, "Error al leer la capa %zu\n", i);
            redNeuronal_destruir(red);
            return NULL;
        }
    }

    for (size_t i = 0; i < red->numero_capas - 1; i++) {
        red->pesos[i] = matriz_leerBN(f);
        if (red->pesos[i] == NULL) {
            fprintf(stderr, "Error al leer los pesos %zu\n", i);
            redNeuronal_destruir(red);
            return NULL;
        }
    }

    red->funcion = malloc((red->numero_capas - 1) * sizeof(float(*)(float)));
    if (red->funcion == NULL) {
        fprintf(stderr, "Error al leer las funciones de activación\n");
        redNeuronal_destruir(red);
        return NULL;
    }

    for (size_t i = 0; i < red->numero_capas - 1; i++) {
        funcion_activacion_t func_activacion_id;
        fread(&func_activacion_id, sizeof(funcion_activacion_t), 1, f);

        switch (func_activacion_id) {
            case SIGMOIDEA:
                red->funcion[i] = sigmoidea;
                printf("Activacion %zu = SIGMOIDEA\n", i);
                break;
            case RELU:
                red->funcion[i] = relu;
                printf("Activacion %zu = RELU\n", i);
                break;
            case IDENTIDAD:
                red->funcion[i] = identidad;
                printf("Activacion %zu = IDENTIDAD\n", i);
                break;
            case TANH:
                red->funcion[i] = tanh_activacion;
                printf("Activacion %zu = TANH\n", i);
                break;
            default:
                fprintf(stderr, "Función de activación desconocida: %d\n", func_activacion_id);
                redNeuronal_destruir(red);
                return NULL;
        }

    }
    
    return red;
}

bool redNeuronal_guardar(redNeuronal_t *red, FILE *f){

    fwrite(&(red->numero_capas), sizeof(size_t), 1, f);

    fwrite(red->neuronas, sizeof(size_t), red->numero_capas, f);

    for (size_t i = 0; i < red->numero_capas; i++){
        if (!matriz_escribirBN(red->capas[i], f)) return false;
    }

    for (size_t i = 0; i < red->numero_capas-1; i++){
        if (!matriz_escribirBN(red->pesos[i], f)) return false;
    }

    for (size_t i = 0; i < red->numero_capas - 1 ; i++){
        funcion_activacion_t func_activacion_id = SIGMOIDEA;
        if (red->funcion[i] == sigmoidea) {
            func_activacion_id = SIGMOIDEA;
        } else if (red->funcion[i] == relu) {
            func_activacion_id = RELU;
        } else if (red->funcion[i] == identidad) {
            func_activacion_id = IDENTIDAD;
        } else if (red->funcion[i] == tanh_activacion) {
            func_activacion_id = TANH;
        }
        fwrite(&func_activacion_id, sizeof(funcion_activacion_t), 1, f);
    }
    return true;
}

float sigmoidea(float x) {
    if (x > SIGMOIDEA_LIMIT) return 1.0f;
    if (x < -SIGMOIDEA_LIMIT) return 0.0f;
    return 1.0f / (1.0f + exp(-x));
}

float sigmoidea_derivada(float x) {
    float sigmoide = sigmoidea(x);
    return sigmoide * (1.0f - sigmoide);
}

float relu(float x) {
    return (x > 0.0f) ? x : 0.01f * x;
}

float relu_derivada(float x) {
    return (x > 0) ? 1.0f : 0.01f; 
}

float identidad(float x) {
    return x;
}

float identidad_derivada(float x) {
    return 1.0f;
}

float tanh_activacion(float x) {
    return tanhf(x);
}

float tanh_activacion_derivada(float x) {
    float tanh_x = tanh(x);
    return 1.0f - tanh_x * tanh_x;
}