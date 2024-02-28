#include "receiver.h"
#include "manager.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <string.h>

#define MSG_MAX_LEN 1024

static pthread_t receiveThread;
static pthread_t printThread;
static pthread_mutex_t mutexR = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t condR = PTHREAD_COND_INITIALIZER;

static List * list;

// Initiates receiving functionality by assigning the provided List pointer,
// and creating separate threads for receiving messages and printing them.
void receiveInit(List * l) {
    list = l;
    pthread_create(
            &receiveThread,
            NULL,
            receiverThread,
            NULL
            );
    pthread_create(
            &printThread,
            NULL,
            printerThread,
            NULL
            );
}

// Receiver thread function responsible for receiving messages,
// terminating them properly, and adding them to the List for further processing.
void *receiverThread(void *unused) {
    int socketDescriptor = getSocketDescriptor();

    while(1) {
        struct sockaddr_in sinRemote;
        unsigned int sin_len = sizeof(sinRemote);

        char messageRx[MSG_MAX_LEN];
        int bytesRx = recvfrom(socketDescriptor,
                               messageRx, MSG_MAX_LEN,
                               0,
                               (struct sockaddr *) &sinRemote, &sin_len);
        if(bytesRx < 0)
        {
            fprintf(stderr,"Sendto Error : %d \n",bytesRx);
            exitSignal();
        }

        int terminateIdx = (bytesRx < MSG_MAX_LEN) ? bytesRx : MSG_MAX_LEN - 1;
        messageRx[terminateIdx] = '\0';
        int len = strlen(messageRx);
        char * msg;
        pthread_mutex_lock(&mutexR);
        {
            msg = (char*)malloc(len);
            strcpy(msg, messageRx);
            List_append(list, msg);
            pthread_cond_signal(&condR);
        }
        pthread_mutex_unlock(&mutexR);
    }
    return NULL;
}

//Prints the message received and frees the memory allocated
void* printerThread(void * unused) {
    while(1) {
        char * msg;
        pthread_mutex_lock(&mutexR);
        {
            pthread_cond_wait(&condR, &mutexR);
            msg = List_first(list);
            List_remove(list);
        }

        pthread_mutex_unlock(&mutexR);
        char *ch = "[Friend]: ";
        fputs(ch, stdout);
        fputs(msg, stdout);
        fflush(stdout);
        
        if (*(msg) == '!' && *(msg + 2) == '\0') {
            free(msg);
            exitSignal();
        } else {
            free(msg);
        }
    }
    return NULL;
}

// Shuts down the receiving and printing threads by canceling them and waiting for their completion.
void receiveShutdown(void) {
    List_free(list, freeItem);
    pthread_cancel(receiveThread);
    pthread_cancel(printThread);
    pthread_cond_destroy(&condR);
    pthread_mutex_destroy(&mutexR);

    pthread_join(receiveThread, NULL);
    pthread_join(printThread, NULL);
}

