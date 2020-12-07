#ifndef __CACHE_H_
#define __CACHE_H_
#include "stdint.h"

typedef struct block {
  int tag;
  int16_t *words;
  unsigned char valid:1;
  unsigned char dirty:1;
  //para lru
  char last_accessed;
} block_t;

typedef struct cache {
  int ways;
  int size;
  int block_size;
  int total_accesses;
  int missed_accesses;
  int blocks_len;
  block_t * blocks;
} cache_t;


/*Inicializa la cache */
int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs);
/*Devuelve el conjunto al que mapea la direccion */
unsigned int cache_find_set(cache_t* self,uint16_t address);
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
/**/
int cache_get_blocks(int cs,int bs);
/*Destruye la cache */
void cache_destroy(cache_t* self);

#endif