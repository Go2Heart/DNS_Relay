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
    if (recvfrom(clientFd, recvbuf, sizeof(recvbuf), 0, (struct sockaddr *)&serverAddr, &addrlen) < 0) {
        perror("recvfrom error");
        close(clientFd);
        return -1;

    }
    DNS_HEADER *recvHeader = (DNS_HEADER *) malloc(sizeof(DNS_HEADER));
    DNS_QUESTION *recvQuestion = (DNS_QUESTION *) malloc(sizeof(DNS_QUESTION));
    DNS_RR *recvAnswer = (DNS_RR *) malloc(sizeof(DNS_RR)); /* RR linklist head */
    parseResponse(recvbuf, recvHeader, recvQuestion, recvAnswer);
    //recvAnswer = (recvAnswer->next);
    DNS_RR *linklist = recvAnswer->next;
    unsigned char* name = recvQuestion->qname;
    Ip *cacheIp = (Ip *)malloc(sizeof(Ip));
    cacheIp->next = NULL;
    Ip *ip = cacheIp;
    while(linklist != NULL) {
        if(linklist->type == 0x01 ) {
            //If the answer is an A record, we can get the IP address updated in cache
            if(logLevel >= 3)printf("%d.%d.%d.%d\n", linklist->rdata[0], linklist->rdata[1], linklist->rdata[2], linklist->rdata[3]);
            //adding to cache
            ip->next = malloc(sizeof(Ip));
            memcpy(ip->next->ip, linklist->rdata, 4);
            ip->next->next = NULL;
            ip = ip->next;
        }
        linklist = (linklist->next);
    }
    insertCache(cache, name, cacheIp);
    if(logLevel >= 3)printCache(cache, logFile);

    if(logLevel >= 2)printf("not in cache, using forwarder\n");
    DNS_QUERY *pQuery = (DNS_QUERY *)malloc(sizeof(DNS_QUERY));
    pQuery->header = recvHeader;
    pQuery->question = recvQuestion;
    replyDnsQuery(serverFd, pQuery, recvAnswer->next);

    free(recvHeader);
    free(recvQuestion);
    free(recvAnswer);

    return 1;
}
