#include <stdio.h> //entrada e saída
#include <stdlib.h> //funções de comparação (string)
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <time.h>
#include <sys/time.h>

#define IPADRESS "127.0.0.1" //localhost: roda em dois terminais no seu proprio computador
#define PORTNUMBER 8888 //porta escolhido
#define TRUE 1
#define SIZE 1024 //usa em alocaçõe de memória

struct sockaddr_in create_socket_addr(){
  // configuring server information so that server socket is found
  //struct: variável conjunta; info necessarias de conexao
  struct sockaddr_in server_socket;
  server_socket.sin_addr.s_addr = inet_addr(IPADRESS);
  server_socket.sin_family = AF_INET; //Família do endereço (serve pra UDP e TCP)
  server_socket.sin_port = htons(PORTNUMBER);
  memset(server_socket.sin_zero, '\0', sizeof server_socket.sin_zero); //memset: jeito mais seguro de utilizar uma string
  return server_socket;
}

int send_message(char *message, struct sockaddr_in server_socket){
  // creating socket and sending message through it
  int sent_bytes = strlen(message) + 1; //+1 terminador do char
  int client_socket = socket(PF_INET, SOCK_DGRAM, 0); //DEFINIÇÃO DE TRANSPORTE UDP; abertura do SOCKET no int; PF_INET: Limitador do UDP; SOCK_DGRAM: datagrama->UDP
  socklen_t addr_size;
  addr_size = sizeof server_socket;//tamanho do endereço q esta sendo enviado
  sendto(client_socket,message,SIZE,0,(struct sockaddr *)&server_socket,addr_size); //envio da mensagem; pra quem esta sendo enviando (server_socket) e o tamanho da mensagem

  return client_socket;
}

int receive_answer (int client_socket, struct sockaddr_in server_socket){ //recebe uma char
  // waiting answer, when receiving it, returning 0
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    char *received_message = (char*)malloc(SIZE * sizeof(char));; //variavel para guardar o q sera recebido do servidor; malloc: separa o espaço do vetor
    socklen_t addr_size = sizeof server_socket;//tamanho do endereço q esta sendo enviado
    int udp_return = recvfrom(client_socket,received_message, SIZE, 0, (struct sockaddr *)&server_socket, &addr_size); //tamanho da variavel recebido
    if (udp_return > 0){ //condição para sair do while
      //printf("recebeu");
      char *subbuff = (char*)malloc(SIZE - 1 * sizeof(char)); //variavel auxiliar
      memcpy( subbuff, &received_message[0], SIZE -1 ); //copia o q esta na mensagem recebida para essa nova variavel
      subbuff[udp_return] = '\0'; //acrescenta o /0 no final para garantir o printf

   //   printf("Resposta do servidor %d: %s\n",udp_return,subbuff);
      printf("Resposta do servidor: %s\n\n",subbuff);

      return 0;
    }

}

int rsv(char* message){

  struct sockaddr_in server_socket = create_socket_addr();

  int client_socket = send_message(message, server_socket);

  receive_answer(client_socket, server_socket);
}

int rtt(){
  char message[SIZE];
  //do stuff
  snprintf(message, 4, "rtt \n"); //4: Tamanho da string
  struct sockaddr_in server_socket = create_socket_addr();

  int client_socket = send_message(message, server_socket);
  clock_t tInicio = clock();

  int received = receive_answer(client_socket, server_socket);
  if (received == 0){
    clock_t tFim = clock();

    double tDecorrido = ((double)(tFim - tInicio)/(CLOCKS_PER_SEC));
    printf("rtt = %lf segundo\n", tDecorrido);
  }else{
    printf("Time out!");
  }
}


int main(int argc, char const *argv[]){ //argc: quantidade de argumentos argv: conjunto de argumentos

  if (argc > 1){ //a pessoa esta passando so o executavel, nao pode fazer isso
     if (strcmp(argv[1], "rsv") == 0 && argc == 3){ //CASO DO RSV tem que ter mais um argumento do [nome]
       // if is rsv, send name that will be resolved
       char message[SIZE];
       snprintf(message, SIZE, "rsv: %s", argv[2]); //passa pra variavel mensagem para poder manipular
       rsv(message);
     }

     else if (strcmp(argv[1], "rtt") == 0){
       // if is rrt, send rtt
       int counter;
       for (counter=0; counter<=9; counter++){ //manda 10x
         rtt();
         printf("\n");
       }
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
