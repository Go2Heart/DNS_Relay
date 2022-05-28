//
// Created by Go2Heart on 2022/5/26.
//

#include "dnsResolver.h"


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
/*int isPtr(){}*/

bool sendDnsMessage(const char *name, const char *server, const int port, char *buf, int bufSize) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return false;
    }
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(server);
    if (addr.sin_addr.s_addr == INADDR_NONE) {
        struct hostent *host = gethostbyname(server);
        if (host == NULL) {
            perror("gethostbyname");
            return false;
        }
        addr.sin_addr = *(struct in_addr *) host->h_addr;
    }
    DNS_HEADER *header = (DNS_HEADER *) buf;
    header->id = htons(getpid());
    header->flags = htons(0x0100);
    header->qdcount = htons(1);
    header->ancount = 0;
    header->nscount = 0;
    header->arcount = 0;
    DNS_QUESTION *question = (DNS_QUESTION *) (malloc(sizeof(DNS_QUESTION)));
    question->qname = (uint8_t *) name;
    question->qtype = htons(1);
    question->qclass = htons(1);
    printf("%lu\n", sizeof(DNS_HEADER));
    buf[16-4] = 5;
    buf[17-4] = 'b';
    buf[18-4] = 'a';
    buf[19-4] = 'i';
    buf[20-4] = 'd';
    buf[21-4] = 'u';
    buf[22-4] = 3;
    buf[23-4] = 'c';
    buf[24-4] = 'o';
    buf[25-4] = 'm';
    buf[26-4] = 0;
    buf[sizeof(DNS_HEADER) + strlen(name) +2] = 0;
    buf[sizeof(DNS_HEADER) + strlen(name) + 3] = 1;
    buf[sizeof(DNS_HEADER) + strlen(name) + 4] = 0;
    buf[sizeof(DNS_HEADER) + strlen(name) + 5] = 1;
    /* DNS_QUESTION *question = (DNS_QUESTION *) (buf + sizeof(DNS_HEADER));
    question->qname = (uint8_t *) malloc(strlen(name));
    question->qtype = htons(1);
    question->qclass = htons(1);
    strcpy((char *) question->qname, name);
    printf("%lu\n", sizeof(*question)); */
    socklen_t addrlen = sizeof(addr);
    int len = sizeof(DNS_HEADER) + sizeof(DNS_QUESTION);
    int ret = sendto(fd, buf, 27, 0, (struct sockaddr *) &addr, addrlen);
    if (ret < 0) {
        perror("sendto");
        return false;
    }
    printf("send dns message: %s\n", buf);
    unsigned char recvbuf[1024] = {0};
    struct sockaddr_in from;
    ret = recvfrom(fd, (char*)recvbuf, bufSize, 0,(struct sockaddr *) &from, &addrlen);
    if (ret < 0) {
        perror("recvfrom");
        return false;
    }
    recvbuf[ret] = '\0';
    printf("receive dns message: %s\n", recvbuf);

    close(fd);
    return true;
}
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
    }


}