#include "cache.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#define MEMORY_SIZE 32768 //en cantidad de palabras
#define WORD_SIZE 2 // cada palabra de 2 bytes

#define VALID 1
#define INVALID 0

#define SUCCESS 0
#define ERROR 1

int16_t mainMemory[MEMORY_SIZE]; // Memoria principal de 64KB

/* La via es un array de palabras y el numero de bloque 
 * dentro de la via queda determinado por el tamanio del bloque.
 */

int cache_get_blocks(int cs,int bs) {
  return (cs * 1024) / bs;
}

static int block_init(block_t *block,int bs) { 
  block->words = (int16_t*) malloc(bs);
  if(!block->words) {
    return ERROR;
  }

  block->dirty = 0;
  block->valid = INVALID;
  block->last_accessed = 0;
  return SUCCESS;
}

static void block_destroy(block_t *block) {
  if(block->words) {
    free(block->words);
  }
  block->dirty = -1;
  block->last_accessed = -1;
  block->tag = -1;
  block->valid = -1;
}

static int get_bits(int num) {
  int bits;
  for(bits = 0; (num = num >> 1) > 1; bits++) {}
  
  return bits;
}

int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs) {
  //inicializa la memoria en cero.
  memset(mainMemory, 0, WORD_SIZE * MEMORY_SIZE);
  self->blocks_len = cache_get_blocks(cs, bs);

  bool error = false;

  for (int i=0; i < self->blocks_len; i++) {
    if (block_init(blocks + i,bs)) {
      error = true;
    }
  }

  if(error) {
    cache_destroy(self);
    return ERROR;
  }

  self->blocks = blocks;
  self->ways = ways;
  self->miss_rate = 0;
  self->size = cs;
  self->block_size = bs;

  return SUCCESS;
}

unsigned int cache_find_set(cache_t* self,uint16_t address) {
  unsigned int bits_index = get_bits(self->blocks_len);
  //bits_offset = F -> 2^F = block_size
  unsigned int bits_offset = get_bits(self->block_size);
  //bits_tag = 16 bits - bits_index - bits_offset
  unsigned int bits_tag = WORD_SIZE * 8 - bits_index - bits_offset;

  int index =  address >> bits_offset;
  index = (index << bits_offset) << bits_tag;
  
  return (index >> bits_tag);
}

static unsigned int find_lru(block_t *blocks,int setnum) {
  return 1;
}

unsigned int cache_is_dirty(cache_t* self,int way, int setnum) {
  if(way > self->ways) return -1;
  
  int index = (setnum - 1) * self->ways - 1;
  //dentro del conjunto se le suma la via
  return self->blocks[index + way].dirty;
}

void cache_read_block(cache_t* self,int blocknum) {
  //Aca hay que tener en cuenta la politica de reemplazo LRU
  //cache[conjunto][via] = mainMemory[blocknum]
  //set = blocknum % sets
  int set  = blocknum % (self->blocks_len / self->ways);
  //bloques que me tengo que desplzar.
  int offset = self->ways * set;
  int way = find_lru(self->blocks,set);

  block_t* block = self->blocks + offset + way;
  memcpy(block->words, mainMemory , self->block_size);
}

void cache_write_block(cache_t* self,int way, int setnum) {

}

char cache_read_byte(cache_t* self,int address) {
  return 'a';
}

void cache_write_byte(cache_t* self,int address, char value) {

} 

int cache_get_miss_rate(cache_t* self) {
  return self->miss_rate;
}

void cache_destroy(cache_t* self) {
  for (int i=0; i < self->blocks_len; i++) {
    block_destroy(self->blocks + i);
  }
}
