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
    write(sockfd, msg, sizeof(UpdateMessage));
}

void createAndSendUpdateMessage(int sockfd, float x, float y, float angle, bool isFiring) {
    UpdateMessage msg;
    msg.x = x;
    msg.y = y;
    msg.angle = angle;
    msg.isFiring = isFiring;
    sendUpdateMessage(sockfd, &msg);
    char buff[MAX];
    bzero(buff, sizeof(buff));
    read(sockfd, buff, sizeof(buff));
    printf("From Server : %s", buff);
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

    printf("RETURNING");

    return sockfd;
}

void close_socket(int sockfd) {
    close(sockfd);
}