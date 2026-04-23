
#include<stdint.h>
#include <stddef.h>
#include<math.h>
#include "matriz.h"

//MATRIZ_T
struct matriz{
        float **m; //matriz
        size_t fs; //filas
        size_t cs; //columnas
    };

    //DESTRUCCION

    void matriz_destruir(matriz_t *matriz){


        for (size_t i = 0; i < matriz->fs; i++)
        {
            free(matriz->m[i]);
        }
        free(matriz->m);
        free (matriz);
    }


        //CREACION 


    matriz_t *_matriz_crear(size_t n, size_t m){ //n = filas  m = columnas

        matriz_t *matriz = malloc(sizeof(matriz_t));
        if (matriz==NULL) return NULL;

       
        matriz->m = malloc(n * sizeof(float*));
        if(matriz->m==NULL)
        {
            free(matriz); //libero la estructura que ya cree antes
            return NULL;
        }
       

        for (size_t i = 0; i < n; i++)
        {
            matriz->m[i]=malloc(m * sizeof(float));
           
            if (matriz->m[i]==NULL)
            {
                matriz->fs = i; //Pongo que las filas sean las que creo hasta el momento para no liberar memoria que no es mia
                matriz_destruir(matriz);
                return NULL;
            }
            
        }

        matriz->fs = n; //Inicializo Filas 
        matriz->cs = m; //Inicializo Columnas
        
        return matriz;
    }


     matriz_t *_matriz_crear_inicializada(size_t n, size_t m){
         matriz_t *matriz = _matriz_crear(n,m);
         if (matriz==NULL)return NULL;
         inicializar_random(matriz); //En este paso crear matriz ya verifica que sea una matriz valida
         return matriz;
        }

        //GETTERS


    size_t matriz_filas(const matriz_t *matriz){ return matriz->fs;}

    size_t matriz_columnas(const matriz_t *matriz){ return matriz->cs;}

    void matriz_dimensiones(const matriz_t *matriz, size_t *filas, size_t *columnas){
        *filas = matriz->fs;
        *columnas = matriz->cs;
    }
    
    float matriz_obtener(const matriz_t *matriz, size_t fila, size_t columna){ return matriz->m[fila][columna];}


    bool matriz_copiar(const matriz_t *matrizDestino , const matriz_t *matrizOrigen){
        if (matrizOrigen->cs!=matrizDestino->cs || matrizOrigen->fs!=matrizDestino->fs )
        {return false;} //verifico si son del mismo tamanio

        //copio
        for (size_t i = 0; i < matrizDestino->fs; i++) {
            for (size_t j = 0; j < matrizDestino->cs; j++) {
                matrizDestino->m[i][j]= matrizOrigen->m[i][j];
            }
        }

        return true;
        
}


        //GENERACION DE MATRICES

    matriz_t *matriz_leer(size_t n, size_t m){

       matriz_t *matriz = _matriz_crear(n,m);
       if (matriz==NULL){printf("error al leer matriz");return NULL;} 

       char buffer[200];
        //printf("\nmatriz de %zu X %zu\n",n,m);
        for (size_t i = 0; i < matriz->fs; i++)
        {

            if(fgets(buffer,200,stdin)==NULL){
                printf("Error al leer la fila %zu\n",i);
                matriz_destruir(matriz);
                return NULL;
            } //Leo la fila completa separada por espacios

            char *aux = buffer;
            for (size_t j = 0; j < matriz->cs; j++)
            {
               matriz->m[i][j] = strtod(aux,&aux);

               // Verificar que strtod haya convertido un número válido
             if (aux == buffer) {
                    printf("Error en la conversión de valor en fila %zu, columna %zu\n", i + 1, j + 1);
                    matriz_destruir(matriz);  // Liberar memoria si strtod no convierte
                    return NULL;
                }
            }

        }
        
        return matriz;
    }

        //EXTENSION DE MATRICES

    //FILAS A 1

    bool matriz_extender_filas(matriz_t *matriz){

        float **aux= realloc(matriz->m,(matriz->fs+1) * sizeof(float*)); //agrego una fila
            if (aux==NULL) //verifico
            {
                printf("error al extender filas de matriz");
                return false;
            }


        matriz->m = aux;

        //Y ahora agrego todas las columnas de la fila
        matriz->m[matriz->fs]= malloc(matriz->cs * sizeof(float));

        if ((matriz->m[matriz->fs]==NULL))
        {
            printf("error al extender filas de matriz");
            return false;
        }
        

        for (size_t i = 0; i < matriz->cs; i++)
        {
            matriz->m[matriz->fs][i]=1.0f;
        }
        

        matriz->fs++; //anoto que le agregue una fila

        return true;

    }

    //COLUMNAS A 1

    bool matriz_extender_columnas(matriz_t *matriz){


        for (size_t i = 0; i < matriz->fs; i++)
        {
            float *aux = realloc(matriz->m[i],(matriz->cs+1) * sizeof(float));

            if (aux==NULL)
            {
                printf("error al extender columnas de matriz");
                return false;
            }

            matriz->m[i]=aux;
            matriz->m[i][matriz->cs]=1.0f;
        }

        matriz->cs++;

        return true;

    }


    void matriz_imprimir(const matriz_t *matriz){
        for (size_t i = 0; i < matriz->fs; i++) {
            for (size_t j = 0; j < matriz->cs; j++) {
                printf("%f\t", matriz->m[i][j]);         
             //printf("%c", (matriz->m[i][j]==1) ? ' ' : '*');
            }
            printf("\n");
        }
    }

    void matriz_setter(matriz_t *matriz, size_t fila, size_t columna, float valor) {

        if (fila >= matriz_filas(matriz) || columna >= matriz_columnas(matriz)){
            return;
        }

    matriz->m[fila][columna] = valor;
    }

    void matriz_escribirtxt(matriz_t *m, FILE *f){
        for (size_t i = 0; i < m->fs; i++) {
            for (size_t j = 0; j < m->cs; j++) {
                fputc((int)(m->m[i][j]), f);
              // fputc((m->m[i][j]==1) ? ' ' : '*',f);
            }
            fputc('\n', f);
        }
    }

     // APLICACIONES SUELTAS

    //MULTIPLICAR

    matriz_t *matriz_multiplicar(const matriz_t *a, const matriz_t *b){
        if (a->cs != b->fs)
        {
            printf("multiplicacion no valida, dimensiones incorrectas");
            return NULL;
        }
        
        matriz_t *matriz = _matriz_crear(a->fs,b->cs);

        if (matriz==NULL)return NULL; //verifico

        
        //multiplico
        for (size_t i = 0; i < matriz->fs; i++)
        {
            for (size_t j = 0; j < matriz->cs; j++)
            {

                matriz->m[i][j]=0; //inicializo en 0 antes de la multiplicacion

                for (size_t k = 0; k < a->cs; k++) //a->cs == b->fs
                {
                    matriz->m[i][j]+= a->m[i][k] * b->m[k][j]; //k es la columna que multiplica a cada fila y voy sumando en cada una
                }
                
            }
            
        }
        
        return matriz;
    }


    //APLICAR FUNCION

    void matriz_aplicar(matriz_t *m, float (*funcion)(float)){
        for (size_t i = 0; i < m->fs; i++)
        {
            for (size_t j = 0; j < m->cs; j++)
            {
                m->m[i][j]= funcion(m->m[i][j]);
            }
        }

    }   

//INICIALIZAR MATRIZ

void inicializar_random(matriz_t *m){
    float x = sqrt(6.0f / (m->fs + m->cs));
    for (size_t i = 0; i < m->fs; i++)
        {
            for (size_t j = 0; j < m->cs; j++){
                m->m[i][j]= -x + (2.0f * x * ((float)rand() / (float)RAND_MAX));
            }
        }
        return;
    }   


//LEER Y ESCRIBIR
//los primeros sizeof(size_t) bytes del archivo seran el numero de filas, y los proximos el de columnas.
// despues los siguietes sizeof(float) seran cada float de la matriz en orden 00 01 02 ... [fs][cs]

 bool matriz_escribirBN(matriz_t *m, FILE *f){
    fwrite(&(m->fs),sizeof(size_t),1,f);
    fwrite(&(m->cs),sizeof(size_t),1,f);

    for (size_t i = 0; i < m->fs; i++)
        {
            fwrite(m->m[i],sizeof(float),m->cs,f);
            /*
            for (size_t j = 0; j < m->cs; j++)
            {
                fwrite(&m->m[i][j],sizeof(float),1,f)//uso float de 32 bits
            }*/
        }

        return true;
 }


 matriz_t* matriz_leerBN(FILE *f){
    
    size_t fs;
    fread(&fs,sizeof(size_t),1,f);
    
    size_t cs;
    fread(&cs,sizeof(size_t),1,f);

    matriz_t *m= _matriz_crear(fs,cs);
    if (m==NULL) return NULL;
    
    for (size_t i = 0; i < fs; i++)
    {
        fread(m->m[i],sizeof(float),cs,f);
    }
    
    return m;
 }

