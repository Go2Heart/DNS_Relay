//
// Created by Go2Heart on 2022/5/22.
//
#include "dnsResolver.h"

int main() {
    char name[256] = {0};
    printf("Please input the domain name(input exit to stop):\n");
    scanf("%s", name);
    dnsQuery(name);
    return 0;
}
