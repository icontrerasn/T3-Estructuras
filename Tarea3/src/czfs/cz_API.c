#include "cz_API.h"

czFILE* cz_open(char* filename, char mode){
  czFILE* file = malloc(sizeof(czFILE));
  file->data = fopen(filename, "rb");
  if (mode == "r"){
    if (file->data){
      //QUE HACER
    } else {
      return NULL;
    }
  } else if (mode == "w") {
    if (file->data){
      return NULL;
    } else {
      //QUE HACER
    }
  } else {
    printf("Modo no válido");
  }


int cz_exists(char* filename){
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

}
