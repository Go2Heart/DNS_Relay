//
// Created by Go2Heart on 2022/7/5.
//

#ifndef DNSRELAY_GLOBAL_H
#define DNSRELAY_GLOBAL_H
#include "dnsStructure.h"
 int serverFd;
 int clientFd;
 struct sockaddr_in clientAddr;
 struct sockaddr_in serverAddr;
 socklen_t addrlen;

#endif //DNSRELAY_GLOBAL_H
