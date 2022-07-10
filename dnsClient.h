/** @file */
//
// Created by Go2Heart on 2022/7/9.
//

#ifndef DNSRELAY_DNSCLIENT_H
#define DNSRELAY_DNSCLIENT_H
#include "dnsResolver.h"


/**
 * @details 初始化dns client
 * @return sockfd: 初始化成功后的socket描述符
 */
int initDnsClient();

/**
 * @details 当客户端收到本地用户报文时，调用该函数
 * @param cache
 * @return error code
 */
int clientRecv(Cache *cache);

#endif //DNSRELAY_DNSCLIENT_H
