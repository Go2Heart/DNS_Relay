/**
 * @file dnsStructure.h
 * @author Go2Heart (yanyibin@bupt.edu.cn)
 * @brief 
 * @version 0.1
 * @date 2022-04-24
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#ifdef _WIN32
#include <winsock2.h>
    #include <Ws2tcpip.h>
    #include <Windows.h>
#else // Linux
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
#include <errno.h>
#include <stdbool.h>

#endif
#define BUFSIZE 1024
#define DNS_SERVER_PORT 53
#define DNS_SERVER_ADDR "114.114.114.114"
/**
 * @brief the structure of a dns packet header
 * 
 */
typedef struct {
    char *name;
    char *ip;
} DNS_MAP;

typedef struct {
    uint16_t id;
    uint16_t flags;
    uint16_t qdcount;
    uint16_t ancount;
    uint16_t nscount;
    uint16_t arcount;
} DNS_HEADER;
/**
 * @brief the structure of a dns question
 * 
 */
typedef struct {
    uint8_t * qname;
    uint16_t qtype;
    uint16_t qclass;
} DNS_QUESTION;

/**
 * @brief 
 * 
 */
typedef struct {
    uint8_t* name; /* linked list of labels */
    uint16_t type;
    uint16_t class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rdata;
    struct dns_rr* next;
} DNS_RR;


