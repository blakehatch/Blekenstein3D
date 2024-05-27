#include "game-state.h"
#include <functional> // For std::reference_wrapper

GameState::GameState() {
    players.fill(Player());
    playerCount = 0;
}

bool GameState::addPlayer(const Player& player) {
    if (playerCount < maxPlayers) {
        if (playerMap.find(player.getClientId()) == playerMap.end()) {
            players[playerCount++] = player;
            playerMap[player.getClientId()] = player;
        }
        return true;
    }
    return false;
}

bool GameState::removePlayer(int index) {
    if (index < playerCount) {
        for (int i = index; i < playerCount - 1; ++i) {
            playerMap.erase(players[i].getClientId());
            players[i] = players[i + 1];
        }
        --playerCount;
        return true;
    }
    return false;
}

std::reference_wrapper<Player> GameState::getPlayer(int index) {
    if (index < playerCount) {
        return std::ref(players[index]);
    }
    throw std::out_of_range("Index out of range");
}

int GameState::getPlayerCount() const {
    return playerCount;
}

int GameState::getPlayerIndexByClientId(int clientId) const {
    auto it = playerMap.find(clientId);
    if (it != playerMap.end()) {
        for (int i = 0; i < playerCount; ++i) {
            if (players[i].getClientId() == clientId) {
                return i;
            }
        }
    }
    return 0;
}


const std::array<Player, GameState::maxPlayers>& GameState::getPlayers() const {
    return players;
}
