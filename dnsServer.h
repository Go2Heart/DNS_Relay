//
// Created by Go2Heart on 2022/6/29.
// @brief: used to answer local dns query

#ifndef DNSRELAY_DNSSERVER_H
#define DNSRELAY_DNSSERVER_H


#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dnsStructure.h"
#include "dnsResolver.h"




/*
 * @brief: initilize the dns server to listen the port
 * @param: port: the port to listen
 * @return: 0: success, -1: failed
 */
extern int initDnsServer(int port);

/*
 * @brief: receive the dns query from client
 * @param: sockfd: the socket fd to receive
 * @return: the dns question
 */
extern int serverRecv(int sockfd, Cache *cache, Trie *staticTable);

/*
 * @brief: reply the dns query to client
 * @param: sockfd: the socket fd to reply
 * @param: question: the dns question
 * @param: answer: the dns answer
 * @return: 0: success, -1: failed
 */
extern int replyDnsQuery(int sockfd, DNS_QUERY *query, DNS_RR *answer);

typedef struct new_id {
    uint16_t id;
    int ttl;
    struct sockaddr_in addr;
}NEW_ID;
extern NEW_ID idTable[1024];

bool isExpired(NEW_ID id);

uint16_t getNewId(uint16_t id, struct sockaddr_in addr);
#endif //DNSRELAY_DNSSERVER_H
