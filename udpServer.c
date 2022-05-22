/**
 * @file udpServer.cpp  
 * @author Go2Heart (go2heart@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-23
 * 
 * 
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#ifdef _WIN32
    #include <Winsock2.h>
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
    #include <time.h>
#endif

#define MSGSIZE 256

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: udpClient <address> <port>\n");
        printf("e.g. udpClient 127.0.0.1 1234");
        return 1;
    }

    char* address = argv[1];
    int port = atoi(argv[2]);

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

    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }

    /* Handle multiple sockets */
    u_int yes = 1;
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, (char*) &yes, sizeof(yes)) < 0) {
        perror("Reuse address failed");
        return 1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(port);

    if (bind(fd, (struct sockaddr*) &addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    //Sstruct ip_mreq mreq;
    //Smreq.imr_multiaddr.s_addr = inet_addr(address);
    //Smreq.imr_interface.s_addr = htonl(INADDR_ANY);
    //Ssize_t len = sizeof(mreq);
    //Sif (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &mreq, len) < 0) {
    //S    perror("setsockopt");
    //S    return 1;
    //S}

    while(true) {
        char msgbuf[MSGSIZE];
        socklen_t addrlen = sizeof(addr);
        int nbytes = recvfrom(fd, msgbuf, MSGSIZE, 0, (struct sockaddr *)&addr, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        }
        msgbuf[nbytes] = '\0';

        printf("%s\n", msgbuf);
    }

    #ifdef _WIN32
        WSACleanup();
    #endif
     return 0;
}
