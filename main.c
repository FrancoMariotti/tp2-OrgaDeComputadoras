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

#define VERSION_OPTION 'V'
#define HELP_OPTION 'h'
#define OUTPUT_OPTION 'o'
#define WAYS_OPTION 'w'
#define CACHE_OPTION 'c'
#define BLOCK_OPTION 'b'

#define READ_COMMAND "R"
#define WRITE_COMMAND "W"
#define MISSRATE_COMMAND "MR"

#define INVALID_MESSAGE "Invalid option , use -h or --help to list valid commands\n"

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void show_error(char* error) {
  fprintf(stderr,"%sERROR: %s%s\n" ,ANSI_COLOR_RED,error,ANSI_COLOR_RESET);
}

void show_help(){
  printf("Usage: \n \
    tp2 -h \n \
    tp2 -V \n \
    tp2 options archivo \n \
    Options: \n \
    -V, --version Version del programa.\n \
    -h, --help Imprime Ayuda.\n \
    -o, --output Archivo de salida.\n \
    -w, --ways Cantidad de vı́as.\n \
    -c, --cachesize Tamaño del caché en kilobytes.\n \
    -b, --blocksize Tamaño de bloque en bytes.\n \
    Examples:\n \
    tp2 -w 4 -c 8 -b 16 prueba1.mem\n");
}

void show_invalid(){
  printf(INVALID_MESSAGE);
}

void start_simulation(FILE* input,FILE* output,int cache_size,int block_size,int ways) {
  cache_t cache;
  int len_blocks = cache_get_blocks(cache_size,block_size);
  block_t blocks[len_blocks];
  cache_init(&cache,blocks,ways,cache_size,block_size);
  char *line = NULL;
  size_t len = 0;
  ssize_t nread;
  char command[COMMAND_LENGTH];
  float current_missrate = 0;

  while ((nread = getline(&line, &len, input)) != -1) {
    memset(command,0,COMMAND_LENGTH);
    if (sscanf(line, "%s", command) == -1) {
      show_error("en lectura de comando");
    }
    current_missrate = cache_get_miss_rate(&cache);
    
    if (!strcmp(command,READ_COMMAND)) {
      uint16_t address;
      if (sscanf(line, "%s %hd", command,&address) == -1) {
        show_error("en lectura de comando");
      }
      unsigned char value = cache_read_byte(&cache,address);
      printf("Value:%d\n", value);

      if(cache_get_miss_rate(&cache) >= current_missrate) {
        printf("MISS DE LECTURA\n");
      } else {
        printf("HIT DE LECTURA\n");
      }
    } else if (!strcmp(command,WRITE_COMMAND)) {
      uint16_t address;
      int value;
      if (sscanf(line, "%s %hd, %d", command,&address,&value) == -1) {
        show_error("en lectura de comando");
      }
      cache_write_byte(&cache,address,(char)value);

      if(cache_get_miss_rate(&cache) >= current_missrate) {
        printf("MISS DE ESCRITURA\n");
      } else {
        printf("HIT DE ESCRITURA\n");
      }
    } else if (!strcmp(command,MISSRATE_COMMAND)) {
      printf("Miss rate:%f\n",cache_get_miss_rate(&cache));
    } else {
      show_error("en lectura de comando");
    }
  }

  free(line);
  cache_destroy(&cache);
}

int main(int argc, char **argv) {
  int c; 

  FILE* input = NULL;
  FILE* output = stdout;
  int cache_size = 0;
  int block_size = 0;
  int ways = 0;

  char input_filename[FILENAME_LENGTH];
  memset(input_filename,0,FILENAME_LENGTH);
  char output_filename[FILENAME_LENGTH];
  memset(output_filename,0,FILENAME_LENGTH);

  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, VERSION_OPTION},
      {"help", no_argument, 0, HELP_OPTION},
      {"ways", required_argument, 0, WAYS_OPTION},
      {"cachesize", no_argument, 0, CACHE_OPTION},
      {"blocksize", no_argument, 0, BLOCK_OPTION},
      {"output", required_argument, 0, OUTPUT_OPTION}
    };

    c = getopt_long(argc, argv, "c:b:Vhw:o:", long_options, &option_index);
    if (c == -1)
      break;

    if (c == VERSION_OPTION) {

    } else if (c == HELP_OPTION) {
      show_help();
    } else if (c == WAYS_OPTION) {
      ways = atoi(optarg);
    } else if (c == CACHE_OPTION) {
      cache_size = atoi(optarg);
    } else if (c == BLOCK_OPTION) {
      block_size = atoi(optarg);
    } else if (c == OUTPUT_OPTION) {
      memcpy(output_filename, optarg, FILENAME_LENGTH);
    } else {
      show_invalid();
    }
  }

  if(argc < 8) {
    show_error("cantidad de parametros incorrecta");
    return ERROR;
  }

  memcpy(input_filename, argv[argc-1], FILENAME_LENGTH);

  //abrimos archivo de instrucciones en modo lectura.
  input = fopen(input_filename,"r");
  
  //output = fopen(output_filename,"r");

  if (cache_size && block_size && ways) {
    start_simulation(input,output,cache_size,block_size,ways);
  }

  fclose(input);

  return SUCCESS;
}
