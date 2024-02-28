#ifndef S_TALK_SENDER_H
#define S_TALK_SENDER_H

#include "list.h"

void sendInit(List * l);
void* senderThread(void* unused);
void* keyboardThread(void* unused);
void sendShutdown(void);

#endif //S_TALK_SENDER_H
