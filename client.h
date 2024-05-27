#ifndef CLIENT_H
#define CLIENT_H

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

void sendUpdateMessage(int sockfd, UpdateMessage *msg);
void createAndSendUpdateMessage(int sockfd, float x, float y, float angle, bool isFiring);
int create_and_connect_socket();
void close_socket(int sockfd);

#endif // CLIENT_H
