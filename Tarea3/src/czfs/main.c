#include "cz_API.h"


void read_bin(char* data, int bloque, int bytes){
  int size_bloque = 1024;
  char* pathdata = "simdiskfilled.bin";
  FILE* filebin;
  filebin = fopen(pathdata, "rb");
  fseek(filebin, bloque*size_bloque, SEEK_SET);
  fread(data, bytes, 1, filebin);
  fclose(filebin);
}

int main ()
{
  char filename[11] = "texto.txt";
  //get_indice(filename);
  //unsigned indice = found_file(filename);
  //printf("%u\n", indice);
  //char mode = 'r';
  //czFILE* file = cz_open(filename, 'r');
  //printf("%u\n", hexchar_to_dec(file->size));
  //cz_ls();


  return 0;
}

// int main ()
// {
//   char data[1];
//   read_bin(data, 0, 1);
//
//   printf("%d\n", *data);
//   // unsigned number = get_decimal(2, data);
//   // printf("%d\n", number);
//
//   return 0;
// }
