#ifndef servidor_hpp
#define servidor_hpp

#include "global.hpp"

#define PENSAR 0
#define FOME 1
#define COMER 2
#define ESQUERDA (nfilosofo+4)%N 
#define DIREITA (nfilosofo+1)%N 

struct shared_data {
  sem_t colher;
  sem_t filosofo[N];
  int estado[N];
};

void initialize_shared();
void finalize_shared();
void socket_Create_and_Conect(int &server_fd, int &new_socket);
void *filosofo(int num);
void agarraGarfo(int);
void deixaGarfo(int);
void testar(int);

#endif