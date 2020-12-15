#include "cache.h"
#include "stdlib.h"
#include "string.h"
#include "stdbool.h"

#define UPPER_BYTE 0
#define WORD_SIZE 2 // cada palabra de 2 bytes

#define VALID 1
#define INVALID 0

#define DIRTY 1
#define CLEAN 0

#define SUCCESS 0
#define ERROR 1

#define MEMORY_SIZE 32768 //en cantidad de palabras
int16_t mainMemory[MEMORY_SIZE]; // Memoria principal de 64KB

/* ******************************************************************
 *                        FUNCIONES PRIVADAS
 * *****************************************************************/

/* La via es un array de palabras y el numero de bloque 
 * dentro de la via queda determinado por el tamanio del bloque.
 */

static int block_init(block_t *block,int bs) {
  block->words = malloc(sizeof(int16_t) * (bs / WORD_SIZE));
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
  unsigned int index = (address >> self->bits_offset);
  index <<= (sizeof(unsigned int) * 8 - self->bits_index);
  index >>= (sizeof(unsigned int) * 8 - self->bits_index);
  
  return index; 
}

static unsigned int find_offset_by_addr(cache_t* self,uint16_t address) {
  unsigned int offset =  address << (sizeof(unsigned int) * 8 - self->bits_offset);
  offset  >>= (sizeof(unsigned int) * 8 - self->bits_offset);
  
  return offset; 
}

static unsigned int get_memory_offset(cache_t* self,unsigned int blocknum) {
  return blocknum * self->block_size / WORD_SIZE;
}

static unsigned int find_tag_by_addr(cache_t* self,uint16_t address) {
  unsigned int tag = address >> (self->bits_index + self->bits_offset);
  return tag; 
}

static unsigned int get_set_offset(cache_t* self,unsigned int setnum) {
  return self->ways * setnum;
}

static unsigned int get_word_offset(unsigned int block_offset) {
  return block_offset / WORD_SIZE;
}

static unsigned int get_byte_offset(unsigned int block_offset) {
  return block_offset % WORD_SIZE;
}

static unsigned int get_blocknum_by_address(cache_t* self,uint16_t address) {
  return address / self->block_size;
}

//updatea distancia lru dentro del conjunto.
static void update_lru_distance(cache_t* self, block_t* being_used, int set_offset) {
  block_t* set = self->blocks + set_offset;

  for (int i = 0; i < self->ways; i++) {
    set[i].distance++;
  }
  being_used->distance = 0;
}

static unsigned int find_lru(cache_t* self,int set_offset) {
  //bloques que me tengo que desplzar.
  block_t* set = self->blocks + set_offset;

  unsigned int way = 0;

  for (unsigned int i = 0; i < self->ways; i++) {
    if(set[i].valid == INVALID) return i; 

    if(set[i].distance > set[way].distance) { 
      way = i;
    }
  }

  return way; 
}

//En los parametros me viene la via y el set como orden (1ero,2dp),no como su posicion.
static unsigned int is_dirty(cache_t* self,int way, int setnum) { 
  if(way > self->ways) return -1;
  
  int index = setnum * self->ways;
  //dentro del conjunto se le suma la via
  return self->blocks[index + way].dirty;
}

/* debe escribir en memoria los datos contenidos en el bloque setnum de la via way */
static int write_block(cache_t* self, int way, int setnum) {
  /* Obtenemos el contenido de la cache */
  block_t block = self->blocks[setnum * self->ways + way];

  if (!block.valid) return ERROR;

  //Me quedo con las palabras a escribir en memoria
  int16_t *words_to_write = block.words;

  //Armo la direccion de memoria principal del bloque a partir del tag y el index
  unsigned int address = block.tag;
  address = address << (self->bits_index + self->bits_offset); 
  address += (setnum << self->bits_offset);

  //Escribimos el bloque de cache en memoria
  unsigned int blocknum = get_blocknum_by_address(self,(uint16_t)address);

  int memory_offset =  get_memory_offset(self,blocknum);
  memcpy(mainMemory + memory_offset, words_to_write, self->block_size);

  //una vez que escribimos nuestro bloque en memoria, este vuelve a estar limpio
  block.dirty = CLEAN;

  return SUCCESS;
}

static int read_block(cache_t* self,int blocknum,int setnum,unsigned int tag) {
  self->missed_accesses ++;
  int set_offset = get_set_offset(self,setnum);
  //bloques que me tengo que desplzar.
  int way = find_lru(self,set_offset);
  
  block_t* block = self->blocks + set_offset + way;
  //Aca block es el bloque que va a ser reemplazado por lo que antes de hacer esto,debemos revisar
  //si el bit de dirty esta en 1 y en ese caso escribir el bloque en memoria
  if (is_dirty(self, way, setnum)) {
    if (write_block(self, way, setnum)) {
      return ERROR;
    }
  } 

  //copio el bloque a la cache en el conjunto correspondiente.
  block->valid = VALID;
  int memory_offset =  get_memory_offset(self,blocknum);
  memcpy(block->words, mainMemory + memory_offset , self->block_size);  

  //copio el tag del bloque.
  block->tag = tag;

  return SUCCESS;
}

/* ******************************************************************
 *                        PRIMITIVAS CACHE
 * *****************************************************************/

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

  self->bits_index = get_bits(self->blocks_len / self->ways);
  self->bits_offset = get_bits(self->block_size);
  self->bits_tag = 16 - self->bits_index - self->bits_offset;

  return SUCCESS;
}

char cache_read_byte(cache_t* self,uint16_t address) {
  self->total_accesses ++;
  unsigned int setnum = find_set_by_addr(self,address);
  
  //bloques que me tengo que desplzar.
  unsigned int set_offset = get_set_offset(self,setnum);
  
  block_t* set = self->blocks + set_offset;
  block_t* block = set;

  unsigned int tag = find_tag_by_addr(self,address);
  unsigned int block_offset = find_offset_by_addr(self,address);
  unsigned int word_offset = get_word_offset(block_offset);
  unsigned int byte_offset = get_byte_offset(block_offset);
  unsigned int blocknum = get_blocknum_by_address(self,address);

  char data = 0;
  bool found = false;

  for (int i=0; (i < self->ways) && (!found) ; i++) {
    block = set + i;
    if(block->tag == tag && block->valid == VALID) {
      //se encontro el bloque en la cache.
      //aca hay que leer el bloque teniendo en cuenta el offset.
      found = true;
      int16_t word = *(block->words + word_offset);

      if (byte_offset == UPPER_BYTE) {
        data =  (char)(word >> 8);
      } else {
        data =  (char)(word & 0xFF);
      }
    }
  }

  //hay que buscar el bloque en memoria.
  if (!found) {
    read_block(self,blocknum,setnum,tag);

    for (int i=0; (i < self->ways) && (!found) ; i++) {
      block = set + i;
      if(block->tag == tag) {
        int16_t word = *(block->words + word_offset);
        found = true;

        if (byte_offset == UPPER_BYTE) {
          data = (char)(word >> 8);
        } else {
          data = (char)(word & 0xFF);
        }
      }
    }
  }

  update_lru_distance(self,block,setnum);

  return data;
}

//Escribe el byte value en la posicion correcta del bloque que corresponde a address
void cache_write_byte(cache_t* self,uint16_t address, char value) {
  unsigned int tag = find_tag_by_addr(self,address);
  unsigned int block_offset = find_offset_by_addr(self,address);
  unsigned int word_offset = get_word_offset(block_offset);
  unsigned int byte_offset = get_byte_offset(block_offset);
  unsigned int setnum  = find_set_by_addr(self,address);
  //el offset del set en bloques
  unsigned int set_offset = get_set_offset(self,setnum);
  unsigned int blocknum = get_blocknum_by_address(self,address);


  block_t* set = self->blocks + set_offset;
  block_t* block = set;

  //Busco en cache
  self->total_accesses ++;
  bool found = false;

  //Una vez que estamos dentro del conjunto buscado nuestra cache funciona como una FA,por lo que nuestro
  //bloque puede estar en cualquiera de las 4 vias
  for (int i=0; (i < self->ways) && (!found) ; i++) {
    block = set + i;
    if(block->tag == tag && block->valid == VALID) {  
      //se encontro el bloque en la cache.
      found = true;
      
      //aca hay que escribir el value teniendo en cuenta el offset que esta en bytes.
      int16_t data = block->words[word_offset];
      if (byte_offset == UPPER_BYTE) {         
        data = (int16_t)((value << 8) | (data && 0xFF));
      } else {
        data = (int16_t)(value | (data && 0xFF00));
      }

      block->words[word_offset] = data;
      
      //Como escribi el bloque debo marcarlo como dirty
      block->dirty = DIRTY;
    }
  }
  
  //Caso miss:hay que buscar el bloque en memoria.
  if (!found) {
    read_block(self, blocknum,setnum,tag);

    for (int i=0; (i < self->ways) && (!found) ; i++) {
      block = set + i;
      if(block->tag == tag) {
        found = true;
        
        //aca hay que escribir el value teniendo en cuenta el offset que esta en bytes.

        int16_t data = block->words[word_offset];
        if (byte_offset == UPPER_BYTE) {         
          data = (int16_t)((value << 8) | (data && 0xFF));
        } else {
          data = (int16_t)(value | (data && 0xFF00));
        }

        block->words[word_offset] = data;
        //Como escribi el bloque debo marcarlo como dirty
        block->dirty = DIRTY;
      }
    }
  }

  update_lru_distance(self,block,setnum);
} 

float cache_get_miss_rate(cache_t* self) {
  if(self->total_accesses) {
    return (float)self->missed_accesses / (float)self->total_accesses;
  }
  return 0;
}

void cache_destroy(cache_t* self) {
  for (int i=0; i < self->blocks_len; i++) {
    block_destroy(self->blocks + i);
  }
}
