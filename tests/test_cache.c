#include "../cache.h"
#include <stdio.h>
#include <assert.h>
#include <string.h>

#define EXIT_FAILURE 1
#define SUCCESS 0

#define CACHE_SIZE 8 
#define BLOCK_SIZE 32
#define WAYS 4

extern int16_t mainMemory[];

//Pruebas inicializacion de la cache

int test00LaCacheSeInicializaCorrectamente(cache_t* self, block_t* blocks){
  cache_init(self, blocks, WAYS, CACHE_SIZE, BLOCK_SIZE);

  assert(self->ways == WAYS);
  assert(self->size == CACHE_SIZE);
  assert(self->block_size == BLOCK_SIZE);
  assert(self->bits_offset == 5); // 2^5 = 32
  assert(self->bits_index == 6); // 2^6 = 64 conjuntos de cache
  assert(self->bits_tag == 5); // 16 bits por direccion - 5  bits de offset - 6 bits de index = 5 
  assert(self->total_accesses == 0);
  assert(self->missed_accesses == 0);
  assert(self->blocks_len == 256);
  
  for (int i = 0; i < 256; i++) {
  	block_t block = self->blocks[i];
  	assert(block.valid == 0);
  	assert(block.distance == 0);
  	assert(block.dirty == 0);
  }

  printf("La cache se inicializa correctamente: OK\n");
  return SUCCESS;
}  

//Pruebas de escritura en cache

int test00AlEscribirUnDatoQueNoEstaEnCacheHayMissYSeEscribeElBloqueEnCache(cache_t* self){
	uint16_t address = 1;
	assert(self->missed_accesses == 0);
	cache_write_byte(self, address, 10);
	assert(mainMemory[0] != 10);
	assert(self->total_accesses == 1);
	assert(self->missed_accesses == 1);
	char value = cache_read_byte(self,address);
	assert(self->total_accesses == 2);
	assert(self->missed_accesses == 1);
	assert(value == 10);

	printf("Al escribir un dato que no esta en cache hay miss y se escribe en cache : OK\n");
	return SUCCESS;
}

int test01AlEscribirUnDatoQueEstaEnCacheHayHitYSeEscribeElBloqueEnCache(cache_t* self){
	uint16_t address = 1;
	assert(self->missed_accesses == 1);
	assert(self->total_accesses == 2);
	cache_write_byte(self, address, 20);
	assert(mainMemory[0] != 20);
	assert(self->total_accesses == 3);
	assert(self->missed_accesses == 1);
	char value = cache_read_byte(self,address);
	assert(self->missed_accesses == 1);
	assert(value == 20);

	printf("Al escribir un dato que esta en cache hay hit y se escribe en cache : OK\n");
	return SUCCESS;
}

int test02AlQuitarUnBloqueDeCacheEsteSeEscribeEnMemoria(cache_t* self){
	uint16_t address = 1;
	int total_sets = 64;
	char value = 30;
	//Llenamos el conjunto 0 de cache y al traer el ultimo bloque de la iteracion
	//se reemplaza el bloque de la dir 0 por ser el LRU y este escribe en memoria
	for (char i = 0; i < 4; i ++) {
		address = (uint16_t)(address + total_sets * WAYS * BLOCK_SIZE);
		value = (char)(value + i);
		cache_write_byte(self,address, value);
	}
	assert(mainMemory[0] == 20);
	char byte = cache_read_byte(self,address);
	assert(byte != 20);

	printf("Al quitar un bloque de cache que habia sido escrito (dirtybit == 1), este se escribe en mem ppal : OK\n");
	return SUCCESS;
}

//Pruebas de lectura en cache

int test00AlLeerUnDatoQueNoEstaEnCacheHayMissYSeLeeElDatoCorrectamente(cache_t* self){
	uint16_t address = 0;
	mainMemory[0] = 10; 
	char result = cache_read_byte(self, address);
	assert(self->total_accesses == 1);
	assert(self->missed_accesses == 1);
	assert(result = 10);

	printf("Al leer un dato que no esta en cache hay miss y se lee el dato correctamente : OK\n");
	return SUCCESS;
}

int test01AlLeerUnDatoQueEstaEnCacheHayHitYSeLeeElDatoCorrectamente(cache_t* self){
	uint16_t address = 0;
	char result = cache_read_byte(self, address);
	assert(self->total_accesses == 2);
	assert(self->missed_accesses == 1);
	assert(result = 10);

	printf("Al leer un dato que esta en cache hay hit y se lee el dato correctamente : OK\n");
	return SUCCESS;
}

//Prueba de miss rate
int test00SeCalculaElMissRateCorrectamente(cache_t* self){
	float result = cache_get_miss_rate(self);
	assert(result == 0.5);

	printf("El Miss Rate se calcula correctamente: OK\n");
	return SUCCESS;
}

int tests(){
	cache_t cache;
	int len_blocks = cache_get_blocks(CACHE_SIZE,BLOCK_SIZE);
	block_t blocks[len_blocks];
	memset(blocks,0,sizeof(block_t) * len_blocks);

	printf("\n---Incio pruebas de inicializacion de la cache --\n\n");
	test00LaCacheSeInicializaCorrectamente(&cache, blocks);
	
	printf("\n---Incio pruebas de escritura en cache --\n\n");
	test00AlEscribirUnDatoQueNoEstaEnCacheHayMissYSeEscribeElBloqueEnCache(&cache);
	test01AlEscribirUnDatoQueEstaEnCacheHayHitYSeEscribeElBloqueEnCache(&cache);
	test02AlQuitarUnBloqueDeCacheEsteSeEscribeEnMemoria(&cache);

	printf("\n---Incio pruebas de lectura en cache --\n\n");
	//Vuelvo a inicializar la cache para limpiarla
	cache_destroy(&cache);
	cache_init(&cache, blocks, WAYS, CACHE_SIZE, BLOCK_SIZE);
	test00AlLeerUnDatoQueNoEstaEnCacheHayMissYSeLeeElDatoCorrectamente(&cache);
	test01AlLeerUnDatoQueEstaEnCacheHayHitYSeLeeElDatoCorrectamente(&cache);

	printf("\n---Incio pruebas de calculo de miss rate --\n\n");
	test00SeCalculaElMissRateCorrectamente(&cache);

	cache_destroy(&cache);

	return SUCCESS;
}

int main(){
	return tests();
}