#include "stdio.h"

#define V_OPTION 'V'
#define H_OPTION 'h'
#define O_OPTION 'o'
#define W_OPTION 'w'
#define CS_OPTION 'cs'
#define BS_OPTION 'bs'

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



int main(int argc, char const *argv[]) {
  int c; 
  
  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, V_OPTION},
      {"help", no_argument, 0, H_OPTION},
      {"ways", no_argument, 0, W_OPTION},
      {"cachesize", required_argument, 0, CS_OPTION},
      {"blocksize", required_argument, 0, BS_OPTION},
      {"output", required_argument, 0, O_OPTION},
    };

    c = getopt_long(argc, argv, "Vhdmo:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case V_OPTION:
        break;
      
      case H_OPTION:
        show_help();
        break;
      
      case W_OPTION:
        break;
      
      case CS_OPTION:
        break;

      case BS_OPTION:
        
        break;

      case O_OPTION:
        
        break;
      
      default:
    }
  }
  return 0;
}
