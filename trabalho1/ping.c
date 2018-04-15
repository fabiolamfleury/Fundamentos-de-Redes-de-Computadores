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
  sendto(client_socket, message, SIZE, 0, (struct sockaddr *)&server_socket, addr_size); //envio da mensagem; pra quem esta sendo enviando (server_socket) e o tamanho da mensagem

  return client_socket;
}

char* receive_answer (int client_socket, struct sockaddr_in server_socket){ //recebe uma char
  // waiting answer, when receiving it, returning 0
    char *received_answer = NULL;
    struct timeval tv;
    tv.tv_sec = 1;
    tv.tv_usec = 0;
    setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

    char *received_message = (char*)malloc(SIZE * sizeof(char));; //variavel para guardar o q sera recebido do servidor; malloc: separa o espaço do vetor
    socklen_t addr_size = sizeof server_socket;//tamanho do endereço q esta sendo enviado
    int udp_return = recv(client_socket,received_message, SIZE, 0); //tamanho da variavel recebido
    if (udp_return > 0){ //condição para sair do while
      //printf("recebeu");
      char *buffer= (char*)malloc(SIZE - 1 * sizeof(char)); //variavel auxiliar
      memcpy( buffer, &received_message[0], SIZE -1 ); //copia o q esta na mensagem recebida para essa nova variavel
      buffer[udp_return] = '\0'; //acrescenta o /0 no final para garantir o printf

   //   printf("Resposta do servidor %d: %s\n",udp_return,subbuff);
      received_answer = buffer;
    }
    return received_answer;
}

int rsv(char* message){

  struct sockaddr_in server_socket = create_socket_addr();

  int client_socket = send_message(message, server_socket);
  char* answer =  receive_answer(client_socket, server_socket);
  if (answer != NULL){
    printf("%s\n", answer);

  } else {
      printf("Time out!\n");
  }
}

long int calc_rtt(struct timeval initial_time, struct timeval end_time){
  long int elapsed_time = (end_time.tv_usec + 1000 * end_time.tv_sec)
  - (initial_time.tv_usec + 1000 * initial_time.tv_sec);

  long int elapsed_milisseconds = (unsigned) elapsed_time % 1000;
  return elapsed_milisseconds;
}

int rtt(){
  char message[SIZE];
  //do stuff
  snprintf(message, 4, "rtt \n"); //4: Tamanho da string
  struct sockaddr_in server_socket = create_socket_addr();

  int client_socket = send_message(message, server_socket);
  struct timeval initial_time;
  gettimeofday(&initial_time,NULL);

  char *received = receive_answer(client_socket, server_socket);
  if (received != NULL){
    struct timeval end_time;
    gettimeofday(&end_time,NULL);

    long int elapsed_time = calc_rtt(initial_time, end_time);
    printf("RTT = %ld milissegundos \t Horário do servidor: %s\n", elapsed_time,received);
  }else{
    printf("Time out!\n");
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
       srand(time(NULL));
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
