
// This files acts as a manager to build the socket, delete or exiting the program.
#include "receiver.h"
#include "sender.h"
#include "manager.h"
#include <stdio.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>
#include <stdlib.h>
#include <zconf.h>

static pthread_mutex_t mutexM = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condM = PTHREAD_COND_INITIALIZER;

struct sockaddr_in sockIn;
int socketDescriptor;
struct addrinfo *remoteMachine;

// Configures a UDP socket for the local machine, binding it to the specified port.
void mySocketSetup(char* PORT) {
    memset(&sockIn, 0, sizeof(sockIn));
    sockIn.sin_family = AF_INET;
    sockIn.sin_addr.s_addr = htonl(INADDR_ANY);
    sockIn.sin_port = htons(atoi(PORT));
    socketDescriptor = socket(AF_INET, SOCK_DGRAM, 0);
    int check = bind(socketDescriptor, (struct sockaddr *)&sockIn, sizeof(sockIn));
    if(check != 0)
    {
        fprintf(stderr,"Socket bind error : %s \n", gai_strerror(check));
        exitSignal();
    }
}

// Sets up a UDP socket address for the remote machine using the given IP address and port.
void remoteSocketSetup(char *remoteIP, char *remotePort) {
    struct addrinfo hints;
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    int check = getaddrinfo(remoteIP, remotePort, &hints, &remoteMachine);
    if(check != 0) {
        fprintf(stderr, "getaddr error: %s \n", gai_strerror(check));
        exitSignal();
    }
}

//returns the SocketDescriptor
int getSocketDescriptor(void) {
    return socketDescriptor;
}

// returns Remote Socket
struct addrinfo *getRemoteSocket() {
    return remoteMachine;
}

//signals the exit program
void exitSignal() {
    pthread_mutex_lock(&mutexM);
    {
        pthread_cond_signal(&condM);
    }
    pthread_mutex_unlock(&mutexM);
}

//exiting the program
void exitProgram(void) {
    pthread_mutex_lock(&mutexM);
    {
        pthread_cond_wait(&condM, &mutexM);
    }
    pthread_mutex_unlock(&mutexM);

    printf("Exiting Program \n");
    sendShutdown();
    receiveShutdown();
    close(socketDescriptor);
    freeaddrinfo(remoteMachine);
    remoteMachine = NULL;
    pthread_mutex_destroy(&mutexM);
    pthread_cond_destroy(&condM);
}

//To empty the list 
void freeItem(void* item) {
    free(item);
}