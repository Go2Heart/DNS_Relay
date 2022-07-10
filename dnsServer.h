/** @file */
//
// Created by Go2Heart on 2022/6/29.
// @brief: used to answer local dns query

#ifndef DNSRELAY_DNSSERVER_H
#define DNSRELAY_DNSSERVER_H

#include "dnsResolver.h"


/**
 * @brief: 初始化dns server并且启动监听
 * @param: port: 监听端口
 * @return: 0: 成功, -1: 失败
 */
extern int initDnsServer(int port);

/**
 * @brief: 从用户客户端收到一个dns query，并且解析dns query，然后将解析结果发送给用户客户端
 * @param: sockfd: 用户客户端的socket描述符
 * @return: 1: 成功, -1: 失败
 */
extern int serverRecv(int sockfd, Cache *cache, Trie *staticTable);

/**
 * @brief 新id对象，用于生成新的id，并且保存原id对象及其对应的地址以及ttl
 */
typedef struct new_id {
    uint16_t id;
    int ttl;
    struct sockaddr_in addr;
}NEW_ID;
/**
 * @brief id转换表，表的下标为id，值为NEW_ID结构体指针
 */
extern NEW_ID idTable[65535];
/**
 * @details 检查id是否过期
 * @param id
 * @return 1: 过期, 0: 未过期
 */
bool isExpired(NEW_ID id);

/**
 * @details 将原id转换为新id
 * @param id
 * @param addr
 * @return 新id
 */
uint16_t getNewId(uint16_t id, struct sockaddr_in addr);


#endif //DNSRELAY_DNSSERVER_H
