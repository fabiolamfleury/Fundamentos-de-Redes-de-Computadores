#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define IPADRESS "127.0.0.1"
#define PORTNUMBER 8888
#define TRUE 1
#define SIZE 1024

int client (char *message){
  socklen_t addr_size;

  // configuring server information so that server socket is found
  struct sockaddr_in server_socket;
  server_socket.sin_addr.s_addr = inet_addr(IPADRESS);
  server_socket.sin_family = AF_INET;
  server_socket.sin_port = htons(PORTNUMBER);
  memset(server_socket.sin_zero, '\0', sizeof server_socket.sin_zero);

  // creating socket and sending message through it
  int sent_bytes = strlen(message) + 1;
  int client_socket = socket(PF_INET, SOCK_DGRAM, 0);
  addr_size = sizeof server_socket;
  sendto(client_socket,message,SIZE,0,(struct sockaddr *)&server_socket,addr_size);

  // waiting answer, when receiving it, returning 0
  while(TRUE){
    // TODO: Implement time out (udp messages can be lost), return -1
    char *received_message = (char*)malloc(SIZE * sizeof(char));;
    int udp_return = recv(client_socket,received_message, SIZE, 0);
    if (udp_return > 0){
      printf("recebeu");
      char *subbuff = (char*)malloc(SIZE - 1 * sizeof(char));
      memcpy( subbuff, &received_message[0], SIZE -1 );
      subbuff[udp_return] = '\0';
      struct timeval tv;

      printf("Resposta do servidor %d: %s\n",udp_return,subbuff);
      return 0;
    }
  }
}

int main(int argc, char const *argv[]){
  char message[SIZE];

  if (argc > 1){
     if (strcmp(argv[1], "rsv") == 0 && argc == 3){
       // if is rsv, send name that will be resolved
       snprintf(message, SIZE, "rsv: %s", argv[2]);
       client(message);
     }
     else if (strcmp(argv[1], "rtt") == 0){
       // if is rrt, send rtt
       snprintf(message, 4, "%s", argv[1]);
       client(message);
     }
     else{
       printf("Argumentos inválidos.\nA função deve ser chamada da seguinte maneira:\n ./client rsv [nome] OU ./client rtt\n");
     }
  }
    else{
      printf("Argumentos inválidos.\nA função deve ser chamada da seguinte maneira:\n ./client rsv [nome] OU ./client rtt\n");
    }

  return 0;
}
