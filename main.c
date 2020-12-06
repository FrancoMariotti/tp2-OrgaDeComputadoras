#include "stdio.h"
#include "getopt.h"
#include "stdbool.h"

#define V_OPTION 'V'
#define H_OPTION 'h'
#define O_OPTION 'o'
#define W_OPTION 'w'
#define CACHE_OPTION 'c'
#define BLOCK_OPTION 'b'
#define SIZE_OPTION  's'

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

int main(int argc, char **argv) {
  int c; 
  
  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, V_OPTION},
      {"help", no_argument, 0, H_OPTION},
      {"ways", no_argument, 0, W_OPTION},
      {"cachesize", required_argument, 0, CACHE_OPTION},
      {"blocksize", required_argument, 0, BLOCK_OPTION},
      {"output", required_argument, 0, O_OPTION},
    };

    c = getopt_long(argc, argv, "Vhw:cs:bs:o:", long_options, &option_index);
    if (c == -1)
      break;

    if ( c == V_OPTION ) {

    } else if ( c == H_OPTION ) {
      show_help();
    } else if ( c == W_OPTION ) {

    } else if ( c == CACHE_OPTION ) {

    } else if ( c == BLOCK_OPTION ) {
      
    } else if ( c == O_OPTION ) {
      
    }

  }
  return 0;
}
