#ifndef __CACHE_H_
#define __CACHE_H_
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