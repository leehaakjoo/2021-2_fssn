#include <iostream>
#include <string.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>
#include <thread>


using namespace std;

void* ReceiveHandler(void*);

int main()
{
    int client;
    int portNum = 65457;
    bool isExit = false;
    int bufsize = 1024;
    char buffer[bufsize];
    char* ip = "127.0.0.1";

    struct sockaddr_in server_addr;
    pthread_t thread_id=0;

    client = socket(AF_INET, SOCK_STREAM, 0);


    if (client < 0) 
    {
        cout << "\nError establishing socket..." << endl;
        exit(1);
    }

    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(portNum);
    server_addr.sin_addr.s_addr = inet_addr(ip);
  
    int *csock = &client;
    if(connect(*csock,(struct sockaddr *)&server_addr, sizeof(server_addr)) != -1){
        pthread_create(&thread_id,0, &ReceiveHandler, (void*)csock);
        pthread_detach(thread_id);
    }
    cout<< "echo-client is activated" << endl;


    do {
        cin >> buffer;
        send(client, buffer, bufsize, 0);
        if(strcmp(buffer, "quit") == 0 ){
            cout << "echo-client is de-activated" <<endl;
            close(client);
            exit(1);
        }
    } while(!isExit);

     return 0;
}

void* ReceiveHandler(void* lp){
    int *csock = (int*)lp;
    char buffer[1024];
    int bufSize = 1024;
    int bytecount;
    bool isExit = false;
    
    do{
        recv(*csock, buffer, bufSize, 0);
        if(strcmp(buffer, "") != 0 ){
            std::cout<<"> received: "<< buffer << std::endl;
        }
        buffer[0] = '\0'; //버퍼 비우기
        if(csock == NULL){
            isExit = true;
            break;
        }   
    } while(!isExit);
    isExit = false;
}