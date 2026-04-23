LIB_DIR = libs

CFLAGS = -g -Wall -Werror -std=c99 -pedantic -I$(LIB_DIR)

all: procesar

procesar: procesar.o $(LIB_DIR)/matriz.o $(LIB_DIR)/imagen.o $(LIB_DIR)/tipografia.o $(LIB_DIR)/redNeuronal.o
	gcc -g procesar.o $(LIB_DIR)/matriz.o $(LIB_DIR)/imagen.o $(LIB_DIR)/tipografia.o $(LIB_DIR)/redNeuronal.o -o procesar -lm

procesar.o: procesar.c $(LIB_DIR)/matriz.h $(LIB_DIR)/imagen.h  $(LIB_DIR)/tipografia.h
	gcc $(CFLAGS) -c procesar.c

matriz.o: $(LIB_DIR)/matriz.c $(LIB_DIR)/matriz.h
	gcc $(CFLAGS) -c matriz.c

imagen.o: $(LIB_DIR)/imagen.c $(LIB_DIR)/imagen.h
	gcc $(CFLAGS) -c imagen.c

tipografia.o: $(LIB_DIR)/tipografia.c $(LIB_DIR)/tipografia.h
	gcc $(CFLAGS) -c tipografia.c

redNeuronal.o: $(LIB_DIR)/redNeuronal.c $(LIB_DIR)/redNeuronal.h
	gcc $(CFLAGS) -c redNeuronal.c
	
clean:
	rm *.o procesar