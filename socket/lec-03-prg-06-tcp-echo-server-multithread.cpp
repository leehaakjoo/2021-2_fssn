#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <netinet/in.h>
#include <resolv.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>
#include <thread>
#include <iostream>


void* SocketHandler(void*);
static int threadNum = 0;
int main(){

    int portNum = 65457;

    struct sockaddr_in server_addr, client_addr;
    int server;
    int * p_int;
    int* csock;

    socklen_t addr_size = 0;
    
    pthread_t thread_id=0;


    server = socket(AF_INET, SOCK_STREAM, 0);
    if(server == -1){
        printf("Error initializing socket %d\n", errno);
        goto FINISH;
    }
    
    p_int = (int*)malloc(sizeof(int));
    *p_int = 1;
        
    if( (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, (char*)p_int, sizeof(int)) == -1 )||
        (setsockopt(server, SOL_SOCKET, SO_KEEPALIVE, (char*)p_int, sizeof(int)) == -1 ) ){
        printf("Error setting options %d\n", errno);
        free(p_int);
        goto FINISH;
    }
    free(p_int);

    server_addr.sin_family = AF_INET ;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(&(server_addr.sin_zero), 0, 8);
    
    if( bind( server, (sockaddr*)&server_addr, sizeof(server_addr)) == -1 ){
        fprintf(stderr,"Error binding to socket, make sure nothing else is listening on this port %d\n",errno);
        goto FINISH;
    }
    if(listen( server, 10) == -1 ){
        fprintf(stderr, "Error listening %d\n",errno);
        goto FINISH;
    }

    //Now lets do the server stuff

    addr_size = sizeof(sockaddr_in);
    
    while(true){
        csock = (int*)malloc(sizeof(int));
        if((*csock = accept( server, (sockaddr*)&client_addr, &addr_size))!= -1){
            std::cout<< "client connected by IP address 127.0.0.1 with Port number ";
            std::cout<< ntohs(client_addr.sin_port)<<std::endl;  
            threadNum++;
            pthread_create(&thread_id, 0, &SocketHandler, (void*)csock ); // 쓰레드 활성화
            pthread_detach(thread_id);
        }
        else{
            fprintf(stderr, "Error accepting %d\n", errno);
        }        
    }
    
FINISH:
;
}

void* SocketHandler(void* lp){
    int *server = (int*)lp;
    std::thread::id this_id = std::this_thread::get_id();
    char buffer[1024];
    int bufSize = 1024;
    bool isExit = false;

    do{
        recv(*server, buffer, bufSize, 0);
        if(strcmp(buffer, "quit") == 0){
            threadNum--;
            std::cout<<"active threads are remained: " << threadNum << std::endl;
            //연결된 client 없을 때 -> y: 프로그램 종료, n: 대기
            if(threadNum == 0){
                char Yn;
                std::cout<< "Do you want to exit program? (y/n) ";
                std::cin>>Yn;
                if (Yn == 'y'){
                    exit(1);
                }
            }
            isExit = true;
            goto FINISH;
        } else{
            std::cout<<"echoed: "<<buffer << "  By Thread-" << this_id <<std::endl;
        }
        send(*server, buffer, bufSize, 0);
    } while(!isExit);
    free(server);
FINISH:
    free(server);
    return 0;
}