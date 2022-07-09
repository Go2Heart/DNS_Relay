//
// Created by Go2Heart on 2022/7/9.
//

#include "dnsClient.h"
socklen_t addrlen;
int initDnsClient() {
    int sockFd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockFd < 0) {
        perror("socket");
        exit(-1);
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(DNS_SERVER_PORT);
    serverAddr.sin_addr.s_addr = inet_addr(DNS_SERVER_ADDR);

    return sockFd;

}

int clientRecv(Cache *cache) {
    char recvbuf[BUFSIZE] = {0};
    if (recvfrom(serverFd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &addrlen) < 0) {
        perror("recvfrom error");
        close(serverFd);
        return -1;

    }
    DNS_HEADER *recvHeader = (DNS_HEADER *) malloc(sizeof(DNS_HEADER));
    DNS_QUESTION *recvQuestion = (DNS_QUESTION *) malloc(sizeof(DNS_QUESTION));
    DNS_RR *recvAnswer = (DNS_RR *) malloc(sizeof(DNS_RR)); /* RR linklist head */
    parseResponse(recvbuf, recvHeader, recvQuestion, recvAnswer);
    //recvAnswer = (recvAnswer->next);
    DNS_RR *linklist = recvAnswer->next;
    unsigned char* name = recvQuestion->qname;
    while(linklist != NULL) {
        if(linklist->type == 0x01 ) {
            //If the answer is an A record, we can get the IP address updated in cache
            if(logLevel >= 3)printf("%d.%d.%d.%d\n", linklist->rdata[0], linklist->rdata[1], linklist->rdata[2], linklist->rdata[3]);
            //adding to cache
            struct dns_map *map = (struct dns_map *)malloc(sizeof(struct dns_map));
            map->name = (uint8_t *)malloc(strlen(name) + 1);
            memcpy(map->name, name, strlen(name) + 1);
            memcpy(map->ip, linklist->rdata, 4);
            insertCache(cache, map->name, map->ip);
            if(logLevel >= 3)printCache(cache, logFile);
        }
        linklist = (linklist->next);
    }


    if(logLevel >= 2)printf("not in cache, using forwarder\n");
    DNS_QUERY *pQuery = (DNS_QUERY *)malloc(sizeof(DNS_QUERY));
    pQuery->header = recvHeader;
    pQuery->question = recvQuestion;
    replyDnsQuery(clientFd, pQuery, recvAnswer->next);

    free(recvHeader);
    free(recvQuestion);
    free(recvAnswer);

    return 1;
}
