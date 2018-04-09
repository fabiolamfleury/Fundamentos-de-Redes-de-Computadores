#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <netdb.h>

#define IPADRESS "127.0.0.1"
#define PORTNUMBER 8888
#define SIZE 1024
#define TRUE 1

int hostname_to_ip(char * hostname , char* ip)
{

    struct in_addr **addr_list;
    int i;
    printf("Hostname: %s\n",hostname);
    struct hostent *hostent = gethostbyname(hostname);
    if ( hostent == NULL)
    {
        herror("gethostbyname");
        return 1;
    }

    addr_list = (struct in_addr **) hostent->h_addr_list;

    for(i = 0; addr_list[i] != NULL; i++)
    {
        //Return the first one;
        strcpy(ip , inet_ntoa(*addr_list[i]) );
        return 0;
    }

    return 1;
}

const char* rsv(char *arguments, char *ip){
  int length = strlen(arguments) - 5;
  char hostname[length + 1];
  memcpy( hostname, &arguments[5], length);
  hostname[length] = '\0';
  hostname_to_ip(hostname , ip);

  return ip;
}

int server (){
  struct sockaddr_storage server_storage;
  socklen_t socket_size, client_socket_size;


  // configuring server address information so that server socket is created properly
  struct sockaddr_in server_socket_addr;
  server_socket_addr.sin_family = AF_INET;
  server_socket_addr.sin_port = htons(PORTNUMBER);
  server_socket_addr.sin_addr.s_addr = inet_addr(IPADRESS);
  memset(server_socket_addr.sin_zero, '\0', sizeof server_socket_addr.sin_zero);

  int server_socket = socket(PF_INET, SOCK_DGRAM, 0);

  // binding server socket address with socket created
  bind(server_socket, (struct sockaddr *) &server_socket_addr, sizeof(server_socket_addr));

  /*Initialize size variable to be used later on*/
  socket_size = sizeof server_storage;

  while(TRUE){
    char *buffer = (char*)malloc(SIZE * sizeof(char));
    // will receive any UDP package, storing client information in server_storage so that the properly answer can be send
    int udp_return = recvfrom(server_socket,buffer,SIZE,0,(struct sockaddr *)&server_storage, &socket_size);

    char *answer = (char*)malloc(SIZE * sizeof(char));
    printf("%s\n", buffer);
     if (strcmp(buffer, "rtt") == 0){
        snprintf(answer, SIZE, "Calcular o : %s", buffer);
        printf("%d %s\n",udp_return,answer);

     }
     else {
       char ip[100];
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
