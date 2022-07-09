/** @file */
//
// Created by Go2Heart on 2022/7/9.
//

#ifndef DNSRELAY_DNSCLIENT_H
#define DNSRELAY_DNSCLIENT_H
#include "dnsResolver.h"


/**
 * @details init dns client
 * @return
 */
int initDnsClient();

/**
 * @details when dns client receive a packet, it will call this function to process it
 * @param cache
 * @return
 */
int clientRecv(Cache *cache);

#endif //DNSRELAY_DNSCLIENT_H
