#include <stdio.h>
#include <arpa/inet.h>

#define PORT 1
#define SERVER_ADDR "127.0.0.1"

int main() {
    struct sockaddr_in sa;
    inet_pton(AF_INET, SERVER_ADDR, &(sa.sin_addr));
    char *str = inet_ntoa(sa.sin_addr);
    printf("%u", sa.sin_addr.s_addr);
    printf("%s", str);
    // in_addr_t addr =  inet_addr(SERVER_ADDR);
    // char raddr[8] = inet_ntoa(addr);
    // printf("%u\n", raddr);
    return 0;
}