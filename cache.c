#include "cache.h"
#include "stdlib.h"

#define MAIN_MEMORY 32768 //en cantidad de palabras
#define WORD_SIZE 2 // cada palabra de 2 bytes

#define VALID 1
#define INVALID 0

#define SUCCESS 0
#define ERROR 1

int16_t memPrincipal[MAIN_MEMORY]; // Memoria principal de 64KB

/* La via es un array de palabras y el numero de bloque 
 * dentro de la via queda determinado por el tamanio del bloque.
 */
typedef struct block {
  int tag;
  int16_t *words;
  char valid;
  char dirty;
  //para lru
  char last_accessed;
} block_t;

struct cache {
  int ways;
  int size;
  int block_size;
  int miss_rate;
  block_t * blocks;
};

int cache_amount_blocks(int cs,int bs) {
  return cs / bs;
}

static int initialize_block(block_t *block,int bs) { 
  block->words = (int16_t*) malloc(bs);
  if(!block->words) return ERROR;

  block->dirty = 0;
  block->valid = INVALID;
  block->last_accessed = 0;
  return SUCCESS;
}

static void blocks_destroy(block_t *blocks,int cs,int bs) {
  int amount_blocks = cache_amount_blocks(cs,bs);
  
  for (int i=0; i < amount_blocks; i++) {
    if(blocks[i].words) {
      free(blocks[i].words);
    }
  }
}

static int amount_bits(int num) {
  int amount_bits;
  for(amount_bits = 0, (amount_bits >> 1) > 1; amount_bits++);
  
  return amount_bits
}

/* Pre: La estructura cache fue inicializada
 */
int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs) {
  //inicializa la memoria en cero.
  memset(memPrincipal, 0, WORD_SIZE * MAIN_MEMORY);

  for (size_t i = 0; i < self->size; i++) {
    if (initialize_block(self->blocks + i,bs)) {
      blocks_destroy(self->blocks,cs,bs);
      return ERROR;
    }
  }
 
  //incializo frecuencia de miss en cero.
  self->miss_rate = 0;
  self->size = cs;
  self->block_size = bs;

  return SUCCESS;
}

unsigned int cache_find_set(cache_t* self,uint16_t address) {
  //address -> tag | index | offset
  //index -> me determinan el conjunto 
  int amount_blocks = cache_amount_blocks(self->size, self->block_size);
  int bits_index = amount_bits(amount_blocks);
  int bits_offset = amount_bits(self->block_size);
  int bits_tag = sizeof(address) - bits_index - bits_offset;

  int index =  address >> bits_offset;
  index = (index << bits_offset) << bits_index;
  
  return (index >> bits_tag);
}

unsigned int cache_find_lru(cache_t* self,int setnum) {

}

unsigned int cache_is_dirty(cache_t* self,int way, int setnum) {
  //chequear si el bit de dirty en el bloque esta en 1
  int index = (setnum - 1) * self->ways - 1;
  return self->blocks[index].dirty;
}

void cache_read_block(cache_t* self,int blocknum) {

}

void cache_write_block(cache_t* self,int way, int setnum) {

}

char cache_read_byte(cache_t* self,int address) {

}

void cache_write_byte(cache_t* self,int address, char value) {

} 

int cache_get_miss_rate(cache_t* self) {
  return self->miss_rate;
}

void cache_destroy(cache_t* self) {
  blocks_destroy(self->blocks,self->size,self->block_size);
}
