#include <stdio.h>
#include <stdlib.h>
#include <winsock.h>

int main(void) {
    
    
    char target[100];
    scanf("%s", target);
    target[strlen(targe+t)] = '.';
    WSADATA wsaData;
    WSAStartup (0x101, (LPWSADATA) &wsaData);
    LPHOSTENT hp;
    struct in_addr sAddr;

    int i;
    for(i = 1; i < 255; ++i) {
        char numberString[100];
        itoa(i, numberString, 10);
        char ip[200];
        strcpy(ip, target);
        strcat(ip, numberString);
        sAddr.s_addr = inet_addr(ip);
        hp = gethostbyaddr((LPSTR) &sAddr, sizeof(sAddr), AF_INET);

        if(hp) {
            printf("%s, %s\n", ip,hp->h_name);
        }
    }
    system("pause");
    return 0;
}