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
typedef struct bloque {
  int tag;
  int16_t *words;
  char valid;
  char dirty;
  //para lru
  char last_accessed;
} bloque_t;

struct cache {
  int ways;
  int size;
  int miss_rate;
  bloque_t *bloques;
};

/* Pre: La estructura cache fue inicializada
 */
int cache_init(cache_t* self) {
  //incializo frecuencia de miss en cero.
  self->miss_rate = 0;
  //inicializa la memoria en cero.
  memset(memPrincipal,0,WORD_SIZE*MAIN_MEMORY);

  self->bloques = (bloque_t*)malloc(sizeof(bloque_t)*self->size);

  if(!self->bloques) return ERROR;

  for (size_t i = 0; i < size; i++) {
    self->bloques[i]->dirty = 0;
    self->bloques[i]->valid = INVALID;
    self->bloques[i]->last_accessed = 0;
  }

  return SUCCESS;
}

unsigned int cache_find_set(cache_t* self,int address) {

}

unsigned int cache_find_lru(cache_t* self,int setnum) {

}

unsigned int cache_is_dirty(cache_t* self,int way, int setnum) {

}

void cache_read_block(cache_t* self,int blocknum) {

}

void cache_write_block(cache_t* self,int way, int setnum) {

}

char cache_read_byte(cahce_t* self,int address) {

}

void cache_write_byte(cache_t* self,int address, char value) {

} 

int cache_get_miss_rate(cache_t* self) {
  return self->miss_rate;
}

void cache_destroy(cache_t* self) {
  free(self->bloques);
}
