#pragma once

#include <stdlib.h>
#include <stdio.h>


struct cz_file{
  FILE* data;
};
typedef struct cz_file czFILE;



czFILE* cz_open(char* filename, char mode);

int cz_exists(char* filename);

int cz_read(czFILE* file_desc, void* buffer, int nbytes);

int cz_write(czFILE* file_desc, void* buffer, int nbytes);

int cz_close(czFILE* file_desc);

int cz_mv(char* orig, char *dest);

int cz_cp(char* orig, char* dest);

int cz_rm(char* filename);

void cz_ls();
