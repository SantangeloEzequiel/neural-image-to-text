#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <string.h>

uint32_t leer_32(FILE *f) {
    uint8_t b[4];
    fread(b, 1, 4, f);
    return b[0] << 24 | b[1] << 16 | b[2] << 8 | b[3];
}

void saltear(FILE *f, size_t n) {
    for(size_t i = 0; i < n; i++)
        fgetc(f);
}

int main(int argc, char *argv[]) {
    char *fn_out = NULL;

    if(argc != 1 && !strcmp(argv[1], "-h")) {
        fprintf(stderr, "Uso: %s <imagen>\n", argv[0]);
        return 1;
    }
    else if(argc != 1) {
        fn_out = argv[1];
    }

    FILE *fi = fopen("t10k-images.idx3-ubyte", "rb");
    if(fi == NULL) {
        fprintf(stderr, "No pudo abrirse %s\n", "t10k-images.idx3-ubyte");
        return 1;
    }
    FILE *fl = fopen("t10k-labels.idx1-ubyte", "rb");
    if(fl == NULL) {
        fclose(fi);
        fprintf(stderr, "No pudo abrirse %s\n", "t10k-labels.idx1-ubyte");
        return 1;
    }

    leer_32(fi);
    uint32_t n = leer_32(fi);
    uint32_t filas = leer_32(fi);
    uint32_t columnas = leer_32(fi);

    leer_32(fl);
    leer_32(fl);

    n = 5;
    uint8_t (*pixeles)[columnas * n] = malloc(n * filas * columnas);

    saltear(fi, filas * columnas * 1000);
    saltear(fl, 1000);

    srand(time(NULL));

    char number[n + 1];

    for(size_t i = 0; i < n; i++) {
        int r = rand() % 1000;
        saltear(fi, r * filas * columnas);
        saltear(fl, r);

        int n = fgetc(fl);
        printf("%d", n);
        number[i] = n + '0';

        for(size_t f = 0; f < filas; f++) {
            for(size_t c = 0; c < columnas; c++) {
                pixeles[f][c + i * columnas] = fgetc(fi) > 64 ? 1 : 0;
            }
        }
    }
    putchar('\n');

    number[n] = '\0';

    fclose(fi);
    fclose(fl);

    char fn[100];
    tmpnam(fn);
    FILE *fo = fopen(fn, "wt"); // Este no debería fallar ;)

    fprintf(fo, "P1\n%d %d\n", n * columnas, filas);
    for(size_t f = 0; f < filas; f++) {
        for(size_t c = 0; c < n * columnas; c++)
            fprintf(fo, "%d\n", pixeles[f][c]);
    }
    fclose(fo);
    free(pixeles);

    char buffer[200];
    sprintf(buffer, "convert %s -monochrome -depth 1 -define bmp:format=bmp3 %s.bmp", fn, fn_out ? fn_out : number);
    system(buffer);
    remove(fn);


    return 0;
}
