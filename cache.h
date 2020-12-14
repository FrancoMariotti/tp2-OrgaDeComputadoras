#ifndef CACHE_H
#define CACHE_H
#include "stdint.h"

typedef struct block {
  int tag;
  int16_t *words;
  unsigned char valid:1;
  unsigned char dirty:1;
  //para lru
  int distance;
} block_t;

typedef struct cache {
  int ways;
  int size;
  int block_size;
  int bits_offset;
  int bits_index;
  int bits_tag;
  int total_accesses;
  int missed_accesses;
  int blocks_len;
  block_t * blocks;
} cache_t;

/*Inicializa la cache */
int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs);
/* PRE: la cache fue creada.
 * Lee el byte referenciado por address del sistema de memoria simulado 
 */
char cache_read_byte(cache_t* self,uint16_t address);
/* PRE: la cache fue creada.
 * Escribe el byte value en la posicion de memoria referenciada por address 
 */
void cache_write_byte(cache_t* self,uint16_t address, char value);
/* PRE: la cache fue creada.
 * Devuelve la tasa de miss de la cache simulada 
 */
float cache_get_miss_rate(cache_t* self);
/* Dado un tamanio de cache cs y un tamanio de bloque bs determina la 
 * cantidad de bloques que tendria la cache. 
 */
int cache_get_blocks(int cs,int bs);
/*Destruye la cache */
void cache_destroy(cache_t* self);
#endif