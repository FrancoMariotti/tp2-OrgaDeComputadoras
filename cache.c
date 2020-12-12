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
  int index =  address >> self->bits_offset;
  index = index << (self->bits_offset + self->bits_tag);
  index = index >> (self->bits_tag + self->bits_offset);
  
  return index; 
}

static int find_set_by_blocknum(cache_t* self,int blocknum) {
  if(!self->ways) return -1;

  return blocknum % (self->blocks_len / self->ways);
}

static unsigned int find_offset_by_addr(cache_t* self,uint16_t address) {
  int offset =  address << (self->bits_tag + self->bits_index);
  offset = offset >> (self->bits_tag + self->bits_index);
  
  return offset; 
}

static unsigned int find_tag_by_addr(cache_t* self,uint16_t address) { 
  return (address >> (self->bits_index + self->bits_offset)); 
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
    if(set[i].valid == INVALID) return i; 

    if(set[i].distance > set[way].distance) { 
      way = i;
    }
  }

  return way; 
}

//REVISARLO AL PROGRAMAR EL WRITE
static unsigned int is_dirty(cache_t* self,int way, int setnum) { // EN LOS PAR ME VIENE LA VIA Y EL SET COMO ORDEN(1ERO ,2DO)
  if(way > self->ways) return -1;
  
  int index = (setnum - 1) * self->ways - 1;
  //dentro del conjunto se le suma la via
  return self->blocks[index + way].dirty;
}

/* debe escribir en memoria los datos contenidos en el bloque setnum de la via way */
static int write_block(cache_t* self, int way, int setnum) {
  /* Obtenemos el contenido de la cache */
  block_t block = self->blocks[(setnum - 1) * self->ways + (way - 1)];

  if (!block.valid) return ERROR;

  //Me quedo con las palabras a escribir en memoria
  int16_t *words_to_write = block.words;

  //Armo la direccion de memoria principal del bloque a partir del tag y el index
  int address = block.tag;
  address = address << (self->bits_index + self->bits_offset); 
  address += (setnum << self->bits_offset);

  //int block_offset = setnum * self->ways + (way - 1);

  //Escribimos el bloque de cache en memoria
  memcpy(&address, words_to_write, self->block_size);

  //una vez que escribimos nuestro bloque en memoria, este vuelve a estar limpio
  block.dirty = CLEAN;

  return SUCCESS;
}

static void read_block(cache_t* self,int blocknum) {
  //Aca hay que tener en cuenta la politica de reemplazo LRU
  //cache[conjunto][via] = mainMemory[blocknum]
  //set = blocknum % sets
  self->missed_accesses ++;
  int setnum  = find_set_by_blocknum(self,blocknum);
  //bloques que me tengo que desplzar.
  int set_offset = self->ways * setnum;
  int way = find_lru(self,setnum);

  //ACA POR LO QUE ENTIENDO BLOCK ES EL BLOQUE EN CACHE QUE SE VA A REEMPLAZAR POR LO QUE HABRIA QUE CHEQUEAR SI 
  //ESTA DIRTY Y SI ES VALIDO, EN CASO AFIRMATIVO VOLVER A ESCRIBIRLO EN MEMORIA
  //LO DE SI ES VALIDO NO ESTOY SEGURO SI HACE FALTA CHEQUEARLO
  block_t* block = self->blocks + set_offset + way;
  if (is_dirty(self, way, setnum) && block->valid == VALID) write_block(self, way, setnum);

  //copio el bloque a la cache en el conjunto correspondiente.
  block->valid = VALID;
  int memory_offset =  blocknum * self->block_size / WORD_SIZE;
  memcpy(block->words, mainMemory + memory_offset , self->block_size);  

  //copio el tag del bloque.
  uint16_t block_address = (uint16_t)(blocknum * self->block_size);
  block->tag = find_tag_by_addr(self,block_address);
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

  self->bits_offset = get_bits(self->blocks_len / self->ways);
  self->bits_index = get_bits(self->block_size);
  self->bits_tag = WORD_SIZE * 8 - self->bits_index - self->bits_offset;

  return SUCCESS;
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
  int set_offset = self->ways * setnum;
  
  block_t* set = self->blocks + set_offset;
  block_t* block = NULL;
  unsigned int tag = find_tag_by_addr(self,address);

  unsigned int block_offset = find_offset_by_addr(self,address);
  int word_offset = block_offset / WORD_SIZE;
  int byte_offset = block_offset % WORD_SIZE;
  int blocknum = address / self->block_size;
  char data = 0;
  bool found = false;

  //for (int i=0; i < self->ways; i++) {
  for (int i=0; (i < self->ways) || (!found) ; i++) {
    block = set + i;
    if(block->tag == tag && block->valid == VALID) {
      //se encontro el bloque en la cache.
      //aca hay que leer el bloque teniendo en cuenta el offset.
      found = true;
      int16_t word = *(block->words + word_offset);

      if (byte_offset == UPPER_BYTE) {
        data =  word >> 8;
      } else {
        data =  word & 0xFF;
      }
    }
  }

  //hay que buscar el bloque en memoria.
  if (!found) {
    read_block(self,blocknum);
    for (int i=0; i < self->ways; i++) {
    //no es necesario la condicion !found porque se ejecuto el read_block de arriba
    //for (int i=0; (i < self->ways) || (!found) ; i++) {
      block = set + i;
      if(block->tag == tag) {
        int16_t word = *(block->words + word_offset);

        if (byte_offset == UPPER_BYTE) {
          data = word >> 8;
        } else {
          data = word & 0xFF;
        }
      }
    }
  }

  update_lru_distance(self,block,setnum);

  return data;
}

//Escribe el byte value en la posicion correcta del bloque que corresponde a address
void cache_write_byte(cache_t* self,uint16_t address, char value) {
  //Para este hay que implementar WB/WA
  
  unsigned int tag = find_tag_by_addr(self,address);
  //suponemos que el offset de la dir de memoria es un unico campo para bytes
  unsigned int offset = find_offset_by_addr(self,address);
  unsigned int setnum  = find_set_by_addr(self,address);
  //el offset del set en bloques
  unsigned int set_offset = setnum * self->ways;

  block_t* set = self->blocks + set_offset;
  block_t* block = NULL;

  //Busco en cache
  self->total_accesses ++;
  bool found = false;

  //Una vez que estamos dentro del conjunto buscado nuestra cache funciona como una FA,por lo que nuestro
  //bloque puede estar en cualquiera de las 4 vias
  for (int i=0; (i < self->ways) || (!found) ; i++) {
    block = set + i;
    if(block->tag == tag && block->valid == VALID) {
      //se encontro el bloque en la cache.
      found = true;
      //aca hay que escribir el value teniendo en cuenta el offset que esta en bytes.
      *((char*)block->words + offset) = value;
      //Como escribi el bloque debo marcarlo como dirty
      block->dirty = DIRTY;
    }
  }
  
  //Caso miss:hay que buscar el bloque en memoria.
  if (!found) {
    unsigned int blocknum = address / self->block_size;  
    read_block(self, blocknum);
    /* Aca pasa lo mismo. Si ya ejecutaste el read_block de arriba no tiene sentido hacer un !found
     * porque el  bloque esta en memoria. Por esta razon !found = true siempre.
     */
    //for (int i=0; (i < self->ways) || (!found) ; i++) {
    for (int i=0; i < self->ways; i++) {
      block = set + i;
      if(block->tag == tag) {
        //aca hay que escribir el value teniendo en cuenta el offset que esta en bytes.
        //esto queda muy feo de leer habria que mejorarlo.
        *((char*)block->words + offset) = value;
        //Como escribi el bloque debo marcarlo como dirty
        block->dirty = DIRTY;
      }
    }
  }

  update_lru_distance(self,block,setnum);
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
