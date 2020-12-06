#ifndef __CACHE_H_
#define __CACHE_H_
/*Este modulo implementa una cache asociativa por conjuntos*/

typedef struct cache cache_t;

/*Inicializa la cache */
void cache_init(cache_t self);
/*Devuelve el conjunto al que mapea la direccion */
unsigned int cache_find_set(uint16_t address);
/*Devuelve el o los bloques menos recientemente usados */
unsigned int cache_find_lru(int setnum);
/*Determina el estado del bit D del bloque pasado por parametro  */
unsigned int cache_is_dirty(int way, int setnum);
/*Almacena en la cache el bloque de memoria pasado por parametro*/
void cache_read_block(int blocknum);
/* */
void cache_write_block(int way, int setnum);
/* */
char cache_read_byte(uint16_t address);
/* */
void cache_write_byte(uint16_t address, char value);
/* */
int cache_get_miss_rate();

#endif