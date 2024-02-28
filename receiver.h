#ifndef S_TALK_RECIEVER_H
#define S_TALK_RECEIVER_H


#include "list.h"

void receiveInit(List * l);
void* receiverThread(void* unused);
void receiveShutdown(void);
void* printerThread(void * unused);

#endif //S_TALK_RECEIVER_H
