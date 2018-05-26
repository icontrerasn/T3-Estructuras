#pragma once

#include <stdlib.h>
#include <stdio.h>


struct cz_file{
  char open_mode;
  char filename[11];
  char size[4];
  char time_creat[4];
  char time_mod[4];
  char punteros_bloq_datos[252][4];
  char indirect_pointer[4];
};
typedef struct cz_file czFILE;

int directorio(char* filename);

unsigned found_file(char* filename);

unsigned binary_to_decimal(int n, unsigned char* arr);

int hexchar_to_dec(char value[4]);

int get_indice(char* filename);

czFILE* cz_open(char* filename, char mode);

int cz_exists(char* filename);

int cz_read(czFILE* file_desc, void* buffer, int nbytes);

int cz_write(czFILE* file_desc, void* buffer, int nbytes);

int cz_close(czFILE* file_desc);

int cz_mv(char* orig, char *dest);

int cz_cp(char* orig, char* dest);

int cz_rm(char* filename);

void cz_ls();
