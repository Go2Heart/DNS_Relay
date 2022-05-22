/**
 * @file udpClient.cpp      
 * @author Go2Heart (go2heart@outlook.com)
 * @brief 
 * @version 0.1
 * @date 2022-04-23
 *  * 
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#ifdef _WIN32
    #include <winsock2.h>
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
#endif

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("Usage: udpServer <address> <port>\n");
        printf("e.g. udpServer 127.0.0.1:1234\n");
        return 1;
    }

    char* address = argv[1];
    int port = atoi(argv[2]);

    const int delay_secs = 1;
    const char *message = "Hello World!";

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

    /**
     * @brief create the UDP socket
     * 
     */
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }


    /**
     * @brief bind the socket to the address and port
     * 
     */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);

    while(true) {
        char ch = 0;
        int nbytes = sendto(fd, message, strlen(message), 0, (struct sockaddr *)&addr, sizeof(addr));
        if (nbytes < 0) {
            perror("sendto");
            return 1;
        }
    #ifdef _WIN32
        Sleep(delay_secs * 1000);
    #else
        sleep(delay_secs);
    #endif
    }

#ifdef _WIN32
    WSACleanup();
#endif
    
    return 0;
}