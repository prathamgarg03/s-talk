
//This file is the sender file 
#include "sender.h"
#include "manager.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>
#include <netdb.h>

#define MSG_MAX_LEN 1024

static pthread_t sendThread;
static pthread_t inputThread;
static pthread_mutex_t mutexS = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condS = PTHREAD_COND_INITIALIZER;

static List * list;

// Initiates sending functionality by assigning the provided List pointer,
// and creating separate threads for keyboard input and sender operations.
void sendInit(List * l) {
    list = l;
    pthread_create(
            &inputThread,
            NULL,
            keyboardThread,
            NULL);
    pthread_create(
            &sendThread,
            NULL,
            senderThread,
            NULL);
}
// Listner for the users input
void *keyboardThread(void *unused) {
    while(1) {
        char messageRx[MSG_MAX_LEN];
        fgets(messageRx, MSG_MAX_LEN, stdin);
        int len  = strlen(messageRx);
        char * msg = (char*)malloc(len + 1);
        strcpy(msg, messageRx);
        if(len > 1) {
            pthread_mutex_lock(&mutexS);
            {
                List_append(list, msg);
                pthread_cond_signal(&condS);
            }
            pthread_mutex_unlock(&mutexS);
        }
    }
    return NULL;
}

//Sends it to the reciver and listens for any other message sent by the user if not 
//unlocks the mutex so that the thread can be used by any other function
void *senderThread(void *unused) {
    int socketDescriptor = getSocketDescriptor();
    struct addrinfo* sinRemote = getRemoteSocket();
    while(1) {
        pthread_mutex_lock(&mutexS);
        {
            pthread_cond_wait(&condS, &mutexS);
            char * messageTx = List_first(list);
            unsigned int sin_len = strlen(messageTx);
            int bytesSent = sendto(socketDescriptor,
                                       messageTx, sin_len + 1,
                                       0,
                                       sinRemote->ai_addr, sinRemote->ai_addrlen);
            if(bytesSent < 0) {
                fprintf(stderr,"Sendto Error : %d \n",bytesSent);
                exitSignal();
            }
            if (*(messageTx) == '!' && *(messageTx + 2) == '\0') {
                free(messageTx);
                exitSignal();
            } else {
                List_remove(list);
                free(messageTx);
            }
        }
        pthread_mutex_unlock(&mutexS);
    }
    return NULL;
}

// Shuts down the input and sending threads by canceling them and waiting for their completion.
void sendShutdown(void) {
    List_free(list, freeItem);
    pthread_cancel(inputThread);
    pthread_cancel(sendThread);
    pthread_cond_destroy(&condS);
    pthread_mutex_destroy(&mutexS);


    pthread_join(inputThread, NULL);
    pthread_join(sendThread, NULL);
}

