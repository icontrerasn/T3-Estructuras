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
  get_indice(filename);
  czFILE* new_file = cz_open('new_file.txt', 'w');
  cz_cp('texto.txt', 'new_file.txt');
  cz_open(filename, 'r');
  cz_mv('text.txt', 'nuevo_texto.txt');
  cz_rm('nuevo_texto.txt');
  cz_ls();
  printf("Existe smile.png: %d", cz_exists('smile.png'));
  printf("Existe smile.jpg: %d", cz_exists('smile.jpg'));
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
