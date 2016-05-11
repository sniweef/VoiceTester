#ifndef VOIPHELPER_MAIN_H_INCLUDED
#define VOIPHELPER_MAIN_H_INCLUDED

void * handleClientConn(void * pArg);
int call(unsigned long long callNumber, uint32_t ip, uint16_t port);
int rejectCall();
int acceptCall();
int downCall();

#endif // VOIPHELPER_MAIN_H_INCLUDED
