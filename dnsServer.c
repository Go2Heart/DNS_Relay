//
// Created by Go2Heart on 2022/6/29.
//

//#include <printf.h>
#include <stdio.h>
#include <stdlib.h>
#include "dnsServer.h"
#include "dnsResolver.h"

int initDnsServer(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        printf("%s\n",strerror(errno));
        //return -1;
    }

    memset(&server_addr, 0,  sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    server_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("bind error");
        //return -1;
    }
    return sockfd;
}

DNS_QUERY *receiveDnsQuery(int sockfd) {
    char buf[BUFSIZE] = {0};
    unsigned addr_len = sizeof(server_addr);
    recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&server_addr, &addr_len);
    printf("receiving dns query from %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    DNS_HEADER *header = (DNS_HEADER *)malloc(sizeof(DNS_HEADER));
    DNS_QUESTION *question = (DNS_QUESTION *)malloc(sizeof(DNS_QUESTION));
    memcpy(header, buf, sizeof(DNS_HEADER));
    ntohDnsHeader(header);
    printf("id: %d\n", header->id);
    header->id = getNewId(header->id, server_addr);
    printf("new id: %d\n", header->id);
    printDnsHeader(header);
    char name[256];
    int len = sizeof(DNS_HEADER);
    parseName(buf, sizeof(DNS_HEADER), name);
    question->qname = (uint8_t *)malloc(strlen(name) + 1);
    memcpy(question->qname, name, strlen(name) + 1);
    len += (int)strlen((char *)question->qname) + 2; /* etc.  .com /0 type class */
    memcpy(&question->qtype, buf+len, sizeof (question->qclass));
    len += sizeof (question->qtype);
    memcpy(&question->qclass, buf+len, sizeof (question->qclass));
    len += sizeof (question->qclass);
    ntohDnsQuestion(question);
    printDnsQuestion(question);

    /* create dns query */
    DNS_QUERY *query = (DNS_QUERY *)malloc(sizeof(DNS_QUERY));
    query->header = header;
    query->question = question;

    return query;
}
void encodeDomainName(uint8_t *buffer, const char *domain)
{
    uint8_t *buf = buffer;
    const char *beg = domain;
    const char *pos;
    int len = 0;
    int i = 0;

    while ((pos = strchr(beg, '.')))
    {
        len = pos - beg;
        buf[i] = len;
        i += 1;
        memcpy(buf + i, beg, len);
        i += len;
        beg = pos + 1;
    }

    len = strlen(domain) - (beg - domain);
    buf[i] = len;
    i += 1;
    memcpy(buf + i, beg, len);
    i += len;

    buf[i] = 0;
    i += 1;

    buffer += i;
}

int replyDnsQuery(int sockfd, DNS_QUERY *query, DNS_RR *answer) {
    //construct dns header
    DNS_HEADER *header = (DNS_HEADER *)malloc(sizeof(DNS_HEADER));
    memcpy(header, query->header, sizeof(DNS_HEADER));
    //header->flags = htons(query->header->flags);
    //header->ancount = query->header->ancount;
    header->id = (idTable[query->header->id].id);
    printf("replying dns query with id: %d\n", header->id);
    printDnsHeader(header);
    int answerCount = 0;
    answerCount = query->header->ancount + query->header->nscount + query->header->arcount;
    htonDnsHeader(header);


    //construct dns question
    DNS_QUESTION *question = (DNS_QUESTION *)malloc(sizeof(DNS_QUESTION));
    memset(question, 0, sizeof(DNS_QUESTION));
    createQuestion(question, query);
    //printf("question qname: %s\n", question->qname);

    //memcpy(question, query->question, sizeof(DNS_QUESTION));
    /*htonDnsQuestion(question);
    printf("qname: %s\n", question->qname);
    uint8_t qname[256] = {0};
    encodeDomainName(qname, (char *)question->qname);
    memcpy(question->qname, qname, strlen((char *)qname));*/
    //construct dns answer
    char buf[BUFSIZE] = {0};
    int len = createRequest(buf, header, question);
    DNS_RR *rr = (DNS_RR *)malloc(sizeof(*answer));
    if(answerCount > 0) {
        memcpy(rr, answer, sizeof(*answer));
        printDnsRR(rr);
        len += createAnswer(buf + len, rr);
        //send back to client
    }else printf("no answer\n");
    sendto(sockfd, buf, len, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
    printf("replying dns query to %s:%d\n", inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));

    //free memory
    free(header);
    free(question);
    free(rr);

    free(answer);
    return 0;
}

bool isExpired(NEW_ID id) {
    return id.ttl < time(NULL);
}

uint16_t getNewId(uint16_t id, struct sockaddr_in addr) {
    uint16_t i;
    for (i = 0; i < 1024; ++i) {
        if(isExpired(idTable[i])) {
            idTable[i].id = id;
            idTable[i].ttl = time(NULL) + 10;
            idTable[i].addr = addr;
            return i;
        }
    }
    return i;
}
