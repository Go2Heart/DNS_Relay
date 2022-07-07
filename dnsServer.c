//
// Created by Go2Heart on 2022/6/29.
//

//#include <printf.h>

#include "dnsServer.h"
struct sockaddr_in clientAddr;
struct sockaddr_in serverAddr;
NEW_ID idTable[1024];

int initDnsServer(int port) {
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("socket error");
        printf("%s\n",strerror(errno));
        //return -1;
    }

    memset(&clientAddr, 0, sizeof(clientAddr));
    clientAddr.sin_family = AF_INET;
    clientAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    clientAddr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *) &clientAddr, sizeof(clientAddr)) < 0) {
        perror("bind error");
        //return -1;
    }
    return sockfd;
}

int serverRecv(int sockfd, Cache *cache, Trie *staticTable) {
    char buf[BUFSIZE] = {0};
    unsigned addr_len = sizeof(clientAddr);
    recvfrom(sockfd, buf, sizeof(buf), 0, (struct sockaddr *)&clientAddr, &addr_len);
    if(logLevel >= 2)printf("receiving dns query from %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));
    DNS_HEADER *header = (DNS_HEADER *)malloc(sizeof(DNS_HEADER));
    DNS_QUESTION *question = (DNS_QUESTION *)malloc(sizeof(DNS_QUESTION));
    memcpy(header, buf, sizeof(DNS_HEADER));
    ntohDnsHeader(header);
    if(logLevel >= 2)printf("id: %d\n", header->id);
    header->id = getNewId(header->id, clientAddr);
    if(logLevel >= 2)printf("new id: %d\n", header->id);
    if(logLevel >= 2)printDnsHeader(header);
    char name[256];
    int len = sizeof(DNS_HEADER);
    parseName(buf, sizeof(DNS_HEADER), name);
    question->qname = (uint8_t *)malloc(strlen(name)+1);
    if(logLevel >= 3)printf("qname: %d\n", strlen(name));
    memcpy(question->qname, name, strlen(name));
    question->qname[strlen(name)] = '\0';
    if(logLevel >= 3)printf("qname: %d\n", strlen(name));
    len += (int)strlen((char *)question->qname) + 2; /* etc.  .com /0 type class */
    memcpy(&question->qtype, buf+len, sizeof (question->qclass));
    len += sizeof (question->qtype);
    memcpy(&question->qclass, buf+len, sizeof (question->qclass));
    len += sizeof (question->qclass);
    ntohDnsQuestion(question);
    if(logLevel >= 2)printDnsQuestion(question);

    /* create dns query */
    DNS_QUERY *query = (DNS_QUERY *)malloc(sizeof(DNS_QUERY));
    query->header = header;
    query->question = question;

    unsigned char ip[4] = {0};
    if(question->qtype == 1 && queryCache(cache, name, ip)) { //only query A type in cache
        if(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
            if(logLevel >= 2)printf("Blocked site queried!\n");
            replyDnsQuery(sockfd, query, NULL);
        } else {
            if(logLevel >= 2)printf("Cache hit!\n");
            DNS_RR *answer=NULL;
            if(constructCacheAnswer(ip, &answer, query->header) == -1) {
                printf("construct answer failed\n");
                return -1;
            }
            replyDnsQuery(sockfd, query, answer);
        }
    } else {
        Trie *result = searchTrie(staticTable, name);
        if(result != NULL) {
            memcpy(ip, result->ip, 4);
            DNS_RR *answer=NULL;
            if(constructCacheAnswer(ip, &answer, query->header) == -1) {
                printf("construct answer failed\n");
                return -1;
            }
            replyDnsQuery(sockfd, query, answer);
        } else {
            /* query dns server */
            char buf[BUFSIZE] = {0};
            //createHeader(header);
            htonDnsHeader(header);
            DNS_QUESTION anQuestion = {0};
            //printf("%d\n", strlen(name));
            createQuestion(&anQuestion, query);
            int reqLen = createRequest(buf, header, &anQuestion);
            if(reqLen < 0) {
                return -1;
            }

            int nbytes = sendto(serverFd, buf, reqLen, 0, (struct sockaddr *)&serverAddr, sizeof(serverAddr));
            if (nbytes < 0) {
                perror("sendto");
                return -1;
            }

        }
        free(query);
        free(header);
        free(question);
    }
    if(logLevel >= 2)printf("Cache miss!\n");

    return 1;
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
    if(logLevel >= 2)printf("replying dns query with id: %d\n", header->id);
    if(logLevel >= 2)printDnsHeader(header);
    int answerCount = 0;
    answerCount = query->header->ancount + query->header->nscount + query->header->arcount;
    htonDnsHeader(header);
    //construct dns question
    DNS_QUESTION *question = (DNS_QUESTION *)malloc(sizeof(DNS_QUESTION));
    memset(question, 0, sizeof(DNS_QUESTION));
    createQuestion(question, query);
    char buf[BUFSIZE] = {0};
    int len = createRequest(buf, header, question);
    DNS_RR *rr = (DNS_RR *)malloc(sizeof(*answer));
    if(answerCount > 0) {
        memcpy(rr, answer, sizeof(*answer));
        if(logLevel >= 2)printDnsRR(rr);
        len += createAnswer(buf + len, rr);
        //send back to client
    }else printf("no answer\n");
    sendto(sockfd, buf, len, 0, (struct sockaddr *)&clientAddr, sizeof(clientAddr));
    if(logLevel >= 2)printf("replying dns query to %s:%d\n", inet_ntoa(clientAddr.sin_addr), ntohs(clientAddr.sin_port));

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
