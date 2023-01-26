#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define PORT 8000
#define SERVER_ADDR "127.0.0.1"
#define CLIENT_TERMINATION "0"
#define SERVER_TERMINATION "-1"

struct sockaddr_in server_addr;
int server_sock, sock_addr_len = sizeof(server_addr);
char buffer[1024];

// Function declarations
void closeSocket(void);
char *getMsg(char *msg);
void closeClient();
void closeServer();

int main(int argc, char const *argv[]) {

	// Creating the socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        printf("[-] Socket creation failed.\n") ;
        exit(EXIT_FAILURE);
    } else {
        printf("[+] Socket created successful.\n");
    }

	// Socket initialization
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = inet_addr(SERVER_ADDR);

    // Connecting to the server
    if (argc == 1) 
    {
        if (connect(server_sock, (struct sockaddr*)&server_addr, sock_addr_len) < 0)
        {
            printf("[-] Socket connection failed\n");
            exit(EXIT_FAILURE);
        }
        printf("[+] Connected with socket successfully\n");
        strcpy(buffer, "Echo from client.");
        send(server_sock, (const void *) &buffer, sizeof(buffer), 0);
        recv(server_sock, (void *) &buffer, sizeof(buffer), 0);
        printf("[ %s:%d ] %s\n", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port), buffer);
    } else if (strcmp(argv[1], "-close") == 0)
    {
        if (connect(server_sock, (struct sockaddr*)&server_addr, sock_addr_len) < 0)
        {
            printf("[-] Socket connection failed\n");
            exit(EXIT_FAILURE);
        }
        printf("[+] Termination socket Connected successfully\n");
        closeServer();
    } else {
        printf("[-] Invalid argument\n");
        printf("[#] Usage: \n\t./client\n");
        printf("\t./client -close\n");

        exit(EXIT_FAILURE);
    }

    // Main Loop
    while (1) {
        getMsg(buffer);
        send(server_sock, (const void *) &buffer, sizeof(buffer), 0);
        recv(server_sock, (void *) &buffer, sizeof(buffer), 0);
        printf("[ %s:%d | %s ]\n", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port), buffer);
        // closeSocket();
    }
    closeSocket();
    return 0;
}

void closeSocket(void) {
    close(server_sock);
    printf("[+] Socket closed.\n");
    exit(EXIT_SUCCESS);
}

char *getMsg(char *msg) {
    printf("[ %s:%d | Enter the msg ]: ", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port));
    scanf("%s", msg);
    if (strcmp(msg, CLIENT_TERMINATION) == 0) {
        closeClient();
    }
    return msg;
}

void closeClient() {
    strcpy(buffer, CLIENT_TERMINATION);
    send(server_sock, (const void *) &buffer, sizeof(buffer), 0);
    printf("[#] Disconnection from server successful.\n");
    closeSocket();
}

void closeServer() {
    strcpy(buffer, SERVER_TERMINATION);
    send(server_sock, (const void *) &buffer, sizeof(buffer), 0);
    printf("[!] Termination Request sent, waiting for reply... \n");
    recv(server_sock, (void *) &buffer, sizeof(buffer), 0);
    printf("[ %s:%d ] %s\n", inet_ntoa(server_addr.sin_addr), htons(server_addr.sin_port), buffer);
    printf("[#] Exiting...\n");
    closeSocket();
}