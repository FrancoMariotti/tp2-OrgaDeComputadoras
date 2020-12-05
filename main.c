#include "stdio.h"

#define V_OPTION
#define H_OPTION
#define W_OPTION
#define O_OPTION

int main(int argc, char const *argv[]) {
  int c; 
  
  FILE* outfd = stdout;

  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, 'V'},
      {"help", no_argument, 0, 'h'},
      {"ways", no_argument, 0, 'w'},
      {"cachesize", required_argument, 0, 'cs'},
      {"blocksize", required_argument, 0, 'bs'},
      {"output", required_argument, 0, 'o'},
    };

    c = getopt_long(argc, argv, "Vhdmo:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case V_OPTION:
        break;
      
      case H_OPTION:
        break;
      
      case :
        break;
      
      case :
        break;

      case O_OPTION:
        
        break;
      
      default:
    }
  }
    return 0;
}
