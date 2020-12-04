#include "stdio.h"

int main(int argc, char const *argv[]) {
  int c; 
  
  FILE* outfd = stdout;

  while (true) {
    int option_index = 0;

    static struct option long_options[] = {
      {"version", no_argument, 0, 'V'},
      {"help", no_argument, 0, 'h'},
      {"divisor", no_argument, 0, 'd'},
      {"multiple", no_argument, 0, 'm'},
      {"output", required_argument, 0, 'o'},
    };

    c = getopt_long(argc, argv, "Vhdmo:", long_options, &option_index);
    if (c == -1)
      break;

    switch (c) {
      case V_OPTION:
        operacion = VERSION;
        imprimir_salida(VERSION_MESSAGE);
        break;
      
      case H_OPTION:
        operacion = HELP;
        imprimir_salida(HELP_MESSAGE);        
        break;
      
      case D_OPTION:
        operacion = DIVISOR;
        break;
      
      case M_OPTION:
        operacion = MULTIPLE;
        break;

      case O_OPTION:
        memcpy(outputFileName,optarg,strlen(optarg));
        outputFileptr = outputFileName;
              
        if (outputFileptr && (strcmp("-", outputFileName) != 0)) {
          outfd = fopen(outputFileName,"w");
        }
        
        break;
      
      default:
        operacion = INVALID;
        imprimir_salida(INVALID_MESSAGE);
    }
  }
    return 0;
}
