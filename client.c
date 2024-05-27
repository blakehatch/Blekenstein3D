#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h> // bzero()
#include <stdbool.h>
#include <sys/socket.h>
#include <unistd.h> // read(), write(), close()
#define MAX 80
#define PORT 8080
#define SA struct sockaddr

typedef struct {
    float x;
    float y;
    float angle;
    bool isFiring;
} UpdateMessage;

void sendUpdateMessage(int sockfd, UpdateMessage *msg) {
    const char *randomString = "This is a random string.";
    write(sockfd, randomString, strlen(randomString));
    write(sockfd, msg, sizeof(UpdateMessage));
}

void createAndSendUpdateMessage(int sockfd, float x, float y, float angle, bool isFiring) {
    UpdateMessage msg;
    msg.x = x;
    msg.y = y;
    msg.angle = angle;
    msg.isFiring = isFiring;
    sendUpdateMessage(sockfd, &msg);
}

void readUpdateMessages(int sockfd) {
    char buff[MAX];
    bzero(buff, sizeof(buff));
    int totalBytesRead = 0;
    int bytesRead = 0;

    // Read until the buffer is filled
    while (totalBytesRead < sizeof(buff) - 1) {
        bytesRead = read(sockfd, buff + totalBytesRead, sizeof(buff) - 1 - totalBytesRead);
        if (bytesRead <= 0) {
            break; // Exit if no more data is read or an error occurs
        }
        totalBytesRead += bytesRead;
    }
    buff[totalBytesRead] = '\0'; // Null-terminate the string

    printf("From Server : %s", buff);

    // Extract player count from the received message
    int playerCount = 0;
    sscanf(buff, "Sending list of %d players back.", &playerCount);

    // Dynamically allocate the updateMessages array based on player count
    UpdateMessage *updateMessages = (UpdateMessage *) malloc(playerCount * sizeof(UpdateMessage));
    if (updateMessages == NULL) {
        printf("Memory allocation failed for updateMessages.\n");
        return;
    }

    bytesRead = read(sockfd, updateMessages, playerCount * sizeof(UpdateMessage));
    if (bytesRead > 0) {
        for (int i = 0; i < bytesRead / sizeof(UpdateMessage); ++i) {
            printf("UpdateMessageFS %d: x=%f, y=%f, angle=%f, isFiring=%d\n", 
                   i, updateMessages[i].x, updateMessages[i].y, updateMessages[i].angle, updateMessages[i].isFiring);
        }
    } else {
        printf("Failed to read update messages from server.\n");
    }

    // Free the allocated memory
    free(updateMessages);
}

int create_and_connect_socket() {
    int sockfd;
    struct sockaddr_in servaddr;

    // socket create and verification
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        printf("socket creation failed...\n");
        exit(0);
    } else {
        printf("Socket successfully created..\n");
    }
    bzero(&servaddr, sizeof(servaddr));

    // assign IP, PORT
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr("127.0.0.1");
    servaddr.sin_port = htons(PORT);

    // connect the client socket to server socket
    if (connect(sockfd, (SA*)&servaddr, sizeof(servaddr)) != 0) {
        printf("connection with the server failed...\n");
        exit(0);
    } else {
        printf("connected to the server..\n");
    }

    return sockfd;
}

void close_socket(int sockfd) {
    close(sockfd);
}