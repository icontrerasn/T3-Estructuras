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

#include "server.h"

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
char player_1_name[200];
char player_2_name[200];
int socket_1;
int socket_2;

void make_package(char package[LEN_MESSAGE], int id, int p_size, char *payload){
  char* message_id = decimal_to_binary(id);
  char* payload_size = decimal_to_binary(p_size);
  memcpy(package, message_id, 8);
  memcpy(package + 8, payload_size, 8);
  if (payload != NULL) {
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
      //char bet[5] = {"1", "2", "3", "4", "5"};
      //make_package(buf, GET_BET, 5, bet);
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

  if ((argc == 5) && (((strcmp("-i", argv[1]) == 0) && strcmp("-p", argv[3]) == 0) || ((strcmp("-p", argv[1]) == 0) && strcmp("-i", argv[3]) == 0))){

  } else{
    printf("./server -i <ip_address> -p <tcp-port>\n");
    return 1;
  }

  int conexion_servidor, conexion_cliente, puerto, *new_sock; //declaramos las variables
  char* host;
  socklen_t longc; //Debemos declarar una variable que contendrá la longitud de la estructura
  struct sockaddr_in servidor, cliente;

  char buffer[LEN_MESSAGE]; //Declaramos una variable que contendrá los mensajes que recibamos
  puerto = atoi(argv[4]);
  host = argv[2];
  conexion_servidor = socket(AF_INET, SOCK_STREAM, 0); //creamos el socket
  bzero((char *)&servidor, sizeof(servidor)); //llenamos la estructura de 0's
  servidor.sin_family = AF_INET; //asignamos a la estructura
  servidor.sin_port = htons(puerto);
  servidor.sin_addr.s_addr = inet_addr(host);//INADDR_ANY; //esta macro especifica nuestra dirección
  if(bind(conexion_servidor, (struct sockaddr *)&servidor, sizeof(servidor)) < 0)
  { //asignamos un puerto al socket
    printf("Error al asociar el puerto a la conexion\n");
    close(conexion_servidor);
    return 1;
  }
  listen(conexion_servidor, 2); //Estamos a la escucha

  printf("A la escucha en el puerto %d, host %s\n", ntohs(servidor.sin_port), host);

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
    int conexion_cliente = *(int*)conexion_servidor;
    char client_message[LEN_MESSAGE];

    if (!socket_1){
      socket_1 = conexion_cliente;
    } else {
      socket_2 = conexion_cliente;
    }

    while( (read_size = recv(conexion_cliente, client_message, 16000, 0)) > 0){
      char message_id[8];
      memcpy(message_id, client_message, 8);
      int id = binary_to_decimal(8, message_id);
      printf("Id %d\n", id);

      char payload_size[8];
      memcpy(payload_size, client_message + 8, 8);
      int size = binary_to_decimal(8, payload_size);
      printf("Tamaño %d\n", size);

      char payload[size];

      char buffer[LEN_MESSAGE];

      switch (id) {
        case START_CONNECTION:
          //printf("recibido\n");
          make_package(buffer, CONN_ESTABLISHED, 0, NULL);
          send(conexion_cliente, buffer, LEN_MESSAGE, 0);
          bzero(buffer, LEN_MESSAGE);

          make_package(buffer, ASK_NICKNAME, 0, NULL);
          send(conexion_cliente, buffer, LEN_MESSAGE, 0);
          bzero(buffer, LEN_MESSAGE);
          break;

        case RET_NICKNAME:
          memcpy(payload, client_message + 16, size);
          printf("%s\n", payload);

          if (!*player_1_name){
            memcpy(player_1_name, payload, size);
          } else {
            memcpy(player_2_name, payload, size);
            make_package(buffer, OPP_FOUND, strlen(player_1_name), player_1_name);
            sleep(1);
            send(socket_2, buffer, LEN_MESSAGE, 0);
            char buffer[LEN_MESSAGE];
            make_package(buffer, OPP_FOUND, strlen(player_2_name), player_2_name);
            sleep(1);
            send(socket_1, buffer, LEN_MESSAGE, 0);
          }
          if (socket_1 && socket_2) {
            int sockets[2] = {socket_1, socket_2};
            for (int i = 0; i < 2; i++) {
              int sock = sockets[i];
              char* pot;
              pot = decimal_to_binary(1000);
              make_package(buffer, INITIAL_POT, 2, NULL);
              memcpy(buffer + 16, "00000011", 8);
              memcpy(buffer + 24, "11101000", 8);
              send(sock, buffer, LEN_MESSAGE, 0);

              make_package(buffer, GAME_START, 0, NULL);
              send(sock, buffer, LEN_MESSAGE, 0);

              make_package(buffer, START_ROUND, 0, NULL);
              send(sock, buffer, LEN_MESSAGE, 0);

              make_package(buffer, INITIAL_BET, 1, NULL);
              memcpy(buffer + 16, "00001010", 8);
              send(sock, buffer, LEN_MESSAGE, 0);

              // Hand* mano = malloc(sizeof(Hand));
              // mano = generate_hand();
              // make_package(buffer, FIVE_CARDS, 10, NULL);
              // for (int c = 0; c < 5; c++) {
              //   memcpy(buffer + 16*(c+1), decimal_to_binary(mano->cards[c]->number),8);
              //   memcpy(buffer + 24 + 16*c, decimal_to_binary(mano->cards[c]->color),8);
              // }
              // send(conexion_cliente, buffer, LEN_MESSAGE, 0);
              //make_package(buffer, FIRST, 1, NULL);
              //send(conexion_cliente, buffer, LEN_MESSAGE, 0);
              //make_package(buffer, GET_CHANGE_CARDS, 0, NULL);
              //send(conexion_cliente, buffer, LEN_MESSAGE, 0);
              make_package(buffer, GAME_END, 0, NULL);
              send(sock, buffer, LEN_MESSAGE, 0);
            }
          }
          break;

        case RET_CHANGE_CARDS:
          // char bet[5] = {"1", "2", "3", "4", "5"};
          // make_package(buffer, GET_BET, 5, bet);
          break;

        case RET_BET:
          //make_package(buffer, ERROR_BET, 0, NULL);
          make_package(buffer, OK_BET, 0, NULL);
          make_package(buffer, END_ROUND, 0, NULL);
          //make_package(buffer, SHOW_OPP_CARDS, 10, NULL);
          //make_package(buffer, WINNER_LOSER, 1, NULL);
          //char monto[7];
          //make_package(buffer, UPDATE_POT, 7, monto);
          make_package(buffer, GAME_END, 0, NULL);
          break;

        case ERROR_NOT_IM:
          printf("Se envió mensaje no identificado\n");
          break;


      }

    }
    int count = 0;
    if (read_size == 0){
      count += 1;
      printf("El cliente se ha desconectado\n");
    } else if (read_size == -1){
      printf("Error al recibir los datos\n");
    }
    if (count == 2) {
      close(conexion_cliente);
      free(conexion_servidor);
    }
    free(conexion_servidor);
    return 0;
}
