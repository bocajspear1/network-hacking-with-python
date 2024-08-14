#ifdef __WIN32__
#include <winsock2.h>
#include <ws2tcpip.h>
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 500
#define SERVER_PORT 9999

uint16_t get_read_size(int client_fd) {
    uint16_t size = 0;
    int client_status = recv(client_fd, &size, sizeof(uint16_t), 0);

    if (client_status == 0) {
        printf("Client disconnected\n");
        return 0;
    } else if (client_status == -1) {
        printf("Error on client read!\n");
        return 0;
    } else {
        printf("Got size %d\n", size);
        return size;
    }
}

int read_data(int client_fd, char * buffer) {
    uint16_t size = get_read_size(client_fd);
    int client_status = recv(client_fd, buffer, size, 0);

    if (client_status == 0) {
        printf("Client disconnected\n");
        return 0;
    } else if (client_status == -1) {
        printf("Error on client read!\n");
        return 0;
    } else {
        printf("Read in %d bytes\n", size);
        return size;
    }
}

int main() {
    int server_fd;
    char buffer[BUFFER_SIZE];
    struct sockaddr_in server_addr;
    struct addrinfo data;
    int yes = 1;
    int status = 0;

    int client_fd;
    struct sockaddr_in client_addr;
    char client_ip[INET_ADDRSTRLEN] = {0};
    socklen_t client_addr_size;
    uint16_t size = 0;
    uint8_t small_resp = 0;
    
    int client_status = 0;

    #ifdef __WIN32__
    WSADATA wsaData = {0};
    printf("Running WSAStartup\n");
    int startup = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (startup != 0) {
        wprintf(L"WSAStartup failed: %d\n", startup);
        return 1;
    }
    #endif

    server_fd = socket(AF_INET, SOCK_STREAM, 6);

    if (server_fd == -1) {
        printf("Unable to create socket!\n");
        return 1;
    }

    status = setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    if (status == -1) {
        printf("Failed to set socket option\n");
        return 1;
    }

    memset(&server_addr, 0 , sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);
    char * local_addr = "127.0.0.1";
    status = inet_pton(AF_INET, local_addr, &server_addr.sin_addr);

    if (status != 1) {
        printf("Failed to set IP address\n");
        return 1;
    }

    status = bind(server_fd, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in));
    if (status == -1) {
        printf("Failed to bind\n");
        return 2;
    }

    status = listen(server_fd, 10);
    printf("Listening on port %d\n", SERVER_PORT);

    while (1) {
        memset(&client_addr, 0, sizeof(struct sockaddr_in));
        memset(buffer, 0, BUFFER_SIZE);
        client_addr_size = sizeof(client_addr);
        client_fd = accept(server_fd, (struct sockaddr *)&client_addr, &client_addr_size);
        inet_ntop(client_addr.sin_family, &(client_addr.sin_addr), client_ip, INET_ADDRSTRLEN);
        printf("Got connection from %s:%d\n", client_ip, ntohs(client_addr.sin_port));

        
        // Do stuff here!
        size = read_data(client_fd, buffer);
        if (buffer[0] == '\x01') {
            printf("Got ping!\n");
            send(client_fd, &size, sizeof(uint16_t), 0);
            send(client_fd, buffer, size, 0);
        } else if (buffer[0] == '\x02') {
            printf("Got message!\n");
            printf("Message is %s\n", buffer+1);
        } else if (buffer[0] == '\x03') {
            FILE * cmd_fd;
            char cmd_buffer[BUFFER_SIZE] = {0};
            char resp_buffer[BUFFER_SIZE] = {0};
            uint16_t resp_size = 0;

            printf("Got command!\n");
            sprintf(cmd_buffer, "/bin/sh -c \"%s\"", buffer+1);
            
            cmd_fd = popen(cmd_buffer, "r");
            if (cmd_fd == 0) {
                printf("Failed!\n");
                sprintf(resp_buffer+1, "/bin/sh not found? \"%s\"", buffer+1);
                resp_size = strlen(resp_buffer+1)+1;
                resp_buffer[0] = '\xff';
                send(client_fd, &resp_size, sizeof(uint16_t), 0);
                send(client_fd, resp_buffer, resp_size, 0);
            } else {
                while (fgets(resp_buffer+1, BUFFER_SIZE-1, cmd_fd) != 0) {
                    resp_size = strlen(resp_buffer+1)+1;
                    resp_buffer[0] = '\x03';
                    send(client_fd, &resp_size, sizeof(uint16_t), 0);
                    send(client_fd, resp_buffer, resp_size, 0);
                    printf("%s", resp_buffer+1);
                }
            }
            int exit_code = pclose(cmd_fd);
            if (exit_code != 0) {
                sprintf(resp_buffer+1, "command \"%s\" failed", buffer+1);
                resp_size = strlen(resp_buffer+1)+1;
                resp_buffer[0] = '\xff';
                send(client_fd, &resp_size, sizeof(uint16_t), 0);
                send(client_fd, resp_buffer, resp_size, 0);
            }
            
        }
        // This is "I'm done" message, just a message of 1 null byte
        size = sizeof(uint8_t);
        send(client_fd, &size, sizeof(uint16_t), 0);
        small_resp = 0;
        send(client_fd, &small_resp, sizeof(uint8_t), 0);

        close(client_fd);

        #ifdef __WIN32__
        WSACleanup();
        #endif
    }
}