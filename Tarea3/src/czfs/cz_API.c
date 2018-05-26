#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <limits.h>
#include "cz_API.h"
char* PATHDATA = "simdiskfilled.bin";
int SIZEBLOQUE = 1024;


void convert_bytes_to_bits(int n, unsigned char* arr, char* bin_arr){
  //char bin_arr[32];
  for (int i = 0; i < n; i++){
    char c = arr[i];
    static char bin[CHAR_BIT + 1] = {0};
    for (int j = CHAR_BIT - 1; j >= 0; j--){
      bin[j] = (c % 2) + '0';
      c /= 2;
      bin_arr[i*CHAR_BIT+j] = bin[j];
    }
    printf("%s\n", bin);
  }
}

int hexchar_to_dec(char value[4]){
  int dec;
  dec = value[0]*pow(16,6) + value[1]*pow(16,4) + value[2]*pow(16, 2) + value[3]*pow(16, 0);
  return dec;
}


/* Entrega el índice del archivo filename*/
int directorio(char* filename){
  int entradas = 64;
  char bit_valid[1];
  char name[11];
  char indice[11];
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < entradas; i++) {
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    fread(indice, 4, 1, filebin);
    if (strncmp(name, filename, 11) == 0){
      break;
    }
  }
  fclose(filebin);
  return hexchar_to_dec(indice);
}

/* Entrega la lista con los bloques que contienen los datos*/
void indice(int indice, int* datos){
  char metadata[12];
  char bit_datos[1008];
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, indice*SIZEBLOQUE, SEEK_SET);
  fread(metadata, 12, 1, filebin);
  fread(bit_datos, 1008, 1, filebin);
  
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
      FILE* filebin = fopen(PATHDATA, "rb");
      fseek(filebin, indice*64, SEEK_CUR);
      fread(file->size, 4, 1, filebin);
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
      file->open_mode = w;
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
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < 64; i++) {
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    if (strncmp(name, filename, 11) == 0){
      //fread(indice, 4, 1, filebin);
      fclose(filebin);
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
  free(file_desc);
  return 0;
}

int cz_mv(char* orig, char *dest){
  char bit_valid[1];
  char name[11];
  char indice[11];
  if (cz_exists(orig)){
    if (!(cz_exists(dest))){
      FILE* filebin;
      filebin = fopen(PATHDATA, "rb+");
      fseek(filebin, 0, SEEK_SET);
      for (int i = 0; i < 64; i++) {
        fread(bit_valid, 1, 1, filebin);
        fread(name, 11, 1, filebin);
        printf("%s\n", name);
        if (strncmp(name, orig, 11) == 0){
          fseek(filebin, -11, SEEK_CUR);
          fwrite(dest, 11, 1, filebin);
          break;
        }
        fread(indice, 4, 1, filebin);
      }
      fclose(filebin);
      return 0;
    }
    else{
      printf("Nombre de archivo ya existe\n");
      return 1;
    }
  }
  else{
    printf("Archivo no existe\n");
    return 1;
  }
}


int cz_cp(char* orig, char* dest){
  return 0;
}

int cz_rm(char* filename){
  char bit_valid[1];
  char name[11];
  char indice[11];
  if (cz_exists(filename)){
      FILE* filebin;
      filebin = fopen(PATHDATA, "rb+");
      fseek(filebin, 0, SEEK_SET);
      for (int i = 0; i < 64; i++) {
        fread(bit_valid, 1, 1, filebin);
        fread(name, 11, 1, filebin);
        printf("%s\n", name);
        if (strncmp(name, filename, 11) == 0){
          fseek(filebin, -12, SEEK_CUR);
          fwrite(0, 1, 1, filebin);
          break;
        }
        fread(indice, 4, 1, filebin);
      }
      fclose(filebin);
      return 0;
  }
  else{
    printf("Archivo no existe\n");
    return 1;
  }
}


void cz_ls(){
  char bit_valid[1];
  char name[11];
  unsigned char indice[4];
  //bool found = false;
  char* PATHDATA = "simdiskfilled.bin";
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, 0, SEEK_SET);
  for (int i = 0; i < 252; i++) {
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
