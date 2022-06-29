//
// Created by Go2Heart on 2022/5/26.
//

#ifndef DNSRELAY_DNSRESOLVER_H
#define DNSRELAY_DNSRESOLVER_H

#include "dnsStructure.h"
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


extern void printDnsHeader(DNS_HEADER *header);
extern void printDnsQuestion(DNS_QUESTION *question);
extern void printDnsRR(DNS_RR *rr);
extern void ntohDnsHeader(DNS_HEADER *header);
extern void ntohDnsQuestion(DNS_QUESTION *question);
extern void ntohDnsRR(DNS_RR *rr);

extern void htonDnsHeader(DNS_HEADER *header);
extern void htonDnsQuestion(DNS_QUESTION *question);
extern void htonDnsRR(DNS_RR *rr);




extern bool createHeader(DNS_HEADER *header);
extern bool createQuestion(DNS_QUESTION *question, const char *name);
extern int createRequest(char *request, DNS_HEADER *header, DNS_QUESTION *question);
extern int parseName(char *answer, int offset, char *name);
extern int parseResponse(char *response, DNS_HEADER *header, DNS_QUESTION *question, DNS_RR *answer);
extern int dnsQuery(const char *name, Cache *cache);




#endif //DNSRELAY_DNSRESOLVER_H
