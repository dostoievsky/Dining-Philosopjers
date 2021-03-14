#ifndef servidor_hpp
#define servidor_hpp

#include "global.hpp"

#define PENSAR 0
#define LEFT (i+4)%N
#define RIGHT (i+1)%N
#define THINKING 0
#define HUNGRY 1
#define EATING 2

struct shared_data {
  sem_t spoon;
  sem_t phil[N];
  int state[N];
};

void initialize_shared();
void finalize_shared();

void socket_Create_and_Conect(int &server_fd, int &new_socket);
void philosopher(int i);
void test(int i);
void take_spoon(int i);
void put_spoon(int i);

void sendString(const string&);

#endif