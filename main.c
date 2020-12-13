#define _GNU_SOURCE
#include "stdio.h"
#include "getopt.h"
#include "stdbool.h"
#include "stdlib.h"
#include "string.h"
#include "cache.h"

#define FILENAME_LENGTH 50
#define COMMAND_LENGTH 5

#define SUCCESS 0
#define ERROR 1

#define V_OPTION 'V'
#define H_OPTION 'h'
#define O_OPTION 'o'
#define W_OPTION 'w'
#define CACHE_OPTION 'c'
#define BLOCK_OPTION 'b'

#define INVALID_MESSAGE "Invalid option , use -h or --help to list valid commands\n"

void show_help(){
  printf("Usage: \n \
    tp2 -h \n \
    tp2 -V \n \
    tp2 options archivo \n \
    Options: \n \
    -V, --version Print version and quit.\n \
    -h, --help Print this information.\n \
    -o, --output Location of the output file.\n \
    -w, --input Location of the input file.\n \
    -cs, --action Program action: encode (default) or decode.\n \
    -bs, --action Program action: encode (default) or decode.\n \
    Examples:\n \
    tp2 -w 4 -cs 8 -bs 16 prueba1.mem\n");
}

void show_invalid(){
  printf(INVALID_MESSAGE);
}

void start_simulation(FILE* stream,int cache_size,int block_size,int ways) {
  cache_t cache;
  int len_blocks = cache_get_blocks(cache_size,block_size);
  block_t blocks[len_blocks];
  cache_init(&cache,blocks,ways,cache_size,block_size);

  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  char command[COMMAND_LENGTH];

  while ((nread = getline(&line, &len, stream)) != -1) {
    if (sscanf( line, "%s", command) == -1) {
      //informar error.
    }
    if (strcmp(command,INIT_COMMAND)) {

    } else if (strcmp(command,READ_COMMAND)) {

    } else if (strcmp(command,WRITE_COMMAND)) {
      
    } else if (strcmp(command,MISSRATE_COMMAND)) {
      
    } else {
      
    }

    printf("El comando ingresado es:%s",command);
  }

  cache_destroy(&cache);
  free(line);
}

int main(int argc, char **argv) {
  int c; 

  FILE* stream;
  int cache_size = 0;
  int block_size = 0;
  int ways = 0;
  char filename[FILENAME_LENGTH];
  memset(filename,0,FILENAME_LENGTH);

  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, V_OPTION},
      {"help", no_argument, 0, H_OPTION},
      {"ways", required_argument, 0, W_OPTION},
      {"cachesize", no_argument, 0, CACHE_OPTION},
      {"blocksize", no_argument, 0, BLOCK_OPTION},
      {"output", required_argument, 0, O_OPTION},
    };

    c = getopt_long(argc, argv, "c:b:Vhw:o:", long_options, &option_index);
    if (c == -1)
      break;

    if (c == V_OPTION) {

    } else if (c == H_OPTION) {
      show_help();
    } else if (c == W_OPTION) {
      ways = atoi(optarg);
    } else if (c == CACHE_OPTION) {
      cache_size = atoi(optarg);
    } else if (c == BLOCK_OPTION) {
      block_size = atoi(optarg);
    } else if (c == O_OPTION) {
      memcpy(filename, optarg, FILENAME_LENGTH);
    } else {
      show_invalid();
    }
  }

  //abrimos archivo de instrucciones en modo lectura.
  stream = fopen(filename,"r");

  if (!stream) {
    //informar error
    return ERROR;
  }

  if (cache_size && block_size && ways) {
    start_simulation(stream,cache_size,block_size,ways);
  }

  return SUCCESS;
}
