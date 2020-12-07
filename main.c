#include "stdio.h"
#include "getopt.h"
#include "stdbool.h"
#include "stdlib.h"
#include "cache.h"

#define V_OPTION 'V'
#define H_OPTION 'h'
#define O_OPTION 'o'
#define W_OPTION 'w'
#define CACHE_OPTION 'c'
#define BLOCK_OPTION 'b'
#define SIZE_OPTION  's'

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

void start_simulation(int cache_size,int block_size,int ways) {
  cache_t cache;
  int len_blocks = cache_get_blocks(cache_size,block_size);
  block_t blocks[len_blocks];
  cache_init(&cache,blocks,ways,cache_size,block_size);

  cache_read_block(&cache,0);

  cache_destroy(&cache);
}

int main(int argc, char **argv) {
  int c; 
  bool c_flag = false;
  bool b_flag = false;
  //char *cvalue = NULL;

  int cache_size = 0;
  int block_size = 0;
  int ways = 0;

  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, V_OPTION},
      {"help", no_argument, 0, H_OPTION},
      {"ways", required_argument, 0, W_OPTION},
      {"cache", no_argument, 0, CACHE_OPTION},
      {"block", no_argument, 0, BLOCK_OPTION},
      {"size", required_argument, 0, SIZE_OPTION},
      {"output", required_argument, 0, O_OPTION},
    };

    c = getopt_long(argc, argv, "cbVhw:s:o:", long_options, &option_index);
    if (c == -1)
      break;

    if ( c == V_OPTION ) {

    } else if (c == H_OPTION) {
      show_help();
    } else if (c == W_OPTION) {
      ways = atoi(optarg);
    } else if (c == CACHE_OPTION) {
      c_flag = true;
    } else if (c == BLOCK_OPTION) {
      b_flag = true;
    } else if (c == SIZE_OPTION) {
      if (c_flag) {
        cache_size = atoi(optarg);
        c_flag = false;
      } else if (b_flag) {
        block_size = atoi(optarg);
        b_flag = false;
      }
    } else if (c == O_OPTION) {
      
    } else {
      show_invalid();
    }
  }

  if(cache_size && block_size && ways) {
    start_simulation(cache_size,block_size,ways);
  }

  return 0;
}
