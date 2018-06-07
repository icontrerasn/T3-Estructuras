#include "cz_API.h"

int main(int argc, char *argv[]) {
  if (argc != 2){
    printf("Deben ser 2 parámetros\n");
  }
  char* path = argv[1];
  char filename[11] = "texto.txt";

  printf("Mostrando todos los archivos\n");
  cz_ls();
  printf("Veamos si existe uno\n");
  int existe = cz_exists(filename);
  if (existe) {
    printf("Existe el archivo %s\n", filename);
  } else{
    printf("El archivo %s no existe\n", filename);
  }
  printf("Cambiar de nombre\n");
  cz_mv(filename, "change.txt");
  printf("Ahora se ve el cambio\n");
  cz_ls();
  printf("Borrar archivo\n");
  cz_rm("change.txt");
  printf("Ahora se ve el cambio\n");
  cz_ls();
  int existe2 = cz_exists(filename);
  if (existe2) {
    printf("Existe el archivo %s\n", filename);
    czFILE* file = cz_open(filename, 'r');
    char informacion[100];
    cz_read(file, &informacion, 100);
  } else{
    printf("El archivo %s no existe\n", filename);
  }


  return 0;
}
