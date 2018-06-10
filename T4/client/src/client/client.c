#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<netdb.h>
#include "utilities.h"

#define START_CONNECTION 1
#define CONN_ESTABLISHED 2
#define ASK_NICKNAME 3
#define RET_NICKNAME 4
#define OPP_FOUND 5
#define INITIAL_POT 6
#define GAME_START 7
#define START_ROUND 8
#define INITIAL_BET 9
#define FIVE_CARDS 10
#define FIRST 11
#define GET_CHANGE_CARDS 12
#define RET_CHANGE_CARDS 13
#define GET_BET 14
#define RET_BET 15
#define ERROR_BET 16
#define OK_BET 17
#define END_ROUND 18
#define SHOW_OPP_CARDS 19
#define WINNER_LOSER 20
#define UPDATE_POT 21
#define GAME_END 22
#define IMAGE 23
#define ERROR_NOT_IM 24

#define LEN_MESSAGE 2100

void make_package(char package[LEN_MESSAGE], int id, int p_size, char *payload){
  char* message_id = decimal_to_binary(id);
  char* payload_size = decimal_to_binary(p_size);
  memcpy(package, message_id, 8);
  memcpy(package + 8, payload_size, 8);
  if (p_size != 0) {
    memcpy(package + 16, payload, p_size);
  }
  printf("%s\n", package);
}

void decod_package(char* package, int conexion){
  char message_id[8];
  memcpy(message_id, package, 8);
  int id = binary_to_decimal(8, message_id);

  char payload_size[8];
  memcpy(payload_size, package + 8, 8);
  int size = binary_to_decimal(8, payload_size);

  if (size != 0) {
    char payload[size];
    memcpy(payload, package + 16, size);
  }

  char buffer[LEN_MESSAGE];
  char nickname[256];

  switch (id) {
    case ASK_NICKNAME:
      printf("Ingresa tu nombre: \n");
      scanf("%s", nickname);
      printf("%lu\n", strlen(nickname));
      make_package(buffer, RET_NICKNAME, strlen(nickname), nickname);
      send(conexion, buffer, 16+strlen(nickname), 0);
      bzero(buffer, LEN_MESSAGE);
      break;

      case OPP_FOUND:
        printf("Tu oponente es: \n");
        break;

      case INITIAL_POT:
        printf("Tu monto inicial para apostar es: \n");
        break;

      case GAME_START:
        printf("QUE EMPICE EL JUEGO\n");
        break;

      case START_ROUND:
      printf("Tu dinero: \n");
        break;

      case INITIAL_BET:
        break;

      case FIVE_CARDS:
        break;

      case FIRST:
        break;

      case GET_CHANGE_CARDS:
        break;

      case GET_BET:
        break;

      case ERROR_BET:
        break;

      case OK_BET:
        break;

      case END_ROUND:
        break;

      case SHOW_OPP_CARDS:
        break;

      case WINNER_LOSER:
        break;

      case UPDATE_POT:
        break;

      case GAME_END:
        break;

      case IMAGE:
        break;

      case ERROR_NOT_IM:
        break;
  }
}

int main(int argc, char **argv){
  if(argc<5){
    printf("./client -i <ip_address> -p <tcp-port>\n");
    return 1;
  }
  struct sockaddr_in cliente; //Declaración de la estructura con información para la conexión
  //struct hostent *servidor; //Declaración de la estructura con información del host
  char* servidor;
  servidor = argv[2];
  if(servidor == NULL){
    printf("Host erróneo\n");
    return 1;
  }
  int puerto, conexion;
  char buffer[LEN_MESSAGE];
  puerto = atoi(argv[4]);

  conexion = socket(AF_INET, SOCK_STREAM, 0);

  bzero((char *)&cliente, sizeof(cliente)); //Rellena toda la estructura de 0's
        //La función bzero() es como memset() pero inicializando a 0 todas la variables
  cliente.sin_family = AF_INET;
  cliente.sin_port = htons(puerto);
  cliente.sin_addr.s_addr = inet_addr(servidor);

  //bcopy(); copia los datos del primer elemendo en el segundo con el tamaño máximo
  //del tercer argumento.

  //cliente.sin_addr = *((struct in_addr *)servidor->h_addr); //<--para empezar prefiero que se usen
  //inet_aton(argv[1],&cliente.sin_addr); //<--alguna de estas dos funciones

  if(connect(conexion,(struct sockaddr *)&cliente, sizeof(cliente)) < 0){
    printf("Error conectando con el host\n");
    close(conexion);
    return 1;
  }
  printf("Conectado con %s:%d\n",inet_ntoa(cliente.sin_addr),htons(cliente.sin_port));

  make_package(buffer, START_CONNECTION, 0, NULL);
  send(conexion, buffer, LEN_MESSAGE, 0); //envio
  bzero(buffer, LEN_MESSAGE);
  while (recv(conexion, buffer, LEN_MESSAGE, 0) > 0) {
    decod_package(buffer, conexion);
  }

  return 0;
}
