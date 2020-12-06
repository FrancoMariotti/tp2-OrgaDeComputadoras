#ifndef __CACHE_H_
#define __CACHE_H_
#include "stdint.h"
/*Este modulo implementa una cache asociativa por conjuntos*/

typedef struct cache cache_t;
typedef struct block block_t;


/*Inicializa la cache */
int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs);
/*Devuelve el conjunto al que mapea la direccion */
unsigned int cache_find_set(cache_t* self,uint16_t address);
/*Devuelve el o los bloques menos recientemente usados */
unsigned int cache_find_lru(cache_t* self,int setnum);
/*Determina el estado del bit D del bloque pasado por parametro  */
unsigned int cache_is_dirty(cache_t* self,int way, int setnum);
/*Almacena en la cache el bloque de memoria pasado por parametro*/
void cache_read_block(cache_t* self,int blocknum);
/* */
void cache_write_block(cache_t* self,int way, int setnum);
/* */
char cache_read_byte(cache_t* self,int address);
/* */
void cache_write_byte(cache_t* self,int address, char value);
/* */
int cache_get_miss_rate(cache_t* self);
/*Destruye la cache */
void cache_destroy(cache_t* self);

#endif