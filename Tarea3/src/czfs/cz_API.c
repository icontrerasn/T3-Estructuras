#include <stdbool.h>
#include <string.h>
#include <math.h>
#include "cz_API.h"


// void read_bin(char* data, int bloque, int bytes, int pass){
//   int size_bloque = 1024;
//   char* pathdata = "simdiskfilled.bin";
//   FILE* filebin;
//   filebin = fopen(pathdata, "rb");
//   fseek(filebin, bloque*size_bloque+pass, SEEK_SET);
//   fread(data, bytes, 1, filebin);
//   fclose(filebin);
// }
unsigned binary_to_decimal(int n, unsigned char* arr){
  unsigned decimalNumber = 0;
  for (int i = 0; i < n; i++){
    int a = arr[i] - '0';
    int m = n-1-i;
    int b = pow(2, m);
    decimalNumber += a*b;
  }
  return decimalNumber*(-1);
}

int hexchar_to_dec(char value[4]){
  int dec;
  dec = value[0]*pow(16,6) + value[1]*pow(16,4) + value[2]*pow(16, 2) + value[3]*pow(16, 0);
  return dec;
}

int get_indice(char* filename){
  /* Se asume que se busca un archivo existente*/
  char bit_valid[1];
  char name[11];
  char indice[4];
  //bool found = false;
  char* pathdata = "simdiskfilled.bin";
  FILE* filebin;
  filebin = fopen(pathdata, "rb");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < 64; i++) {
    fread(&bit_valid, 1, 1, filebin);
    fread(&name, 11, 1, filebin);
    fread(&indice, 4, 1, filebin);
    if (bit_valid[0] == 1) {
      if (strncmp(name, filename, 11) == 0){
        //found = true;
        break;
      }
    }
  }
  fclose(filebin);
  //unsigned indice_int = binary_to_decimal(4, indice);
  return hexchar_to_dec(indice);
}

czFILE* cz_open(char* filename, char mode){
  czFILE* file = malloc(sizeof(czFILE));
  unsigned indice;
  char r = 'r';
  char w = 'w';
  if (mode == r){
    file->open_mode = r;
    if (cz_exists(filename)) {
      indice = get_indice(filename);
      char* pathdata = "simdiskfilled.bin";
      FILE* filebin = fopen(pathdata, "rb");
      printf("%d\n", indice);
      fseek(filebin, 1024*indice, SEEK_SET);

      fread(file->size, 4, 1, filebin);
      printf("Tamaño: %u\n", hexchar_to_dec(file->size));
      fread(file->time_creat, 4, 1, filebin);
      fread(file->time_mod, 4, 1, filebin);
      fread(file->punteros_bloq_datos, 1008, 1, filebin);
      fread(file->indirect_pointer, 4, 1, filebin);
      fclose(filebin);
      return file;
    } else {
      return NULL;
    }
  }
  else if (mode == w) {
    if (!(cz_exists(filename))) {
      //go_on
    }
    else{
      return NULL;
    }
  }
  else {
    printf("Modo no válido");
    return NULL;
  }
  return NULL;
}

int cz_exists(char* filename){
  char bit_valid[1];
  char name[11];
  char indice[11];
  //unsigned char indice[4];
  char* pathdata = "simdiskfilled.bin";
  FILE* filebin;
  filebin = fopen(pathdata, "rb");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < 64; i++) {
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    if (strncmp(name, filename, 11) == 0){
      //fread(indice, 4, 1, filebin);
      return 1;
    }
    fread(indice, 4, 1, filebin);
  }
  fclose(filebin);
  return 0;
}

int cz_read(czFILE* file_desc, void* buffer, int nbytes){

  return 0;
}

int cz_write(czFILE* file_desc, void* buffer, int nbytes){
  return 0;
}

int cz_close(czFILE* file_desc){
  return 0;
}

int cz_mv(char* orig, char *dest){
  return 0;
}

int cz_cp(char* orig, char* dest){
  return 0;
}

int cz_rm(char* filename){
  return 0;
}

void cz_ls(){
  char bit_valid[1];
  char name[11];
  unsigned char indice[4];
  //bool found = false;
  char* pathdata = "simdiskfilled.bin";
  FILE* filebin;
  filebin = fopen(pathdata, "rb");
  fseek(filebin, 0, SEEK_SET);
  for (int i = 0; i < 64; i++) {
    fseek(filebin, 0, SEEK_CUR);
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    if (bit_valid[0] == 1) {
      printf("%s\n", name);
    }
    fread(indice, 4, 1, filebin);
  }
  fclose(filebin);
}
