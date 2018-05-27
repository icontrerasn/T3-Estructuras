#include <stdbool.h>
#include <stdio.h>
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
int min(int x, int y){
  if (x < y){
    return x;
  }
  else {
    return y;
  }
}

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

int hexchar_to_dec(unsigned char value[4]){
  unsigned int dec;
  dec = value[0]*pow(16,6) + value[1]*pow(16,4) + value[2]*pow(16, 2) + value[3]*pow(16, 0);
  return dec;
}

int get_indice(char* filename){
  /* Se asume que se busca un archivo existente*/
  char bit_valid[1];
  char name[11];
  unsigned char indice[4];
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
    file->filename = *filename;
    file->bytes_read = 0;
    if (cz_exists(filename)) {
      indice = get_indice(filename);
      file->indice = indice;
      char* pathdata = "simdiskfilled.bin";
      FILE* filebin = fopen(pathdata, "rb");
      fseek(filebin, 1024*indice, SEEK_SET);
      fread(file->size, 4, 1, filebin);
      fread(file->time_creat, 4, 1, filebin);
      fread(file->time_mod, 4, 1, filebin);
      fread(file->punteros_bloq_datos, 4, 252, filebin);
      fread(file->indirect_pointer, 4, 1, filebin);
      fclose(filebin);
      return file;
    } else {
      return NULL;
    }
  }
  else if (mode == w) {
    if (!(cz_exists(filename))) {
      return file;
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
      return 1;
    }
    fread(indice, 4, 1, filebin);
  }
  fclose(filebin);
  return 0;
}

int cz_read(czFILE* file_desc, void* buffer, int nbytes){
  if (file_desc->open_mode == 'w') {
    return -1;
  }
  char* pathdata = "simdiskfilled.bin";
  FILE* filebin = fopen(pathdata, "rb");
  int unread_bytes = hexchar_to_dec(file_desc->size) - file_desc->bytes_read;
  int starting_block = (file_desc->bytes_read) / 1024;
  int first_block_jump = (file_desc->bytes_read) % 1024;
  int last_block_jump = (unread_bytes - first_block_jump) % 1024;
  int bytes_to_read = min(nbytes, unread_bytes);
  int blocks_to_read = first_block_jump + bytes_to_read / 1024;
  if ((first_block_jump + bytes_to_read) % 1024){
    blocks_to_read ++;
  }
  if (bytes_to_read == 0 || bytes_to_read < 0){
    fclose(filebin);
    return -1;
  }
  int redirect = 0;
  int to_write = 1024;
  for (int i = starting_block; i < starting_block + blocks_to_read; i++){
    if (i == starting_block + blocks_to_read){
      to_write = last_block_jump;
    }
    if (redirect){
      fseek(filebin, 0, SEEK_CUR);
      unsigned char temp_buff[to_write];
      fread(temp_buff, to_write, 1, filebin);
      fwrite(temp_buff, to_write, 1, buffer);
    }
    fseek(filebin, hexchar_to_dec(file_desc->punteros_bloq_datos[i]), SEEK_SET);
    unsigned char temp_buff[to_write];
    fread(temp_buff, to_write, 1, filebin);
    fwrite(temp_buff, to_write, 1, buffer);
    if ((i + 1) % 252 == 0){
      redirect = hexchar_to_dec(file_desc->punteros_bloq_datos[i]);
      fseek(filebin, redirect, SEEK_SET);
    }
  file_desc->bytes_read += bytes_to_read;
  }
  fclose(filebin);
  return bytes_to_read;
}

int cz_write(czFILE* file_desc, void* buffer, int nbytes){
  int blocks;
  blocks = (nbytes - 1 / 1024);
  blocks ++;
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
