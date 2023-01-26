#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <unistd.h>

#define PORT 8000
#define CLIENT_TERMINATION "0"
#define SERVER_TERMINATION "-1"

struct sockaddr_in server_addr, client_addr;
int server_sock, client_sock, sock_addr_len = sizeof(client_addr);
char buffer[1024];
pid_t childpid;

// Function declration
void closeSockets(void);

int main() {

	// Creating the socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(server_sock < 0) {
        printf("[-] Socket creation failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[+] Socket creation successful.\n");
    }

	// Socket initialization
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Binding the socket
    if(bind(server_sock, (const struct sockaddr*) &server_addr, sock_addr_len) < 0) {
        printf("[-] Binding socket failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[+] Binding socket successful.\n");
    }

    // Listening for connections
    if(listen(server_sock, 5) < 0) {
        printf("[-] Listening on socket failed.\n");
        exit(EXIT_FAILURE);
    } else {
        printf("[+] Listening on socket successful.\n");
    }

    while (1) 
    {
        // Accepting connection
        client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &sock_addr_len);
        if(client_sock < 0) {
            exit(1);
        } else 
        {
            printf("[ %s:%d ] Connection accepted.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
            // Signal for server termination from client
            recv(client_sock, (void *) &buffer, sizeof(buffer), 0);
            if (strcmp(buffer, SERVER_TERMINATION) == 0) {
                printf("[ %s:%d ] Termination Request Received.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                strcpy(buffer, "Server Termination Affirmed.");
                send(client_sock, (const void *) buffer, sizeof(buffer), 0);
                printf("[+] Exiting...\n");
                closeSockets();
            } else {
                printf("[ %s:%d ] %s\n", inet_ntoa(client_addr.sin_addr), htons(client_addr.sin_port), buffer);
                strcpy(buffer, "Echo from Server.");
                send(client_sock, (const void *) &buffer, sizeof(buffer), 0);
            }
        }
        if ( (childpid = fork()) == 0 ) {
            close(server_sock);
            while (1)
            {
                // Waiting for msg from clients
                recv(client_sock, (void *) &buffer, sizeof(buffer), 0);
                // Server termination signal
                if (strcmp(buffer, "0") == 0) {
                    printf("[ %s:%d ] Disconnected.\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));
                    closeSockets();
                } else {
                    printf("[ %s:%d | Received Msg ]: %s\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port), buffer);
                    // Sending Ack Reply to client
                    strcpy(buffer, "Msg Received");
                    send(client_sock, (const void *) buffer, sizeof(buffer), 0);
                }
            }
            
        }
    }
        
        
    // closeSockets();
    return 0;
}

void closeSockets(void) {
    close(client_sock);
    close(server_sock);
    printf("[+] Sockets closed.\n");
    fflush(stdin);
    fflush(stdout);
    exit(EXIT_SUCCESS);
}