#ifndef global_hpp
#define global_hpp

#include <stdio.h>
#include <stdlib.h>
#include <semaphore.h>
#include <pthread.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/mman.h>
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 

using std::cout;
using std::endl;
using std::string;

#define PORT 8080 
#define N 5

struct shared_data {
  sem_t spoon;
  sem_t phil[N];
  int state[N];
};
#endif