
//
// Created by Go2Heart on 2022/5/22.
//
//TODO adding cache and taking care of the cache
#include "trie.h"
#include "cache.h"
#include "table.h"
#include "dnsResolver.h"
#include "dnsServer.h"

int main() {
#ifdef _WIN32
    /**
     * @brief initialize Windows Wocket API
     * 
     */
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData)) {
        perror("WSAStartup");
        return 1;
    }

#endif
    //read local static table
    Trie* staticTable = loadTable("../localTable.txt");
    Cache *cache = (Cache*)malloc(sizeof(Cache));
    if (initCache(cache) == false) {
        printf("init cache failed\n");
        return -1;
    }
    //printTrie(staticTable, 30);
    int serverFd=initDnsServer(53);
    unsigned char* ip = "";
    char name[256] = {0};
    while(true) {
        memset(name, 0, sizeof(name));
        DNS_QUERY* pQuery = receiveDnsQuery(serverFd);
        memcpy(name, pQuery->question->qname, strlen(pQuery->question->qname));
        //query the static table
        Trie *result = searchTrie(staticTable, name);
        if (result != NULL) {
            printf("found record in static table\n");
            ip = result->ip;
            printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        } else {
            printf("not found in static table\n");
            DNS_HEADER *ansHead = NULL;
            DNS_RR *ansRR = NULL;
            int res = dnsQuery(pQuery, cache, &ansHead, &ansRR);
            if(res == 1){
                ip = cache->list->head->ip; //use cache to get ip
                printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            } else if(res == 2){
                printf("not in cache, using forwarder\n");
                int id = pQuery->header->id;
                memcpy(pQuery->header, ansHead, sizeof(DNS_HEADER));
                pQuery->header->id = id; //keep the same id
                replyDnsQuery(serverFd, pQuery, ansRR);
                continue;
            }
            else {
                printf("query failed\n");
                continue;
            }
        }
        if(ip[0] == 0 && ip[1] == 0 && ip[2] == 0 && ip[3] == 0) {
            printf("Blocked site queried!\n");
            replyDnsQuery(serverFd, pQuery, NULL);
            continue;
        }
        //send the answer
        DNS_RR *answer=NULL;
        if(constructCacheAnswer(ip, &answer, pQuery->header) == -1) {
            printf("construct answer failed\n");
            continue;
        }
        replyDnsQuery(serverFd, pQuery, answer);
    }
    return 0;
}
