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
    printf("input the name you want to query: ");
    DNS_QUERY* temp = receiveDnsQuery(serverFd);
    memcpy(name, temp->question->qname, strlen(temp->question->qname));
    printf("%d\n", strlen(temp->question->qname));
    printf("%s\n", name);
    while(true) {
        //query the static table
        Trie *result = searchTrie(staticTable, name);
        if (result != NULL) {
            printf("found record in static table\n");
            ip = result->ip;
            printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
        } else {
            printf("not found in static table\n");
            if(dnsQuery(name, cache)){
                ip = cache->list->head->ip; //use cache to get ip
                printf("ip: %d.%d.%d.%d\n", ip[0], ip[1], ip[2], ip[3]);
            } else {
                printf("query failed\n");
            }
        }
        //send the answer
        DNS_RR *answer = (DNS_RR*)malloc(sizeof(DNS_RR));
        printf("%d\n", sizeof(DNS_RR));
        answer->name = (uint8_t *)malloc(2); //using pointer to point to the name
        answer->name[0] = 0xc0;
        answer->name[1] = 0x0c;
        answer->type = 1;
        answer->class = 1;
        answer->ttl = 120;
        answer->rdlength = 4;
        answer->rdata = (uint8_t *)malloc(4);
        memcpy(answer->rdata, ip, 4);
        replyDnsQuery(serverFd, temp, answer);
        memset(name, 0, sizeof(name));
        DNS_QUERY* temp = receiveDnsQuery(serverFd);
        memcpy(name, temp->question->qname, strlen(temp->question->qname));
    }
    /*todo scanf("%s", name);
    while(strcmp(name, "exit") != 0) {
        //query the static table
        Trie *result = searchTrie(staticTable, name);
        if (result != NULL) {
            printf("found record in static table\n");
            printf("ip: %d.%d.%d.%d\n", result->ip[0], result->ip[1], result->ip[2], result->ip[3]);
        } else {
            printf("not found in static table\n");
            printf("using dns server to query\n");
            //query dns server
            dnsQuery(name, cache);
        }
        temp = receiveDnsQuery(serverFd);
        memcpy(name, temp->qname, strlen((char*)temp->qname));
        //todo printf("input the name you want to query: ");
        //todo scanf("%s", name);
        //TODO reply the dns query
    }*/

    return 0;
}
