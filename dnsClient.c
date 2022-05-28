//
// Created by Go2Heart on 2022/5/22.
//

#include "dnsResolver.h"
int main() {

    /*
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(address);
     */

    char name[256] = {0};
    printf("Please input the domain name(input exit to stop):\n");
    scanf("%s", name);
    dnsQuery(name);
    return 0;
}
