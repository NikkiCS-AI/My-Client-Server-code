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

    // Initialize fd_set
    fd_set readfds;
    FD_ZERO(&readfds);  // Initialize the set
    int max_fd;

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

    // Add server socket to readfds
    FD_SET(server_fd, &readfds);
    max_fd = server_fd;  // Initialize max_fd with the server socket

    while (1) {
        // Create a copy of readfds to pass to select
        fd_set temp_fds = readfds;

        // Use select to wait for activity on any socket
        int activity = select(max_fd + 1, &temp_fds, NULL, NULL, NULL);
        if (activity < 0) {
            perror("select error");
            break;
        }

        // Check for new incoming connections
        if (FD_ISSET(server_fd, &temp_fds)) {
            new_socket = accept(server_fd, (struct sockaddr *)&address, (socklen_t *)&addrlen);
            if (new_socket < 0) {
                perror("Accepting connection failed");
                continue;
            }

            FD_SET(new_socket, &readfds);  // Add new client to the fd_set
            if (new_socket > max_fd) max_fd = new_socket;  // Update max_fd if necessary
            printf("New client connected: %d\n", new_socket);
        }

        // Handle communication with existing clients
        for (int i = 0; i <= max_fd; i++) {
            if (i != server_fd && FD_ISSET(i, &temp_fds)) {

                // Clear the buffer for the new message
                memset(buffer, 0, sizeof(buffer));

                // Receive data from the client
                int bytes_read = recv(i, buffer, sizeof(buffer) - 1, 0);
                if (bytes_read <= 0) {

                    // Client disconnected
                    printf("Client %d disconnected\n", i);
                    close(i);
                    FD_CLR(i, &readfds);  // Remove from fd_set

                } else {
                    
                    // Forward data to other clients
                    buffer[bytes_read] = '\0';
                    printf("Received from client %d: %s\n", i, buffer);
                    for (int j = 0; j <= max_fd; j++) {
                        if (j != server_fd && j != i && FD_ISSET(j, &readfds)) {
                            send(j, buffer, bytes_read, 0);  // Send message to all other clients
                        }
                    }
                }
            }
        }
    }

    // Close the server socket after exiting the loop
    close(server_fd);

    return 0;
}
