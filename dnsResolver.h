//
// Created by Go2Heart on 2022/5/26.
//

#ifndef DNSRELAY_DNSRESOLVER_H
#define DNSRELAY_DNSRESOLVER_H

#include "dnsStructure.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

extern bool createHeader(DNS_HEADER *header);
extern bool createQuestion(DNS_QUESTION *question, const char *name);
extern int createRequest(char *request, DNS_HEADER *header, DNS_QUESTION *question);
extern int dnsQuery(const char *name);



#endif //DNSRELAY_DNSRESOLVER_H
