#ifndef S_TALK_MANAGER_H
#define S_TALK_MANAGER_H

void mySocketSetup(char* PORT);
void remoteSocketSetup(char* remoteIP, char* remotePort);
int getSocketDescriptor(void);
struct addrinfo* getRemoteSocket();
void exitSignal(void);
void exitProgram(void);
void freeItem(void* item);

#endif //S_TALK_MANAGER_H
