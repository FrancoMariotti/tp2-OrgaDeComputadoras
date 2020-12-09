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

static int find_set_by_blocknum(cache_t* self,int blocknum) {
  if(!self->ways) return -1;

  return blocknum % (self->blocks_len / self->ways);
}

static int block_init(block_t *block,int bs) { 
  block->words = (int16_t*) malloc(bs);
  if(!block->words) {
    return ERROR;
  }

  block->dirty = 0;
  block->valid = INVALID;
  block->distance = 0;
  return SUCCESS;
}

static void block_destroy(block_t *block) {
  if(block->words) {
    free(block->words);
  }
  block->dirty = -1;
  block->distance = -1;
  block->tag = -1;
  block->valid = -1;
}

static int get_bits(int num) {
  int bits;
  for(bits = 0; (num = num >> 1) >= 1; bits++) {}  
  
  return bits;
}

static unsigned int find_set_by_addr(cache_t* self,uint16_t address) {
  unsigned int bits_index = get_bits(self->blocks_len / self->ways); 
  //bits_offset = F -> 2^F = block_size
  unsigned int bits_offset = get_bits(self->block_size);
  //bits_tag = 16 bits - bits_index - bits_offset
  unsigned int bits_tag = WORD_SIZE * 8 - bits_index - bits_offset;

  int index =  address >> bits_offset;
  index = (index << bits_offset) << bits_tag;
  
  return (index >> (bits_tag + bits_offset)); 
}

static unsigned int find_block_by_addr(cache_t* self,uint16_t address) {
  unsigned int bits_index = get_bits(self->blocks_len / self->ways); 
  //bits_offset = F -> 2^F = block_size
  unsigned int bits_offset = get_bits(self->block_size);
  
  return (address >> (bits_index + bits_offset)); 
}

//updatea distancia lru dentro del conjunto.
static void update_lru_distance(cache_t* self, block_t* being_used, int setnum) {
  int set_offset = self->ways * setnum; 
  block_t* set = self->blocks + set_offset;

  for (int i = 0; i < self->ways; i++) {
    set[i].distance++;
  }
  being_used->distance = 0;
}

static unsigned int find_lru(cache_t* self,int setnum) {
  //bloques que me tengo que desplzar.
  int set_offset = self->ways * setnum; 
  block_t* set = self->blocks + set_offset;

  unsigned int way = 0;

  for (unsigned int i = 0; i < self->ways; i++) {
    if(!set[i].valid) return i; 

    if(set[i].distance > set[way].distance) { 
      way = i;
    }
  }

  return way; 
}

int cache_get_blocks(int cs,int bs) {
  return (cs * 1024) / bs;
}

int cache_init(cache_t* self,block_t *blocks,int ways,int cs,int bs) {
  //inicializa la memoria en cero.
  memset(mainMemory, 0, WORD_SIZE * MEMORY_SIZE);
  self->blocks_len = cache_get_blocks(cs, bs);

  bool error = false;

  for (int i=0; i < self->blocks_len; i++) {
    if (block_init(blocks + i,bs) == ERROR) {   
      error = true;
    }
  }

  if(error) {
    cache_destroy(self);
    return ERROR;
  }

  self->blocks = blocks;
  self->ways = ways;
  self->total_accesses = 0;
  self->missed_accesses = 0;
  self->size = cs;
  self->block_size = bs;

  return SUCCESS;
}

//REVISARLO AL PROGRAMAR EL WRITE
unsigned int cache_is_dirty(cache_t* self,int way, int setnum) { // EN LOS PAR ME VIENE LA VIA Y EL SET COMO ORDEN(1ERO ,2DO)
  if(way > self->ways) return -1;
  
  int index = (setnum - 1) * self->ways - 1;
  //dentro del conjunto se le suma la via
  return self->blocks[index + way].dirty;
}

void cache_read_block(cache_t* self,int blocknum) {
  //Aca hay que tener en cuenta la politica de reemplazo LRU
  //cache[conjunto][via] = mainMemory[blocknum]
  //set = blocknum % sets
  self->missed_accesses ++;
  int setnum  = find_set_by_blocknum(self,blocknum);
  //bloques que me tengo que desplzar.
  int offset = self->ways * setnum;
  int way = find_lru(self,setnum);

  block_t* block = self->blocks + offset + way;
  block->valid = VALID;
  //copio el bloque a la cache en el conjunto correspondiente.
  int memory_offset = blocknum * self->block_size / WORD_SIZE;
  memcpy(block->words, mainMemory + memory_offset , self->block_size);
}

char cache_read_byte(cache_t* self,uint16_t address) {
  //Aca se usa la funcion read_block en caso de haber un miss.
  self->total_accesses ++;
  //Para esta funcion hay que primero buscar el 
  //bloque que queremos, si lo encontramos leemos 
  //el byte correspondiente y listo. En caso de 
  //no estar hay que traer el bloque de la memoria 
  //(esto modifica el miss rate)y leer el byte
  //y finalmente updatear distancias lru.
  unsigned int setnum = find_set_by_addr(self,address);
  
  //bloques que me tengo que desplzar.
  int offset = self->ways * setnum;
  int way = find_lru(self,setnum);

  block_t* set = self->blocks + offset + way;
  block_t* block = NULL;
  unsigned int tag = get_tag_by_addr(address);
  
  for (int i=0; i < self->ways; i++) {  
    if(set[i].tag == tag) {
      //se encontro el bloque en la cache.
    }
  }

  //hay que buscar el bloque en memoria.

  update_lru_distance(self,block,setnum);

  return 'a';
}

void cache_write_block(cache_t* self,int way, int setnum) {
  
}

void cache_write_byte(cache_t* self,uint16_t address, char value) {
  //Para este hay que implementar WB/WA
} 

int cache_get_miss_rate(cache_t* self) {
  if(self->total_accesses) {
    return self->missed_accesses / self->total_accesses;
  }
  return 0;
}

void cache_destroy(cache_t* self) {
  for (int i=0; i < self->blocks_len; i++) {
    block_destroy(self->blocks + i);
  }
}
