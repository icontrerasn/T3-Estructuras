## Integrantes:
* Fernanda García Faura - 13200070
* Italo Contreras Narbona - 13639218

## Funciones no implementadas

* cz_write
* cz_close
* cz_cp
* No logramos leer el path desde consola. Actualmente lo tenemos seteado en cz_API.c
## Funcionamiento estructura

 La información se guarda directamente en la estructura como se lee desde el archivo .bin, guardados directamente en formato char[], unsigned char[] dependiendo de lo requerido. La transformación de los datos guardados en la estructura a información relevante se hace a medida que se van requeriendo los distintos valores.


```sh
struct cz_file{
  char open_mode;
  char filename;
  int indice;
  unsigned char size[4];
  int bytes_read;
  char time_creat[4];
  char time_mod[4];
  unsigned char punteros_bloq_datos[252][4];
  char indirect_pointer[4];
};
typedef struct cz_file czFILE;
```
