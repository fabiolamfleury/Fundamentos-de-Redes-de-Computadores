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
    printf("Hostname: %s\n",hostname);
    struct hostent *hostent = gethostbyname(hostname); // STRUCT HOSTENT: //funciona DNS da biblioteca 
    if ( hostent == NULL) //se IP=0.0.0.0 NAO FOI ENCOTRADO
    {
        herror("gethostbyname");  //MUDAR DE ACORDO COM REQUISIÇÃO DO TRABALHO
        return 1;
    }

    struct in_addr **addr_list;
    addr_list = (struct in_addr **) hostent->h_addr_list;

    int i;
    for(i = 0; addr_list[i] != NULL; i++) //copia para o IP 
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

const char* rsv(char *arguments, char *ip){
  int length = strlen(arguments) - 5;
  char hostname[length + 1]; //+ é do /0
  memcpy( hostname, &arguments[5], length); //copiado o nome que se quer resolver
  hostname[length] = '\0'; //coloca o /0
  hostname_to_ip(hostname , ip); //envia para a outra função

  return ip;
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
    
    char msg[5]="Rrtt";

    struct tm time_now= {0};
    time_t time_clock;

    time(&time_clock);
    time_now = *localtime(&time_clock);

    printf("%s\n", buffer);
     if (strcmp(buffer, "rtt") == 0){ //Calcular o RTT
      // snprintf(answer, SIZE, "%s Horário da máquina: %d:%d:%d\n", buffer, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
 	snprintf(answer, SIZE, "%s! Horário da máquina do servidor: %d:%d:%d\n", msg, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
        printf("%d %s\n",udp_return,answer);

     }
     else {
       char ip[100]; //guarda o Ip que sera descoberto
       rsv(buffer, ip);

       snprintf(answer, SIZE, "Rrsv: %s", ip);
       printf("%d %s\n", udp_return,answer);

     }
     size_t length = strlen(answer);
    sendto(server_socket, answer, udp_return, 0, (struct sockaddr *)&server_storage, socket_size);
  }
  return 0;
}

int main(){
  server();

  return 0;
}
