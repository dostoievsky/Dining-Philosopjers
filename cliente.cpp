#include "global.hpp"

int socket_Create_and_Conect();
   
int main(void) 
{ 
    int sock = socket_Create_and_Conect();
    
    if (sock == -1){
        return 0;
    } 

    int i, buffer;
    pid_t pid, pids[N]; // process ids
    for(i=0;i<N;++i)
    {
        pid = fork();
        if(pid==0)
        {
            // child
            send(sock, &i, sizeof(i), 0);
            //while(1){
                read( sock , &buffer, sizeof(buffer));
                cout << " " << buffer << " ";
            //}
            _exit(0);
        }
        else if(pid>0)
        {
            // parent
            pids[i] = pid;
            printf("pids[%d]=%d\n",i,pids[i]);
        }
        else
        {
            perror("fork");
            _exit(0);
        }
    }
  
    for(i=0;i<N;++i) 
        waitpid(pids[i],NULL,0);

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