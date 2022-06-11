//
// Created by Go2Heart on 2022/5/22.
//
//TODO adding cache and taking care of the cache
#include "trie.h"
#include "cache.h"
#include "table.h"
#include "dnsResolver.h"

int main() {
    //read local static table
    Trie* staticTable = loadTable("../localTable.txt");
    Cache *cache = (Cache*)malloc(sizeof(Cache));
    if (initCache(cache) == false) {
        printf("init cache failed\n");
        return -1;
    }
    //printTrie(staticTable, 30);
    char* ip = "";
    char name[256];
    printf("input the name you want to query: ");
    scanf("%s", name);
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
        printf("input the name you want to query: ");
        scanf("%s", name);
        //todo fix cache and linklist adding problem
    }

    return 0;
}
