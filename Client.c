#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#pragma comment(lib, "Ws2_32.lib")

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

#define PORT 8080

int main() {
    init_winsock();

    int client_fd;
    struct sockaddr_in server_address;
    char buffer[1024] = {0};
    char message[1024] = {0}; // Allocate and initialize memory for the message

    // Create the socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (client_fd == INVALID_SOCKET) {
        fprintf(stderr, "Socket creation failed: %d\n", WSAGetLastError());
        cleanup_winsock();
        exit(EXIT_FAILURE);
    }

    // Configure server address
    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1");

    // Connect to the server
    if (connect(client_fd, (struct sockaddr *)&server_address, sizeof(server_address)) < 0) {
        fprintf(stderr, "Connection failed: %d\n", WSAGetLastError());
        closesocket(client_fd);
        cleanup_winsock();
        exit(EXIT_FAILURE);
    }
    printf("Connected to the server.\n");

    // Communicate with the server
    while (1) {
        // Input message
        printf("Enter your message (Enter Exit Server to exit): ");
        if (!fgets(message, sizeof(message), stdin)) {
            fprintf(stderr, "Error reading input\n");
            break;
        }

        // Remove trailing newline from fgets
        message[strcspn(message, "\n")] = '\0';

        // Send message to the server
        if (send(client_fd, message, strlen(message), 0) < 0) {
            fprintf(stderr, "Failed to send message\n");
            break;
        }
        printf("Sent to server: %s\n", message);

        // Check for exit condition
        if (strcmp(message, "Exit Server") == 0) {
            printf("Exiting as per user request.\n");
            break;
        }

        // Receive response from the server
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0); // Leave space for null-terminator
        if (bytes_received < 0) {
            fprintf(stderr, "Failed to receive data\n");
            break;
        } else if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        }

        // Null-terminate received string
        buffer[bytes_received] = '\0';
        printf("Received from server: %s\n", buffer);

        // Clear buffers for the next message
        memset(buffer, 0, sizeof(buffer));
        memset(message, 0, sizeof(message));

    }

    // Close the socket and clean up
    closesocket(client_fd);
    cleanup_winsock();

    return 0;
}
