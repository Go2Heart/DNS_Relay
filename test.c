#include <stdio.h>
#include <stdlib.h>

int main(){
    system("pwd");
    system("cd bin\n""./cmuServer 1234");
    int n;
    scanf("%d", &n);
    for(int i = 0; i < n; i++){
        system("cd bin\n""./cmuClient 127.0.0.1 1234");
        system("test __i");
    }
    
    return 0;
}