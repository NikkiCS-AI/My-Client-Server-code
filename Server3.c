#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

#define close(fd) closesocket(fd)

// Automatic Winsock initialization and cleanup
static void init_winsock() __attribute__((constructor));
static void cleanup_winsock() __attribute__((destructor));

static void init_winsock() {
    WSADATA wsaData;
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        fprintf(stderr, "WSAStartup failed\n");
        exit(EXIT_FAILURE);
    }
}

static void cleanup_winsock() {
    WSACleanup();
}

#define PORT 8080  // Port number for the server

int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};

    // 1. Create socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    // 2. Bind socket to an IP/Port
    address.sin_family = AF_INET;          // IPv4
    address.sin_addr.s_addr = INADDR_ANY;  // Any IP address
    address.sin_port = htons(PORT);        // Host to Network Short

    if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
        perror("Binding failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 3. Listen for incoming connections
    if (listen(server_fd, 1) < 0) {
        perror("Listening failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }
    printf("Server is listening on port %d...\n", PORT);

    // 4. Accept a connection
    new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t*)&addrlen);
    if (new_socket == -1) {
        perror("Accepting connection failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    while (1) {
        

        // 5. Receive data from the client
        int recv_len = recv(new_socket, buffer, sizeof(buffer) - 1, 0); // Reserve space for null terminator
        if (recv_len <= 0) {
            if (recv_len == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("recv failed");
            }
            break;
        }

        // Null-terminate the received string
        buffer[recv_len] = '\0';
        printf("Received from client: %s\n", buffer);

        // Check for "Exit Server" command
        if (strcmp(buffer, "Exit Server") == 0) {
            printf("Exit command received. Closing connection...\n");
            send(new_socket, "Exit command received. Closing connection...\n", strlen("Exit command received. Closing connection...\n"), 0);
            break;
        }

        if (strcmp(buffer, "Date") == 0) {
            time_t now = time(NULL);
            struct tm *timeinfo = localtime(&now);
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT %z\r\n", timeinfo);

            // Send the current date
            send(new_socket, current_date, strlen(current_date), 0);
            printf("Current date sent to client: %s\n", current_date);
        } else {
            const char *error_message = "Command Error. Please only enter either Date or Exit Server";
            send(new_socket, error_message, strlen(error_message), 0);
            printf("Sent error message to client.\n");
        }

        // Clear the buffer
        memset(buffer, 0, sizeof(buffer));
    }

    // 8. Close the sockets
    close(new_socket);
    close(server_fd);


    return 0;
}
