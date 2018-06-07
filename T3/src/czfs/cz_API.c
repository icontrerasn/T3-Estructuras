#include <string.h>
#include <math.h>
#include <limits.h>
#include <errno.h>
#include "cz_API.h"
extern int errno ;

char* PATHDATA = "/home/iic2333/T3/simdiskfilled.bin";
int SIZEBLOQUE = 1024;
int DATA = 252;
int DIREC_ENTRIES = 64;
int INDIREC_ENTRIES = 256;

int quantity_data(int* array){
  int count = 0;
  for (int i = 0; i < DATA; i++) {
    if (array[i] != 0) {
      count ++;
    }
  }
  return count;
}

int min(int x, int y){
  if (x < y){
    return x;
  }
  else {
    return y;
  }
}

int hexchar_to_dec(unsigned char value[4]){
  int dec;
  dec = value[0]*pow(16,6) + value[1]*pow(16,4) + value[2]*pow(16, 2) + value[3]*pow(16, 0);
  return dec;
}

/* Entrega el índice del archivo filename */
int directorio(char* filename, bool _delete){
  int entradas = 64;
  char bit_valid[1];
  char name[11];
  unsigned char indice[4];
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb+");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < entradas; i++) {
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    fread(indice, 4, 1, filebin);
    if (strncmp(name, filename, 11) == 0){
      break;
    }
  }
  if (_delete) {
    unsigned char borrar[1] = "0";
    fseek(filebin, -16, SEEK_CUR);
    fwrite(borrar, 1, 1, filebin);
  }
  fclose(filebin);
  return hexchar_to_dec(indice);
}

/* Entrega la lista con los bloques que contienen los datos
  y el puntero al bloque de direccionamiento indirecto */
void indice(int indice, int* datos){
  char metadata[12];
  unsigned char bit_datos[DATA+1][4];
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, indice*SIZEBLOQUE, SEEK_SET);
  fread(metadata, 12, 1, filebin);
  fread(bit_datos, 4, DATA, filebin);
  fread(bit_datos[DATA], 4, 1, filebin);
  for (int i = 0; i < DATA+1; i++) {
    datos[i] = hexchar_to_dec(bit_datos[i]);
    //printf("%d\n", datos[i]);
  }
  fclose(filebin);
}

/* Bitmap */
void bitmap(int* bloq_datos, int len_datos, int indice, bool change, unsigned char* option){
  //unsigned char borrar[1] = "0";
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb+");
  fseek(filebin, SIZEBLOQUE, SEEK_SET);
  fseek(filebin, SIZEBLOQUE + indice, SEEK_SET);
  if (change)
    fwrite(option, 1, 1, filebin);
  for (int i = 0; i < len_datos; i++) {
    if (bloq_datos[i] != 0) {
      fseek(filebin, SIZEBLOQUE + bloq_datos[i], SEEK_SET);
      if (change)
        fwrite(option, 1, 1, filebin);
    }
  }
  fclose(filebin);
}

/* Entrega una lista con los bloques que contienen más datos
  desde el bloque de direccionamiento indirecto */
void bindirecto(int bloque, int* bloq_datos){
  unsigned char punteros_datos[INDIREC_ENTRIES][4];
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, bloque*SIZEBLOQUE, SEEK_SET);
  fread(punteros_datos, 4, INDIREC_ENTRIES, filebin);
  for (int i = 0; i < INDIREC_ENTRIES; i++) {
    bloq_datos[i] = hexchar_to_dec(punteros_datos[i]);
    //printf("%d\n", bloq_datos[i]);
  }
  fclose(filebin);
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
      indice = directorio(filename, false);
      file->indice = indice;
      FILE* filebin = fopen(PATHDATA, "rb");
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
  FILE* filebin;
  filebin = fopen(PATHDATA, "rb");
  fseek(filebin, 0, SEEK_CUR);
  for (int i = 0; i < DIREC_ENTRIES; i++) {
    fread(bit_valid, 1, 1, filebin);
    fread(name, 11, 1, filebin);
    if (bit_valid[0] == 1){
      if (strncmp(name, filename, 11) == 0){
        fclose(filebin);
        return 1;
      }
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
  FILE* filebin = fopen(PATHDATA, "rb");
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
  free(file_desc);
  return 0;
}

/* Cambiar nombre a archivo */
int cz_mv(char* orig, char *dest){
  int errnum;
  char bit_valid[1];
  char name[11];
  char indice[11];
  if (cz_exists(orig)){
    if (!(cz_exists(dest))){
      FILE* filebin;
      filebin = fopen(PATHDATA, "rb+");
      fseek(filebin, 0, SEEK_SET);
      for (int i = 0; i < DIREC_ENTRIES; i++) {
        fread(bit_valid, 1, 1, filebin);
        fread(name, 11, 1, filebin);
        if (bit_valid[0] == 1){
          if (strncmp(name, orig, 11) == 0){
            fseek(filebin, -11, SEEK_CUR);
            fwrite(dest, 11, 1, filebin);
            break;
          }
        }
        fread(indice, 4, 1, filebin);
      }
      fclose(filebin);
      return 0;
    }
    else{
      errnum = errno;
      fprintf(stderr, "File name already exists: %s\n", strerror(errnum));
      return 1;
    }
  }
  else{
    errnum = errno;
    fprintf(stderr, "File doesn't exists: %s\n", strerror(errnum));
    return 1;
  }
}

/* Copiar archivo FALTA*/
int cz_cp(char* orig, char* dest){
  int errnum;
  if (cz_exists(orig)){
    if (!(cz_exists(dest))){
      czFILE* orig_file = malloc(sizeof(czFILE));
      orig_file = cz_open(orig, 'r');
      int bloques_datos[DATA];
      for (int i = 0; i < DATA; i++){
        bloques_datos[i] = hexchar_to_dec(orig_file->punteros_bloq_datos[i]);
      }
      int len_file = quantity_data(bloques_datos);

      int indirecto = hexchar_to_dec(orig_file->indirect_pointer);
      int more_bloques_datos[INDIREC_ENTRIES];
      bindirecto(indirecto, more_bloques_datos);
      len_file += quantity_data(more_bloques_datos);
      len_file += 1;

      int bloques_dest[len_file];
      int indice = 0;
      //if (len_file-1 <= DATA) { //No se necesita bloque de direc indirecto
      FILE* filebin;
      filebin = fopen(PATHDATA, "rb+");
      fseek(filebin, SIZEBLOQUE + 8, SEEK_SET);
      int pos = 0;
      for (int i = 0; i < SIZEBLOQUE*DIREC_ENTRIES-8; i++) {
        unsigned char bit[1];
        fread(bit, 1, 1, filebin);
        if (pos == len_file) {
          break;
        } else{
          if (bit[0] == 0){
            if (indice == 0)
              indice = 9+i;
            else
              bloques_dest[pos] = 9+i;
              pos += 1;
          }
        }
      }
      if (pos != len_file) {
        fclose(filebin);
        printf("No hay espacio suficiente para copiar el archivo\n");
        return 1;
      } else{
        // Crear archivo en directorio
        fseek(filebin, 0, SEEK_SET);
        for (int i = 0; i < DIREC_ENTRIES; i++) {
          char bit_valid[1];
          char name[11];
          char ind[11];
          fread(bit_valid, 1, 1, filebin);
          fread(name, 11, 1, filebin);
          if (bit_valid[0] == 0){
            fseek(filebin, -11, SEEK_CUR);
            fwrite(dest, 11, 1, filebin);
            //char indice_char = indice + '0';
            //fwrite(indice_char, 4, 1, filebin);
            fclose(filebin);
            break;
          }
          fread(ind, 4, 1, filebin);
        }
        // Reservar bits en bitmap
        //unsigned char reservar[1] = "1";
        //bitmap(bloques_dest, len_file, indice, true, reservar);

      }

      return 0;
    }
    else{
      errnum = errno;
      fprintf(stderr, "File name already exists: %s\n", strerror(errnum));
      return 1;
    }
  }
  else{
    errnum = errno;
    fprintf(stderr, "File doesn't exists: %s\n", strerror(errnum));
    return 1;
  }
}

/* Elminar archivo */
int cz_rm(char* filename){
  int errnum;
  if (cz_exists(filename)){
    int ind = directorio(filename, true);
    int bloq_datos[DATA+1];
    indice(ind, bloq_datos);
    int bloque_indirecto = bloq_datos[DATA];
    int more_bloq_datos[INDIREC_ENTRIES];
    bindirecto(bloque_indirecto, more_bloq_datos);
    unsigned char borrar[1] = "0";
    bitmap(bloq_datos, DATA, ind, true, borrar);
    bitmap(more_bloq_datos, INDIREC_ENTRIES, bloque_indirecto, true, borrar);

    return 0;
  }
  else{
    errnum = errno;
    fprintf(stderr, "File doesn't exists: %s\n", strerror(errnum));
    return 1;
  }
}

/* Mostrar todos los archivos */
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
