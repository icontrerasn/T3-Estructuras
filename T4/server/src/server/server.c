#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>

#include <server.h>
#include <game.h>

Game* game;

int main(int argc, char **argv){
  game = create_game();
  if(argc<5){
    printf("./server -i <ip_address> -p <tcp-port>\n");
    return 1;
  }
  int conexion_servidor, conexion_cliente, puerto, host, *new_sock; //declaramos las variables
  socklen_t longc; //Debemos declarar una variable que contendrá la longitud de la estructura

  struct sockaddr_in servidor, cliente;

  char buffer[100]; //Declaramos una variable que contendrá los mensajes que recibamos
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
  listen(conexion_servidor, 3); //Estamos a la escucha
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
