#include "servidor.hpp"

struct shared_data *compartilhado;

std::string mensagem;
int new_socket;

int main(void) 
{ 
  initialize_shared();
  int server_fd, buffer;
  
  socket_Create_and_Conect(server_fd, new_socket); 
  //while(1){  
    read( new_socket , &buffer, sizeof(buffer));
    cout << "CHEGOU AQUI";
    filosofo(buffer);
  //}
  //for (int i = 0; i < buffer; i++){
      //filosofo(i);
      //send(new_socket, &r, sizeof(r), 0);
  //}
  finalize_shared();
  
    
  return 0; 
} 


void socket_Create_and_Conect(int &server_fd, int &new_socket){
    struct sockaddr_in address; 
    int opt = 1; 
    int addrlen = sizeof(address); 

    // Creating socket file descriptor 
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
       
    // Forcefully attaching socket to the port 8080 
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address.sin_family = AF_INET; 
    address.sin_addr.s_addr = INADDR_ANY; 
    address.sin_port = htons( PORT ); 
       
    // Forcefully attaching socket to the port 8080 
    if (bind(server_fd, (struct sockaddr *)&address,  
                                 sizeof(address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(server_fd, 3) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    } 
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,  
                       (socklen_t*)&addrlen))<0) 
    { 
        perror("accept"); 
        exit(EXIT_FAILURE); 
    } 
}

void *filosofo(int num)
{
   while(1)
   {
      int i = num;
      sleep(1);
      agarraGarfo(i);
      sleep(1);
      deixaGarfo(i);
   }
}

void agarraGarfo(int nfilosofo)
{
   sem_wait(&compartilhado->colher);
   compartilhado->estado[nfilosofo] = FOME;
   mensagem = "Filosofo " + std::to_string(nfilosofo+1) + "tem fome.";
   send(new_socket, &mensagem, sizeof(mensagem), 0);
   //printf("Filosofo %d tem fome.\n", nfilosofo+1);
   //+1 para imprimir filosofo 1 e nao filosofo 0
   testar(nfilosofo);
   sem_post(&compartilhado->colher);
   sem_wait(&compartilhado->filosofo[nfilosofo]);
    sleep(1);
}

void deixaGarfo(int nfilosofo)
{
  sem_wait(&compartilhado->colher);
  compartilhado->estado[nfilosofo]=PENSAR;
  mensagem = "Filosofo " + std::to_string(nfilosofo+1) + " deixou os garfos " + std::to_string(ESQUERDA+1) + " e " + std::to_string(nfilosofo+1);
  send(new_socket, &mensagem, sizeof(mensagem), 0);
  //printf("Filosofo %d deixou os garfos %d e %d.\n", nfilosofo+1, ESQUERDA+1, nfilosofo+1);
  mensagem = "Filosofo " + std::to_string(nfilosofo+1) + " esta a pensar.";
  send(new_socket, &mensagem, sizeof(mensagem), 0);
  //printf("Filosofo %d esta a pensar.\n", nfilosofo+1);
   testar(ESQUERDA);
   testar(DIREITA);
   sem_post(&compartilhado->colher);
}

void testar(int nfilosofo)
{
   if(compartilhado->estado[nfilosofo]==FOME && compartilhado->estado[ESQUERDA]
 !=COMER && compartilhado->estado[DIREITA]!=COMER)
   {
      compartilhado->estado[nfilosofo]=COMER;
      sleep(1);
      mensagem = "Filosofo " + std::to_string(nfilosofo+1) + " agarrou os garfos " + std::to_string(ESQUERDA+1) + " e " + std::to_string(nfilosofo+1);
      send(new_socket, &mensagem, sizeof(mensagem), 0);
      //printf("Filosofo %d agarrou os garfos %d e %d.\n", nfilosofo+1, ESQUERDA+1, nfilosofo+1);
      mensagem = "Filosofo " + std::to_string(nfilosofo+1) + " está a comer.";
      send(new_socket, &mensagem, sizeof(mensagem), 0);
      //printf("Filosofo %d esta a comer.\n", nfilosofo+1);
      sem_post(&compartilhado->filosofo[nfilosofo]);
   }
}

void initialize_shared()
{
  int i;
  int prot=(PROT_READ|PROT_WRITE);
  int flags=(MAP_SHARED|MAP_ANONYMOUS);
  compartilhado= static_cast<shared_data*>(mmap(0,sizeof(*compartilhado),prot,flags,-1,0));
  memset(compartilhado,'\0',sizeof(*compartilhado));
  sem_init(&compartilhado->colher,1,1);
  for(i=0;i<N;++i) sem_init(&compartilhado->filosofo[i],1,1);
}

void finalize_shared()
{
  int i;
  for(i=0;i<N;++i) sem_destroy(&compartilhado->filosofo[i]);
  munmap(compartilhado, sizeof(*compartilhado));
}