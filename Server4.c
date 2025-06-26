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

struct TimeZone {
    const char *code;
    int offset;
    } timezones[] = {
        {"PST", -8},
        {"MST", -7},
        {"CST", -6},
        {"EST", -5},
        {"GMT", 0},
        {"CET", +1},
        {"MSK", +3},
        {"JST", +9},
        {"AEDT", +11},
        {NULL, 0}  // End of array marker
    };

    int getTimeZoneOffset(const char *code) {
        for (int i = 0; timezones[i].code != NULL; i++) {
            if (strcmp(timezones[i].code, code) == 0) {
                return timezones[i].offset;
            }
        }
        return -99;  // Invalid time-zone code
    }




int main() {
    int server_fd, new_socket;
    struct sockaddr_in address;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    int offset;

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
        // Receive data from the client
        int recv_len = recv(new_socket, buffer, sizeof(buffer) - 1, 0); // Reserve space for null terminator

        if (recv_len == 0) {
            printf("Client disconnected.\n");
            break;
        } else if (recv_len < 0) {
            perror("recv failed");
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

        if (strcmp(buffer, "Time") == 0) {
            time_t now = time(NULL);
            struct tm *timeinfo = localtime(&now);

            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);

            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));

        } else if (strcmp(buffer, "Time PST") == 0) {
            int offset = -8;
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);

            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);
            
            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);

            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));
        } else if(strcmp(buffer, "Time MST") == 0){ 
            
            int offset = -7;
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

           // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);

            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time CST") == 0){

            int offset = -6; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);

            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));

        }else if(strcmp(buffer, "Time EST") == 0){ 
            
            int offset = -5; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);


            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time GMT") == 0){ 
            
            int offset = 0; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);


           // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time CET") == 0){ 
            
            int offset = +1; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);

            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time MSK") == 0){ 
            
            int offset = +3; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);


            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time JST") == 0){ 
            
            int offset = +9; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);


            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));


        }else if(strcmp(buffer, "Time AEDT") == 0){ 
            
            int offset = +11; 
            time_t now = time(NULL) + (offset * 3600);
            struct tm *timeinfo = gmtime(&now);
            
            char current_date[1024];
            strftime(current_date, sizeof(current_date), "%a %b %d %H:%M:%S %Y GMT", timeinfo);

            // Append the offset manually in the format "+ 8" or "- 8"
            char formatted_date[1024];
            snprintf(formatted_date, sizeof(formatted_date), "%s %c %d", current_date, offset >= 0 ? '+' : '-', abs(offset));

            // Send the response to the client
            send(new_socket, formatted_date, strlen(formatted_date), 0);
            printf("Current date sent to client: %s\n", formatted_date);


            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));



        } else {
            strcpy(buffer, "Command Error. Please only enter either Time and the location or Exit Server");
            send(new_socket, buffer, strlen(buffer), 0);
            printf("Sent the error message to client.\n");
            
            // Clear the buffer for the next message
            memset(buffer, 0, sizeof(buffer));
            continue;
            
            
        }




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

        // Clear the buffer for the new message
        memset(buffer, 0, sizeof(buffer));
    }

    // 8. Close the sockets
    close(new_socket);
    close(server_fd);


    return 0;
}
