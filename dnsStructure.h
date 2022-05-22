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

#include <stdint.h>


/**
 * @brief the structure of a dns packet header
 * 
 */

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
    uint8_t* qname; /* linked list of labels */
    uint16_t qtype;
    uint16_t qclass;
    struct dns_question* next;
} DNS_QUESTION;

/**
 * @brief 
 * 
 */
typedef struct {
    uint8_t* name; /* linked list of labels */
    uint16_t type;
    uint16_t _class;
    uint32_t ttl;
    uint16_t rdlength;
    uint8_t* rdata;
    struct dns_rr* next;
} DNS_RR;

