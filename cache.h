#ifndef __CACHE_H_
#define __CACHE_H_
/*Este modulo implementa una cache asociativa por conjuntos*/

typedef struct cache cache_t;

/*Inicializa la cache */
void init();
/*Devuelve el conjunto al que mapea la direccion */
unsigned int find_set(int address);
/*Devuelve el o los bloques menos recientemente usados */
unsigned int find_lru(int setnum);
/*Determina el estado del bit D del bloque pasado por parametro  */
unsigned int is_dirty(int way, int setnum);
/*Almacena en la cache el bloque de memoria pasado por parametro*/
void read_block(int blocknum);
/* */
void write_block(int way, int setnum);
/* */
char read_byte(int address);
/* */
void write_byte(int address, char value);
/* */
int get_miss_rate();

#endif