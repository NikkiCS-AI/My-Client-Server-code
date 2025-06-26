#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>

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

// Thread function to handle incoming messages
DWORD WINAPI ClientHandler(LPVOID param) {
    int client_fd = *(int *)param;
    char buffer[1024];

    while (1) {
        memset(buffer, 0, sizeof(buffer));  // Clear the buffer
        int bytes_received = recv(client_fd, buffer, sizeof(buffer) - 1, 0);
        if (bytes_received <= 0) {
            if (bytes_received == 0) {
                printf("Server disconnected.\n");
            } else {
                fprintf(stderr, "Error receiving message: %d\n", WSAGetLastError());
            }
            break;
        }

        buffer[bytes_received] = '\0';  // Null-terminate received string
        printf("\nSecondClient: %s\n", buffer);
    }

    return 0;
}

int main() {
    init_winsock();

    int client_fd;
    struct sockaddr_in server_address;
    char message[1024];

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

    // Create a thread to handle incoming messages
    HANDLE hThread = CreateThread(NULL, 0, ClientHandler, &client_fd, 0, NULL);
    if (hThread == NULL) {
        fprintf(stderr, "CreateThread failed: %d\n", GetLastError());
        closesocket(client_fd);
        cleanup_winsock();
        exit(EXIT_FAILURE);
    }

    // Main loop for sending messages
    while (1) {
        // printf("Enter your message: ");
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

        // Check for exit condition
        if (strcmp(message, "Exit Server") == 0) {
            printf("Exiting as per user request.\n");
            break;
        }
    }

    // Close the socket and clean up
    closesocket(client_fd);
    cleanup_winsock();

    return 0;
}
