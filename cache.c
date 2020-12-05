#include "cache.h"

#define MEM_PRINCIPAL 4096

uint16_t memPrincipal[MEM_PRINCIPAL]; // Memoria principal de 64KB

/*El tamanio de palabra que utilizamos es de  2 bytes/16 bits*/

/* La via es un array de palabras y el numero de bloque 
dentro de la via queda determinado por el tamanio del bloque.*/

typedef struct bloque {
    uint16_t tag;
    uint16_t *palabras;
} bloque_t;

struct cache {
    int cVias;
    int tamanio;
    bloque_t *bloque;
};

void cache_init(cache_t cache) {

}

unsigned int cache_find_set(int address) {

}

unsigned int cache_find_lru(int setnum) {

}

unsigned int cache_is_dirty(int way, int setnum) {

}

void cache_read_block(int blocknum) {

}

void cache_write_block(int way, int setnum) {

}

char cache_read_byte(int address) {

}

void cache_write_byte(int address, char value) {

} 

int cache_get_miss_rate() {

}
