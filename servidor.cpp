#include "servidor.hpp"

struct shared_data *shared;

string mensagem;
int new_socket;

int main(void) 
{   
    string msg;
    int server_fd, buffer;
    initialize_shared();
    socket_Create_and_Conect(server_fd, new_socket); 
    //cout << "CRIOU E CONECTOU" << endl;
    read(new_socket , &buffer, sizeof(buffer));
    //cout << "LEU BUFFER" << endl;
    //cout << buffer << endl;                                                      
    philosopher(buffer); 
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



void philosopher(int i)
{
  while(1)
  {
    sleep(1);
    take_spoon(i);
    sleep(2);
    put_spoon(i);
    sleep(1);
  }
}

void take_spoon(int i)
{
  sem_wait(&shared->spoon);
  shared->state[i] = HUNGRY;
  //printf("Filosofo %d tem fome.\n",i+1);
  mensagem = "Filosofo " + std::to_string(i+1) + " tem fome.";
  sendString(mensagem);
  test(i);
  sem_post(&shared->spoon);
  sem_wait(&shared->phil[i]);
}

void put_spoon(int i)
{
  sem_wait(&shared->spoon);
  shared->state[i] = THINKING;
  mensagem = "Filosofo " + std::to_string(i+1) + " largou a colher " + std::to_string(LEFT+1) + " e " + std::to_string(i+1) + ".";
  sendString(mensagem);
  mensagem = "Filosofo " + std::to_string(i+1) + " está pensando.";
  sendString(mensagem);
  test(LEFT);
  test(RIGHT);
  sem_post(&shared->spoon);
}

void test(int i)
{
  if( shared->state[i] == HUNGRY && shared->state[LEFT] != EATING && shared->state[RIGHT] != EATING)
  {
    shared->state[i] = EATING;
    mensagem = "Filosofo " + std::to_string(i+1) + " pegou a colher " + std::to_string(LEFT+1) + " e " + std::to_string(i+1) + ".";
    sendString(mensagem);
    mensagem = "Filosofo " + std::to_string(i+1) + " está comendo.";
    sendString(mensagem);
    sem_post(&shared->phil[i]);
  }
}

void initialize_shared()
{
  int i;
  int prot=(PROT_READ|PROT_WRITE);
  int flags=(MAP_SHARED|MAP_ANONYMOUS);
  shared= static_cast<shared_data*>(mmap(0,sizeof(*shared),prot,flags,-1,0));
  memset(shared,'\0',sizeof(*shared));
  sem_init(&shared->spoon,1,1);
  for(i=0;i<N;++i) sem_init(&shared->phil[i],1,1);
}

void finalize_shared()
{
  int i;
  for(i=0;i<N;++i) sem_destroy(&shared->phil[i]);
  munmap(shared, sizeof(*shared));
}

void sendString(const string& mensagem){
    uint32_t msgLength = mensagem.size();
    uint32_t sndMsgLength = htonl(msgLength); // Ensure network byte order
    send(new_socket,&sndMsgLength ,sizeof(uint32_t) ,MSG_CONFIRM); // Send the message length
    send(new_socket,mensagem.c_str() ,msgLength ,MSG_CONFIRM); // Send the message data 
}