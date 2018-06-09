#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/socket.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<netdb.h>
#include "utilities.h"
#include <pthread.h>

#include <server.h>

#include"game.h"

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

#define LEN_MESSAGE 2000
Game* game;

void make_package(char package[LEN_MESSAGE], int id, int p_size, char *payload){
  char* message_id = decimal_to_binary(id);
  char* payload_size = decimal_to_binary(p_size);
  memcpy(package, message_id, 8);
  memcpy(package + 8, payload_size, 8);
  if (p_size != 0) {
    memcpy(package + 16, payload, p_size);
  }
}

void decod_package(char* package, int conexion_cliente){
  char message_id[8];
  memcpy(message_id, package, 8);
  int id = binary_to_decimal(8, message_id);
  printf("Id %d\n", id);

  char payload_size[8];
  memcpy(payload_size, package + 8, 8);
  int size = binary_to_decimal(8, payload_size);
  printf("Tamaño %d\n", size);

  char payload[size+1];
  strncpy(payload, package + 16, size);
  printf("%d\n", size);

  char buf[LEN_MESSAGE];

  switch (id) {
    case START_CONNECTION:
      //printf("recibido\n");
      make_package(buf, CONN_ESTABLISHED, 0, NULL);
      send(conexion_cliente, buf, LEN_MESSAGE, 0);
      bzero(buf, LEN_MESSAGE);

      make_package(buf, ASK_NICKNAME, 0, NULL);
      send(conexion_cliente, buf, LEN_MESSAGE, 0);
      bzero(buf, LEN_MESSAGE);
      break;

    case RET_NICKNAME:
      printf("recibido nick\n");
      printf("%s\n", payload);
      // char nickname[256];
      // scanf("%s\n", nickname);
      // make_package(package, RET_NICKNAME, strlen(nickname), nickname);
      // send(conexion, buffer, LEN_MESSAGE, 0);
      // bzero(buffer, LEN_MESSAGE);

      make_package(buf, OPP_FOUND, 0, NULL);

      char pot[4] = "1000";
      make_package(buf, INITIAL_POT, 4, pot);

      make_package(buf, GAME_START, 0, NULL);

      //make_package(buf, START_ROUND, 0, NULL);
      //make_package(buf, INITIAL_BET, 0, NULL);
      //make_package(buf, FIVE_CARDS, 0, NULL);
      //make_package(buf, FIRST, 1, NULL);
      //make_package(buf, GET_CHANGE_CARDS, 0, NULL);
      break;

    case RET_CHANGE_CARDS:
      char bet[5] = {"1", "2", "3", "4", "5"};
      make_package(buf, GET_BET, 5, bet);
      break;

    case RET_BET:
      //make_package(buf, ERROR_BET, 0, NULL);
      make_package(buf, OK_BET, 0, NULL);
      make_package(buf, END_ROUND, 0, NULL);
      //make_package(buf, SHOW_OPP_CARDS, 10, NULL);
      //make_package(buf, WINNER_LOSER, 1, NULL);
      char monto[7];
      //make_package(buf, UPDATE_POT, 7, monto);
      make_package(buf, GAME_END, 0, NULL);
      break;

    case ERROR_NOT_IM:
      break;

  }
}

int main(int argc, char **argv){
  game = create_game();
  if(argc<5){
    printf("./server -i <ip_address> -p <tcp-port>\n");
    return 1;
  }
  int conexion_servidor, conexion_cliente, puerto, host, *new_sock; //declaramos las variables
  socklen_t longc; //Debemos declarar una variable que contendrá la longitud de la estructura
  struct sockaddr_in servidor, cliente;

  char buffer[LEN_MESSAGE]; //Declaramos una variable que contendrá los mensajes que recibamos
  puerto = atoi(argv[4]);
  host = atoi(argv[2]);

  conexion_servidor = socket(AF_INET, SOCK_STREAM, 0); //creamos el socket
  bzero((char *)&servidor, sizeof(servidor)); //llenamos la estructura de 0's
  servidor.sin_family = AF_INET; //asignamos a la estructura
  servidor.sin_port = htons(puerto);
  servidor.sin_addr.s_addr = htons(host);//INADDR_ANY; //esta macro especifica nuestra dirección
  if(bind(conexion_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
  { //asignamos un puerto al socket
    printf("Error al asociar el puerto a la conexion\n");
    close(conexion_servidor);
    return 1;
  }
  listen(conexion_servidor, 2); //Estamos a la escucha
  printf("A la escucha en el puerto %d, host %d\n", ntohs(servidor.sin_port), ntohs(servidor.sin_addr.s_addr));
  longc = sizeof(cliente); //Asignamos el tamaño de la estructura a esta variable

  while((conexion_cliente = accept(conexion_servidor, (struct sockaddr *)&cliente, &longc))){
    if(conexion_cliente<0)
    {
      printf("Error al aceptar trafico\n");
      close(conexion_servidor);
      return 1;
    }
    pthread_t client_thread;
    new_sock = malloc(1);
    *new_sock = conexion_cliente;

    if (pthread_create(&client_thread, NULL, handler, (void*)new_sock) < 0){
      printf("Error al aceptar tráfico\n");
      close(conexion_servidor);
      return 1;
    }

    printf("Conectado con %s:%d\n", inet_ntoa(cliente.sin_addr), htons(cliente.sin_port));
  }
}

void *handler(void *conexion_servidor){
    int read_size;
    int sock = *(int*)conexion_servidor;
    char server_message[16000];
    char client_message[16000];

    if (!game->socket[0]){
      game->socket[0] = sock;
    } else {
      game->socket[1] = sock;
    }

    while( (read_size = recv(sock, client_message, 16000, 0)) > 0){
      //printf("%s\n", buffer);
      //bzero((char *)&buffer, sizeof(buffer));
      //send(conexion_cliente, "Recibido\n", 13, 0);

    }
    if (read_size == 0){
      printf("El cliente se ha desconectado\n");
    } else if (read_size == -1){
      printf("Error al recibir los datos\n");
    }
    free(conexion_servidor);
    return 0;
}
