#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
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
        // Clear the buffer for the new message
        memset(buffer, 0, sizeof(buffer));

        // 5. Receive data from the client
        int recv_len = recv(new_socket, buffer, sizeof(buffer) - 1, 0);
        if (recv_len <= 0) {
            if (recv_len == 0) {
                printf("Client disconnected.\n");
            } else {
                perror("recv failed");
            }
            break;
        }

        buffer[recv_len] = '\0'; // Null-terminate the received string
        printf("Received from client (%d bytes): %s\n", recv_len, buffer);

        // Check if the message is alphanumeric
        int is_alphanumeric = 1;
        for (int i = 0; buffer[i] != '\0'; i++) {
            if (!isalnum(buffer[i]) && buffer[i] != ' ') {
                is_alphanumeric = 0;
                break;
            }
        }       

        if (!is_alphanumeric) {
            const char *error_msg = "Error: Please key in an alphanumeric format message only.\n";
            send(new_socket, error_msg, strlen(error_msg), 0);
            printf("Non-alphanumeric message received. Sent error message to client.\n");
            continue; // Continue to wait for the next message
        }

        // Handle "Exit Server" command
        if (strcmp(buffer, "Exit Server") == 0) {
            const char *exit_msg = "Exit command received. Closing connection...\n";
            send(new_socket, exit_msg, strlen(exit_msg), 0);
            printf("Exit command received. Closing connection...\n");
             send(new_socket, "Exit command received. Closing connection...\n", strlen("Exit command received. Closing connection...\n"), 0);
            break;
        }


        // 6. Reverse the string using bubble sort logic
        int len = strlen(buffer);
        for (int i = 0; i < len - 1; i++) {
            for (int j = 0; j < len - i - 1; j++) {
                char temp = buffer[j];
                buffer[j] = buffer[j + 1];
                buffer[j + 1] = temp;
            }
        }

        // Convert the reversed string to uppercase
        for (int i = 0; buffer[i] != '\0'; i++) {
            buffer[i] = toupper(buffer[i]);
        }

        // 7. Send the response to the client
        send(new_socket, buffer, strlen(buffer), 0);
        
        printf("Modified message sent to client: %s\n ", buffer);

        // Retrieve the client's address information
        struct sockaddr_in client_address;
        int client_addrlen = sizeof(client_address);
        if (getpeername(new_socket, (struct sockaddr *)&client_address, &client_addrlen) == 0) {
            char *client_ip = inet_ntoa(client_address.sin_addr);
            int client_port = ntohs(client_address.sin_port);
            printf("Client connected: IP = %s\n Port = %d\n", client_ip, client_port);
        } else {
            perror("getpeername failed");
        }
    }
    

    // 8. Close the sockets
    close(new_socket);
    close(server_fd);

    return 0;
    
}
