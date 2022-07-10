/** @file */
//
// Created by Go2Heart on 2022/5/26.
//

#ifndef DNSRELAY_DNSRESOLVER_H
#define DNSRELAY_DNSRESOLVER_H

#include "dnsStructure.h"
#include "cache.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>


/**
 * @details 打印DNS报文HEADER信息
 * @param header
 */
extern void printDnsHeader(DNS_HEADER *header);
/**
 * @details 打印DNS报文QUESTION信息
 * @param header
 */
extern void printDnsQuestion(DNS_QUESTION *question);
/**
 * @details 打印DNS报文Resource Record信息
 * @param header
 */
extern void printDnsRR(DNS_RR *rr);
/**
 * @details 将DNS报文HEADER转换为字符串
 * @param header
 */
extern void ntohDnsHeader(DNS_HEADER *header);
/**
 * @details 将DNS报文QUESTION转换为字符串
 * @param question
 */
extern void ntohDnsQuestion(DNS_QUESTION *question);
/**
 * @details 将DNS报文Resource Record转换为字符串
 * @param rr
 */
extern void ntohDnsRR(DNS_RR *rr);
/**
 * @details 将字符串转换为DNS报文HEADER
 * @param header
 */
extern void htonDnsHeader(DNS_HEADER *header);
/**
 * @details 将字符串转换为DNS报文QUESTION
 * @param question
 */
extern void htonDnsQuestion(DNS_QUESTION *question);
/**
 * @details 将字符串转换为DNS报文Resource Record
 * @param rr
 */
extern void htonDnsRR(DNS_RR *rr);

/** @brief DNS报文解析 */
/**
 * @details 解析DNS报文中的域名
 * @param buf
 * @param header
 * @param question
 * @return 报文的长度
 */
extern int parseName(char *answer, int offset, char *name);
/**
 * @details 解析DNS RESPONSE报文
 * @param response
 * @param header
 * @param question
 * @param answer
 * @return RR的数量
 */
extern int parseResponse(char *response, DNS_HEADER *header, DNS_QUESTION *question, DNS_RR *answer);\
/** @brief DNS报文解析结束 */


/** @brief DNS回复报文生成 */

/**
 * @details 创建DNS报文QUESTION
 * @param question
 * @return 是否创建成功
 */
extern bool createQuestion(DNS_QUESTION *question,  DNS_QUERY *query);
/**
 * @details 创建DNS报文Resource Record
 * @param rr
 * @return 报文的长度
 */
extern int createAnswer(char *buf, DNS_RR *rr);
/**
 * @details 从cache构建DNS报文
 * @param buf
 * @param header
 * @param question
 * @return 报文的长度
 */
extern int constructCacheAnswer(Ip *ip, DNS_RR **answer, DNS_HEADER *header);
/**
 * @details 创建DNS报文
 * @param buf
 * @param header
 * @param question
 * @return 是否创建成功
 */
extern int createRequest(char *request, DNS_HEADER *header, DNS_QUESTION *question);
/**
 * @details 将域名转化为DNS报文中的域名
 * @param qname
 * @param buf
 * @return 是否转化成功
 */
extern int encodeName(unsigned char *qname, char *buf);
/**
 * @details 回复DNS QUERY报文
 * @param sockfd
 * @param query
 * @param answer
 * @return
 */
int replyDnsQuery(int sockfd, DNS_QUERY *query, DNS_RR *answer);
/** @brief DNS回复报文生成结束 */


#endif //DNSRELAY_DNSRESOLVER_H
