#include "global.hpp"
#include <vector>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <stdint.h>

int socket_Create_and_Conect();
void printStringSocket(int);

int main(void) 
{ 
    int filosofos = socket_Create_and_Conect();
    string buffer;
    if (filosofos == -1){
        return 0;
    } 

    int i;
    pid_t pid, pids[N]; // process ids
    for(i=0;i<N;++i)
    {
        pid = fork();
        if(pid==0)
        {
            // child
            send(filosofos, &i, sizeof(i), 0);
            _exit(0);
        }
        else if(pid>0)
        {
            // parent
            pids[i] = pid;
            //printf("pids[%d]=%d\n",i,pids[i]);
        }
        else
        {
            perror("fork");
            _exit(0);
        }
    }
    while(1){
        printStringSocket(filosofos);
    }
  
    for(i=0;i<N;++i) 
        waitpid(pids[i],NULL,0);

    //while(1){};
    return 0;
}

int socket_Create_and_Conect()
{
    int sock = 0;
    struct sockaddr_in serv_addr;

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n ERRO NA CRIAÇÃO DO SOCKET \n"); 
        return -1; 
    } 
   
    serv_addr.sin_family = AF_INET; 
    serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)  
    { 
        printf("\ENDEREÇO INVALIDO/NÃO SUPORTADO \n"); 
        return -1; 
    } 
   
    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
    { 
        printf("\nFALHA NA CONEXÃO\n"); 
        return -1; 
    }

    return sock;  
}

void printStringSocket(int mensagem){
    uint32_t msgLength;
    recv(mensagem,&msgLength,sizeof(uint32_t),0);
    msgLength = ntohl(msgLength); 
    std::vector<uint8_t> pkt; 
    pkt.resize(msgLength,0x00);
    recv(mensagem,&(pkt[0]),msgLength,0);
    std::string tmp;
    tmp.assign((char*)&(pkt[0]),pkt.size()); 
    cout << tmp << endl;
}