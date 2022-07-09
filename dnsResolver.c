//
// Created by Go2Heart on 2022/5/26.
//

#include "dnsResolver.h"
#include <errno.h>
int clientFd;
int serverFd;
struct sockaddr_in clientAddr;
struct sockaddr_in serverAddr;
socklen_t addrlen = sizeof(struct sockaddr_in);
 void printDnsHeader(DNS_HEADER *header) {
    printf("===== DNS Header =====\n");
    printf("id: 0x%x\n", (header->id));
    printf("flags: 0x%x\n", (header->flags));
    printf("qdcount: %d\n", (header->qdcount));
    printf("ancount: %d\n", (header->ancount));
    printf("nscount: %d\n", (header->nscount));
    printf("arcount: %d\n", (header->arcount));
    printf("=====================\n");
}

 void printDnsQuestion(DNS_QUESTION *question) {
    printf("===== DNS Question =====\n");

    printf("qname: %s\n", question->qname);
    printf("qtype: %d\n", (question->qtype));
    printf("qclass: %d\n", (question->qclass));
    printf("=====================\n");
}

 void printDnsRR(DNS_RR *rr) {
    printf("===== DNS RR =====\n");
    if(rr->name[0]>>4 == 0xc)printf("name: using PTR\n");
    else printf("name: %s\n", rr->name);
    printf("type: %d\n", (rr->type));
    printf("class: %d\n", (rr->class));
    printf("ttl: %d\n", (rr->ttl));

    printf("rdlength: %d\n", (rr->rdlength));
    if(rr->type == 0x1) printf("rdata: %d.%d.%d.%d\n", rr->rdata[0], rr->rdata[1], rr->rdata[2], rr->rdata[3]);
    else printf("rdata: %s\n", rr->rdata);
    printf("=====================\n");
}
 void ntohDnsHeader(DNS_HEADER *header) {
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);
}
 void ntohDnsQuestion(DNS_QUESTION *question) {
    question->qtype = ntohs(question->qtype);
    question->qclass = ntohs(question->qclass);
}
 void ntohDnsRR(DNS_RR *rr) {
    rr->type = ntohs(rr->type);
    rr->class = ntohs(rr->class);
    rr->ttl = ntohl(rr->ttl);
    rr->rdlength = ntohs(rr->rdlength);
}
 bool createHeader( DNS_HEADER *header) {
    if (header == NULL) {
        return false;
    }
    memset(header, 0, sizeof(DNS_HEADER));
    header->id = htons(header->id);
    header->flags = htons(0x0100);
    header->qdcount = htons(1);
    return true;
}
bool createQuestion(DNS_QUESTION *question, DNS_QUERY *query) {
    if(logLevel >= 3)printf("%s\n", query->question->qname);
    if (question == NULL) {
        return false;
    }
    memset(question, 0, sizeof(*question));
    question->qname = (uint8_t *)malloc(strlen((char *)query->question->qname) + 2);
    memset(question->qname, 0, strlen((char *)query->question->qname) + 2);


    if(question->qname == NULL) {
        return false;
    }
    printf("name: %s\n", query->question->qname);
    encodeName(question->qname, query->question->qname);
    if(logLevel >= 3)printf("%s\n", query->question->qname);
    question->qtype = htons(query->question->qtype);
    question->qclass = htons(query->question->qclass);
    return true;
}

int createRequest(char *request, DNS_HEADER *header, DNS_QUESTION *question) {
    if (request == NULL || header == NULL || question == NULL) {
        return -1;
    }
    int len = sizeof(DNS_HEADER);
    memcpy(request, header, sizeof(DNS_HEADER));
    memcpy(request+len, question->qname, strlen((char *)question->qname) + 1);
    len += (int)strlen((char *)question->qname) + 1; /* etc.  .com /0 type class */
    memcpy(request+len, &question->qtype, sizeof (question->qtype));
    len += sizeof (question->qtype);
    memcpy(request+len, &question->qclass, sizeof (question->qclass));
    len += sizeof (question->qclass);
    return len;

}
int parseName(char *answer, int offset, char *name) {
    if (answer == NULL || name == NULL) {
        return -1;
    }
    int len = 0;
    int flag = 0;
    uint8_t *p = (uint8_t *)answer + offset;
    while(*p != 0) {
        flag = *p;
        if (flag == 0) break;
        if (flag == 0xC0) { /* judge if it is a pointer */
            int pointer = p[1];
            len = parseName(answer, pointer, name+len);
            return len;
        } else {
            memcpy(name + len, p + 1, flag);
            len += flag;
            name[len] = '.';
            len++;
            p = p + flag + 1;
        }

    }
    name[len - 1] = '\0';
    return len;
}

int parseResponse(char *response, DNS_HEADER *header, DNS_QUESTION *question, DNS_RR *answer) {
    if (response == NULL || header == NULL || question == NULL || answer == NULL) {
        return -1;
    }

    int cnt = 0;
    char answerName[256], cname[256];
    unsigned char ip[4];

    DNS_RR *rr = malloc(sizeof(DNS_RR));
    answer->next = (struct dns_rr*) rr;

    memcpy(header, response, sizeof(DNS_HEADER));
    ntohDnsHeader(header);
    int len = sizeof(DNS_HEADER);
    if(logLevel >= 2)printDnsHeader(header);
    char name[256];
    parseName(response, sizeof(DNS_HEADER), name);
    question->qname = (uint8_t *)malloc(strlen(name) + 1);
    memcpy(question->qname, name, strlen(name) + 1);
    len += (int)strlen((char *)question->qname) + 2; /* etc.  .com /0 type class */
    memcpy(&question->qtype, response+len, sizeof (question->qclass));
    len += sizeof (question->qtype);
    memcpy(&question->qclass, response+len, sizeof (question->qclass));
    len += sizeof (question->qclass);
    ntohDnsQuestion(question);
    if(logLevel >= 2)printDnsQuestion(question);
    int ancount = (header->ancount);
    int nscount = (header->nscount);
    int arcount = (header->arcount);
    //todo nscount here
    for (int i = 0; i < ancount + nscount + arcount ; i++) {
        parseName(response, len, answerName);
        unsigned char ptr[2] = {0};
        int ptrFlag = 0;
        if((unsigned char)(response+len)[0]>>4 == 0xc){ /* case PTR */
            ptr[0] = (unsigned char)(response+len)[0];
            ptr[1] = (unsigned char)(response+len)[1];
            ptrFlag = 1;
            len += 2;
        } else if((unsigned char)(response+len)[0] == 0x0){ /* <Root> */
            len += 1;
        } else {
            len += (int)strlen(answerName)-1;
        }
        rr->name = (uint8_t *)malloc(strlen(answerName) + 1);
        memcpy(rr->name, answerName, strlen(answerName) + 1);
        memcpy(&rr->type, response+len, sizeof (rr->type));
        len += sizeof (rr->type);
        memcpy(&rr->class, response+len, sizeof (rr->class));
        len += sizeof (rr->class);
        memcpy(&rr->ttl, response+len, sizeof (rr->ttl));
        len += sizeof (rr->ttl);
        memcpy(&rr->rdlength, response+len, sizeof (rr->rdlength));
        len += sizeof (rr->rdlength);
        ntohDnsRR(rr);
        rr->rdata = (uint8_t *)malloc(rr->rdlength);
        if(ptrFlag) {
            free(rr->name);
            rr->name = (unsigned char*) malloc(2);
            memcpy(rr->name, ptr, 2);
        }
        if(rr->type == 0x05) { /* CNAME */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x01) { /* A */
            memcpy(ip, response+len, rr->rdlength);
            len += rr->rdlength;
            if(logLevel >= 3)printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            rr->rdata = (uint8_t *)malloc(strlen(ip) + 1);
            rr->rdata[4] = '\0';
            memcpy(rr->rdata, ip, strlen(ip) + 1);
            cnt ++;
        } else if(rr->type == 0x1C) { /* AAAA */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x06) { /* SOA */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x0F) { /* MX */
            memcpy(&rr->rdata[strlen(answerName) + 1], response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x0C) { /* PTR */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x0A) { /* NS */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        } else if(rr->type == 0x02) { /* NS */
            memcpy(rr->rdata, response+len, rr->rdlength);
            len += rr->rdlength;
        }
        if(logLevel >= 2)printDnsRR(rr);
        if(i < ancount + nscount + arcount - 1)rr->next = (struct dns_rr *)malloc(sizeof(DNS_RR)), rr = rr->next;
        
    }
    rr->next = NULL;
    return cnt;

}

void htonDnsHeader(DNS_HEADER *header) {
    header->id = htons(header->id);
    header->flags = htons(header->flags);
    header->qdcount = htons(header->qdcount);
    header->ancount = htons(header->ancount);
    header->nscount = htons(header->nscount);
    header->arcount = htons(header->arcount);
}

void htonDnsQuestion(DNS_QUESTION *question) {
    question->qtype = htons(question->qtype);
    question->qclass = htons(question->qclass);
}

void htonDnsRR(DNS_RR *rr) {
    rr->type = htons(rr->type);
    rr->class = htons(rr->class);
    rr->ttl = htonl(rr->ttl);
    rr->rdlength = htons(rr->rdlength);
    switch (rr->type) {
        case 0x05:
            break;
        case 0x01:
            for(int i = 0; i < 4; i++) {
                ;
            }
            break;
        default:
            break;

    }
}

int createAnswer(char *buf, DNS_RR *rr) {
    int len = 0;
    while(rr != NULL){
        htonDnsRR(rr);
        if(rr->name[0] == 0xc0) { //if name is ptr
            memcpy(buf+len, rr->name, 2);
            len += 2;
        } else {
            char *newName = (char *)malloc(strlen((char *)rr->name) + 2);
            encodeName(newName, (char *)rr->name);
            unsigned int nameLen = strlen(newName);
            memcpy(buf + len, newName, nameLen);
            len += nameLen;
        }
        memcpy(buf + len, &rr->type, 2);
        len += 2;
        memcpy(buf + len, &rr->class, 2);
        len += 2;
        memcpy(buf + len, &rr->ttl, 4);
        len += 4;
        memcpy(buf + len, &rr->rdlength, 2);
        len += 2;
        memcpy(buf + len, rr->rdata, ntohs(rr->rdlength));
        len += ntohs(rr->rdlength);
        rr = rr->next;
    }

    return len;
}


/*int isPtr(){}*/
/*
int dnsQuery(DNS_QUERY *query, Cache *cache, DNS_HEADER **ansHead, DNS_RR **ansRR) {
    unsigned char ip[4] = {0};
    char* name = query->question->qname;
    if(queryCache(cache, name, ip)) {
        //found in cache
        printf("found in cache\n");
        printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        return 1;
    } else {
        //not found in cache
        printf("not found in cache\n");
        char buf[BUFSIZE] = {0};

        DNS_HEADER header = {0};
        createHeader(&header);
        DNS_QUESTION question = {0};
        printf("%d\n", strlen(name));
        createQuestion(&question, query);
        int reqLen = createRequest(buf, &header, &question);
        if(reqLen < 0) {
            return -1;
        }
        struct sigaction alarmact;

        bzero(&alarmact,sizeof(alarmact));
        alarmact.sa_handler = sig_alarm;
        alarmact.sa_flags = SA_NODEFER;

        sigaction(SIGALRM,&alarmact,NULL);
        int nbytes = sendto(sockfd, buf, reqLen, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));
        if (nbytes < 0) {
            perror("sendto");
            return -1;
        }

        while(true) {
            struct sockaddr addr;
            memset(&addr, 0, sizeof(addr));
            socklen_t addrlen = sizeof(addr);
            char recvbuf[BUFSIZE] = {0};
            alarm(2);
            if (recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, &addr, &addrlen) < 0) {
                //perror("recvfrom error");
                if(errno == EINTR) {
                    printf("Response longer than 2 seconds\n");
                    close(sockfd);
                    return -1;
                } else {
                    printf("recvfrom error\n");
                    close(sockfd);
                    return -1;
                }
            }
            alarm(0);
            DNS_HEADER *recvHeader = (DNS_HEADER *) malloc(sizeof(DNS_HEADER));
            DNS_QUESTION *recvQuestion = (DNS_QUESTION *) malloc(sizeof(DNS_QUESTION));
            DNS_RR *recvAnswer = (DNS_RR *) malloc(sizeof(DNS_RR)); /* RR linklist head
            parseResponse(recvbuf, recvHeader, recvQuestion, recvAnswer);
            //recvAnswer = (recvAnswer->next);
            DNS_RR *linklist = recvAnswer->next;
            while(linklist != NULL) {

                if(linklist->type == 0x01 ) {
                    //If the answer is an A record, we can get the IP address updated in cache
                    printf("%d.%d.%d.%d\n", linklist->rdata[0], linklist->rdata[1], linklist->rdata[2], linklist->rdata[3]);
                    //adding to cache
                    struct dns_map *map = (struct dns_map *)malloc(sizeof(struct dns_map));
                    map->name = (uint8_t *)malloc(strlen(name) + 1);
                    memcpy(map->name, name, strlen(name) + 1);
                    memcpy(map->ip, linklist->rdata, 4);
                    insertCache(cache, map->name, map->ip);
                    printCache(cache);
                }
                linklist = (linklist->next);
                if(linklist != NULL) {

                }
            }
            //ans = NULL;
            *ansHead = malloc(sizeof(*recvHeader));
            *ansRR = malloc(sizeof(*recvAnswer->next));
            memcpy(*ansHead, recvHeader, sizeof(*recvHeader));
            memcpy(*ansRR, recvAnswer->next, sizeof(*recvAnswer->next));
            free(recvHeader);
            free(recvQuestion);
            //free(recvAnswer);
            break;
        }

    }
    return 2; //not in cache, forwarding to client
}*/

int constructCacheAnswer(unsigned char *ip, DNS_RR **answer, DNS_HEADER *header) {
    header->flags = 0x8180;
    header->ancount = 1;
    if((*answer = (DNS_RR*)malloc(sizeof(DNS_RR)))==NULL) {
        perror("malloc");
        return -1;
    }
    (*answer)->name = (uint8_t *)malloc(2); //using pointer to point to the name
    (*answer)->name[0] = 0xc0;
    (*answer)->name[1] = 0x0c;
    (*answer)->type = 1;
    (*answer)->class = 1;
    (*answer)->ttl = 120;
    (*answer)->rdlength = 4;
    (*answer)->rdata = (uint8_t *)malloc(4);
    memcpy((*answer)->rdata, ip, 4);
    (*answer)->next = NULL;
    return 1;
}

int encodeName(unsigned char *qname, char *name) {
    const char delim[2] = ".";
    char *nameCopy = strdup(name);
    char *token = strtok(nameCopy, delim);
    while(token != NULL) {
        size_t len = strlen(token);
        qname[0] = len;
        memcpy(qname + 1, token, len);
        qname += len + 1;
        token = strtok(NULL, delim);
    }

    return 1;
}

