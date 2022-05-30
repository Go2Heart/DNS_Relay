//
// Created by Go2Heart on 2022/5/26.
//

#include "dnsResolver.h"

extern void printDnsHeader(DNS_HEADER *header) {
    printf("===== DNS Header =====\n");
    printf("id: 0x%x\n", (header->id));
    printf("flags: 0x%x\n", (header->flags));
    printf("qdcount: %d\n", (header->qdcount));
    printf("ancount: %d\n", (header->ancount));
    printf("nscount: %d\n", (header->nscount));
    printf("arcount: %d\n", (header->arcount));
    printf("=====================\n");
}

extern void printDnsQuestion(DNS_QUESTION *question) {
    printf("===== DNS Question =====\n");
    printf("qname: %s\n", question->qname);
    printf("qtype: %d\n", (question->qtype));
    printf("qclass: %d\n", (question->qclass));
    printf("=====================\n");
}

extern void printDnsRR(DNS_RR *rr) {
    printf("===== DNS RR =====\n");
    printf("name: %s\n", rr->name);
    printf("type: %d\n", (rr->type));
    printf("class: %d\n", (rr->class));
    printf("ttl: %d\n", (rr->ttl));
    printf("rdlength: %d\n", (rr->rdlength));
    printf("rdata: %s\n", rr->rdata);
    printf("=====================\n");
}
extern void ntohDnsHeader(DNS_HEADER *header) {
    header->id = ntohs(header->id);
    header->flags = ntohs(header->flags);
    header->qdcount = ntohs(header->qdcount);
    header->ancount = ntohs(header->ancount);
    header->nscount = ntohs(header->nscount);
    header->arcount = ntohs(header->arcount);
}
extern void ntohDnsQuestion(DNS_QUESTION *question) {
    question->qtype = ntohs(question->qtype);
    question->qclass = ntohs(question->qclass);
}
extern void ntohDnsRR(DNS_RR *rr) {
    rr->type = ntohs(rr->type);
    rr->class = ntohs(rr->class);
    rr->ttl = ntohl(rr->ttl);
    rr->rdlength = ntohs(rr->rdlength);
}
extern bool createHeader( DNS_HEADER *header) {
    if (header == NULL) {
        return false;
    }
    memset(header, 0, sizeof(DNS_HEADER));
    header->id = htons(getpid());
    header->flags = htons(0x0100);
    header->qdcount = htons(1);
    return true;
}
bool createQuestion( DNS_QUESTION *question, const char *name) {
    if (question == NULL || name == NULL) {
        return false;
    }
    memset(question, 0, sizeof(*question));
    question->qname = (uint8_t *)malloc(strlen(name) + 2);
    if(question->qname == NULL) {
        return false;
    }
    const char delim[2] = ".";
    char *nameCopy = strdup(name);
    char *token = strtok(nameCopy, delim);
    uint8_t *qname = question->qname;
    question->qtype = htons(1);
    question->qclass = htons(1);
    while(token != NULL) {
        size_t len = strlen(token);

        qname[0] = len;
        memcpy(qname + 1, token, len);
        qname += len + 1;
        token = strtok(NULL, delim);
    }

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
    DNS_RR *rr = answer;

    memcpy(header, response, sizeof(DNS_HEADER));
    ntohDnsHeader(header);
    int len = sizeof(DNS_HEADER);
    printDnsHeader(header);
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
    printDnsQuestion(question);
    int ancount = (header->ancount);

    for (int i = 0; i < ancount; i++) {
        parseName(response, len, answerName);
        rr->name = (uint8_t *)malloc(strlen(answerName) + 1);
        memcpy(rr->name, answerName, strlen(answerName) + 1);
        len += 2;
        memcpy(&rr->type, response+len, sizeof (rr->type));
        //rr->type = ntohs(rr->type);
        len += sizeof (rr->type);
        memcpy(&rr->class, response+len, sizeof (rr->class));
        //rr->class = ntohs(rr->class);
        len += sizeof (rr->class);
        memcpy(&rr->ttl, response+len, sizeof (rr->ttl));
        //rr->ttl = ntohl(rr->ttl);
        len += sizeof (rr->ttl);
        memcpy(&rr->rdlength, response+len, sizeof (rr->rdlength));
        //rr->rdlength = ntohs(rr->rdlength);
        len += sizeof (rr->rdlength);
        ntohDnsRR(rr);
        rr->rdata = (uint8_t *)malloc(rr->rdlength);
        if(rr->type == 0x05) { /* CNAME */
            parseName(response, len, cname);
            rr->rdata = (uint8_t *)malloc(strlen(cname) + 1);
            memcpy(rr->rdata, cname, strlen(cname) + 1);
            len += rr->rdlength;
        } else if(rr->type == 0x01) { /* A */
            memcpy(ip, response+len, rr->rdlength);
            len += rr->rdlength;
            printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            rr->rdata = (uint8_t *)malloc(strlen(ip) + 1);
            memcpy(rr->rdata, ip, strlen(ip) + 1);
            cnt ++;
        }
        if(i != ancount - 1)rr->next = (struct dns_rr *)malloc(sizeof(DNS_RR));
        printDnsRR(rr);
    }
    return cnt;

}
/*int isPtr(){}*/

extern int dnsQuery(const char *name)  {
    char buf[BUFSIZE] = {0};
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0) {
        perror("socket");
        exit(-1);
    }

    struct sockaddr_in serveraddr;
    memset(&serveraddr, 0, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_port = htons(DNS_SERVER_PORT);
    serveraddr.sin_addr.s_addr = inet_addr(DNS_SERVER_ADDR);
    DNS_HEADER header = {0};
    createHeader(&header);
    DNS_QUESTION question = {0};
    createQuestion(&question, name);
    int reqLen = createRequest(buf, &header, &question);
    if(reqLen < 0) {
        return -1;
    }
    int nbytes = sendto(sockfd, buf, reqLen, 0, (struct sockaddr *)&serveraddr, sizeof(serveraddr));
    if (nbytes < 0) {
        perror("sendto");
        return -1;
    }
    while(true) {
        struct sockaddr addr;
        socklen_t addrlen = sizeof(addr);
        char recvbuf[BUFSIZE] = {0};
        nbytes = recvfrom(sockfd, recvbuf, sizeof(recvbuf), 0, &addr, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return -1;
        }
        DNS_HEADER recvHeader = {0};
        DNS_QUESTION recvQuestion = {0};
        DNS_RR recvAnswer = {0};
        parseResponse(recvbuf, &recvHeader, &recvQuestion, &recvAnswer);
    }


}