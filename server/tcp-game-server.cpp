#include <iostream>
#include <cstdlib>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sstream>
#include <arpa/inet.h>
#include <netdb.h>
#include <type_traits>
#include <unistd.h>
#include <vector>
#include <mutex>

#include "game-state.h"
#include "player.h"

// Checks If a c-style string is an integer
bool is_int(char *c) {
  while (*c != '\0')
  {
    if (*c < '0' || *c > '9')
      return false;
    c++;
  }
  return true;
}

typedef struct {
    float x;
    float y;
    float angle;
    bool isFiring;
} UpdateMessage;

std::mutex gameStateMutex;

void handle_client(int sock_client, GameState &gameState) {
    UpdateMessage msg;
    int bytes;

    Player player; // Initialize with default values


  while (true) {
    bytes = recv(sock_client, &msg, sizeof(UpdateMessage), 0);

    if (bytes == 0) {
        std::cout << "[INFO] Client is disconnected.\n";

        if (gameState.removePlayer(sock_client)) {
            std::cout << "[INFO] Player is removed from the game state.\n";
        } else {
            std::cerr << "[ERROR] Failed to remove player from the game state.\n";
        }

        break;
        } else if (bytes < 0) {
            std::cerr << "[ERROR] Something went wrong while receiving data!.\n";
            break;
        } else {
            std::cout << "Received UpdateMessage from sock_client #" << sock_client << ": x=" << msg.x << ", y=" << msg.y << ", angle=" << msg.angle << ", isFiring=" << msg.isFiring << "\n";
            player.setX(msg.x);
            player.setY(msg.y);
            player.setLastGunFireAngle(msg.angle);
            player.setHealth(player.getHealth()); // Assuming health remains unchanged
            if (msg.isFiring) {
                player.fireGun(msg.angle);
            }

            if (gameState.addPlayer(player)) {
                std::cout << "[INFO] Player added to the game state.\n";
            } else {
                std::cerr << "[ERROR] Failed to add player to the game state.\n";
            }

            std::ostringstream ack_stream;
            ack_stream << "Sending list of " << gameState.getPlayerCount() << " players back.";

            UpdateMessage updateMessages[GameState::maxPlayers];
            const auto& players = gameState.getPlayers();
            int playerCount = gameState.getPlayerCount();

            for (int i = 0; i < playerCount; ++i) {
                const Player& p = players[i];
                UpdateMessage updateMsg;
                updateMsg.x = p.getX();
                updateMsg.y = p.getY();
                updateMsg.angle = 0.0f; // Assuming angle is the last gun fire angle
                updateMsg.isFiring = 1; // Assuming Player class has a method isFiring()
                updateMessages[i] = updateMsg;
            }

            std::string ack = ack_stream.str();

            if (send(sock_client, ack.c_str(), ack.length(), 0) < 0) {
                std::cerr << "[ERROR] Acknowledgment cannot be sent, exiting...\n";
                break;
            }

            // Send list of UpdateMessage structs to the client
            if (send(sock_client, updateMessages, sizeof(updateMessages), 0) < 0) {
                std::cerr << "[ERROR] Update messages cannot be sent, exiting...\n";
                break;
            }
        }

    }

    // Close client socket
    close(sock_client);
    std::cout << "[INFO] Client socket is closed.\n";
}

int main(int argc, char **argv) {
    GameState gameState = GameState();

    if (argc != 2 || !is_int(argv[1])) {
        std::cerr << "[ERROR] Port is not provided via command line parameters!\n";
        return -1;
    }
    
    // Create a socket & get the file descriptor
    int sock_listener = socket(AF_INET, SOCK_STREAM, 0);
    // Check If the socket is created
    if (sock_listener < 0) {
        std::cerr << "[ERROR] Socket cannot be created!\n";
        return -2;
    }

    std::cout << "[INFO] Socket has been created.\n";

    // Set SO_REUSEADDR option
    int opt = 1;
    if (setsockopt(sock_listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        std::cerr << "[ERROR] setsockopt(SO_REUSEADDR) failed\n";
        return -1;
    }

    // Address info to bind socket
    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(std::atoi(argv[1]));
    //server_addr.sin_addr.s_addr = INADDR_ANY;
    // OR
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);
    
    char buf[INET_ADDRSTRLEN];

    // Bind socket
    if (bind(sock_listener, (sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "[ERROR] Created socket cannot be binded to ( "
                << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
                << ":" << ntohs(server_addr.sin_port) << ")\n";
        return -3;
    }

    std::cout << "[INFO] Sock is binded to ("  
                << inet_ntop(AF_INET, &server_addr.sin_addr, buf, INET_ADDRSTRLEN)
                << ":" << ntohs(server_addr.sin_port) << ")\n";


    // Start listening
    if (listen(sock_listener, SOMAXCONN) < 0) {
        std::cerr << "[ERROR] Socket cannot be switched to listen mode!\n";
        return -4;
    }
    std::cout << "[INFO] Socket is listening now.\n";

    // Accept a call
    sockaddr_in client_addr;
    socklen_t client_addr_size = sizeof(client_addr); 
    int sock_client; 

    while (true) {

        if ((sock_client = accept(sock_listener, (sockaddr*)&client_addr, &client_addr_size)) < 0) {
            std::cerr << "[ERROR] Connections cannot be accepted for a reason.\n";
            continue;
        }

        std::cout << "[INFO] A connection is accepted now.\n";

        // Get name info
        char host[NI_MAXHOST];
        char svc[NI_MAXSERV];
        if (getnameinfo(
            (sockaddr*)&client_addr, client_addr_size,
            host, NI_MAXHOST,
            svc, NI_MAXSERV, 0) != 0) {
            std::cout << "[INFO] Client: (" << inet_ntop(AF_INET, &client_addr.sin_addr, buf, INET_ADDRSTRLEN)
                        << ":" << ntohs(client_addr.sin_port) << ")\n"; 
        } else {
            std::cout << "[INFO] Client: (host: " << host << ", service: " << svc << ")\n";
        }

        // Handle client
        handle_client(sock_client, std::ref(gameState));
    }

    // Close main listener socket
    close(sock_listener);
    std::cout << "[INFO] Main listener socket is closed.\n";

    return 0;
}
