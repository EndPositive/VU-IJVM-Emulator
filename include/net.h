#ifndef NET_H
#define NET_H

#include "ijvm.h"
#include "frame.h"

word_t net_bind(int port);

word_t net_connect(int port, unsigned int host);

char net_recv(word_t netref);

int net_send(word_t netref, char output);

int net_close(word_t netref);

#endif
