#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <netdb.h> //prove o serviço do DNS
#include <time.h>

#define IPADRESS "127.0.0.1"
#define PORTNUMBER 8888
#define SIZE 1024
#define TRUE 1

int hostname_to_ip(char * hostname , char* ip)
{
    struct hostent *hostent = gethostbyname(hostname);
    if (hostent == NULL)
    {
      strcpy(ip, "Nao encontrado");
      return 1;
    } else{
      struct in_addr **addr_list;
      addr_list = (struct in_addr **) hostent->h_addr_list;
      strcpy(ip , inet_ntoa(*addr_list[0]) );
      return 0;
    }
}

char* rsv(char *arguments, char *ip){
  int length = strlen(arguments) - 5;
  char hostname[length + 1];
  memcpy( hostname, &arguments[5], length);
  hostname[length] = '\0';

  int found = hostname_to_ip(hostname , ip); //envia para a outra função

  char *message = (char*)malloc(SIZE * sizeof(char));
  snprintf(message, SIZE, "Rrsv: %d %s\n", found, ip);
  return message;
}

int server (){

  // configuring server address information so that server socket is created properly
  //Analogo ao cliente
  struct sockaddr_in server_socket_addr;
  server_socket_addr.sin_family = AF_INET;
  server_socket_addr.sin_port = htons(PORTNUMBER);
  server_socket_addr.sin_addr.s_addr = inet_addr(IPADRESS);
  memset(server_socket_addr.sin_zero, '\0', sizeof server_socket_addr.sin_zero);

  int server_socket = socket(PF_INET, SOCK_DGRAM, 0); //abre o socket UDP

  // binding server socket address with socket created
  bind(server_socket, (struct sockaddr *) &server_socket_addr, sizeof(server_socket_addr)); //liga o endereço do socket com o socket criado

  /*Initialize size variable to be used later on*/
  struct sockaddr_storage server_storage; //Guarda as info do cliente para poder enviar as respostas
  socklen_t socket_size, client_socket_size;
  socket_size = sizeof server_storage;

  while(TRUE){ //sempre funciona
    char *buffer = (char*)malloc(SIZE * sizeof(char)); //aqui fica a mensagem do cliente
    // will receive any UDP package, storing client information in server_storage so that the properly answer can be send
    int udp_return = recvfrom(server_socket,buffer,SIZE,0,(struct sockaddr *)&server_storage, &socket_size); //receive from

    char *answer = (char*)malloc(SIZE * sizeof(char)); //variavel que pega a requisição do buffer e gera uma resposta



     if (strcmp(buffer, "rtt") == 0){ //Calcular o RTT
       struct tm time_now= {0};
       time_t time_clock;

       time(&time_clock);
       time_now = *localtime(&time_clock);
 	     snprintf(answer, SIZE, "Rrtt: %d:%d:%d", time_now.tm_hour, time_now.tm_min, time_now.tm_sec);

     }
     else {
       char ip[100]; //guarda o Ip que sera

       strcpy(answer , rsv(buffer, ip));
     }
    sendto(server_socket, answer, udp_return, 0, (struct sockaddr *)&server_storage, socket_size);
  }
  return 0;
}

int main(){
  server();

  return 0;
}
