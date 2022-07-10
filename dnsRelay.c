/** @file */
//
// Created by Go2Heart on 2022/5/22.
//
#include "table.h"
#include "dnsServer.h"
#include "dnsClient.h"
int serverFd;
int clientFd;
int logLevel = 0;
char* logName;
FILE* logFile;
/**
 * @callgraph
 * @details 初始化DNS Relay
 * @return
 */
int main() {
#ifdef _WIN32
    WSADATA wsaData;
    if (WSAStartup(0x0101, &wsaData)) {
        perror("WSAStartup");
        return 1;
    }
#endif
    logName = "dnsRelay.log";
    printf("-----------------------------------------------------------------------\n"
           "██████╗ ███╗   ██╗███████╗    ██████╗ ███████╗██╗      █████╗ ██╗   ██╗\n"
           "██╔══██╗████╗  ██║██╔════╝    ██╔══██╗██╔════╝██║     ██╔══██╗╚██╗ ██╔╝\n"
           "██║  ██║██╔██╗ ██║███████╗    ██████╔╝█████╗  ██║     ███████║ ╚████╔╝ \n"
           "██║  ██║██║╚██╗██║╚════██║    ██╔══██╗██╔══╝  ██║     ██╔══██║  ╚██╔╝  \n"
           "██████╔╝██║ ╚████║███████║    ██║  ██║███████╗███████╗██║  ██║   ██║   \n"
           "╚═════╝ ╚═╝  ╚═══╝╚══════╝    ╚═╝  ╚═╝╚══════╝╚══════╝╚═╝  ╚═╝   ╚═╝   \n"
           "-----------------------------------------------------------------------\n");
    printf("Welcome to DNS Relay!\n");
    printf("The log level: 1.none 2.info 3.debug\n");
    scanf("%d", &logLevel);
    if (logLevel < 1 || logLevel > 3) {
        printf("The log level is invalid, set to default value: 1\n");
        logLevel = 1;
    }
    printf("The log level is set to %d\n", logLevel);
    printf("The log filename is %s\n", logName);
    logFile = fopen(logName, "w");


    //read local static table
    Trie* staticTable = loadTable("../localTable.txt");
    Cache *cache = (Cache*)malloc(sizeof(Cache));
    if (initCache(cache) == false) {
        printf("init cache failed\n");
        return -1;
    }
    serverFd=initDnsServer(53);
    clientFd=initDnsClient();
    fd_set fdSet;
    struct timeval timeout = {0, 10}; /* 50ms */
    while(1) {
        FD_ZERO(&fdSet); /* 清空fdSet */
        FD_SET(serverFd, &fdSet); /* 把serverFd加入到fdSet */
        FD_SET(clientFd, &fdSet); /* 把clientFd加入到fdSet */

        int ret = select(MAX_HOST, &fdSet, NULL, NULL, &timeout); /* 等待某个文件描述符就绪 */
        if (ret == -1) { /* select失败 */
            perror("select");
            return -1;
        } else if (ret == 0) { /* select超时 */
            continue;
        } else {
            if (FD_ISSET(serverFd, &fdSet)) { /* serverFd就绪 */
                serverRecv(serverFd, cache, staticTable);
            }
            if (FD_ISSET(clientFd, &fdSet)) { /* clientFd就绪 */
                clientRecv(cache);
            }
        }
    }
}