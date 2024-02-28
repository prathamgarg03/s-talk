#include <stdio.h>
#include <stdlib.h>
#include "list.h"
#include "receiver.h"
#include "sender.h"
#include "manager.h"

// Main function that sets up sockets, initializes lists, starts threads for sending and receiving,
// and cleans up resources before exiting the program.
int main(int argc, char **argv) {
    printf("Welcome! Assignment 2 by Pratham Garg (pga47) and Gourav Sharma (ghs4) \n");
    printf("Start Chatting: \n");
    if(argc != 4) {
        fprintf(stderr, "usage %s <my port number> <remote machine name> <remote port number>\n", argv[0]);
        exit(0);
    }

    mySocketSetup(argv[1]);
    remoteSocketSetup(argv[2], argv[3]);
    List * receive = List_create();
    List * send = List_create();

    receiveInit(receive);
    sendInit(send);

    exitProgram();
    List_free(receive,freeItem);
    List_free(send,freeItem);
    return 0;
}