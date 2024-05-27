#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <array>
#include <optional>
#include <unordered_map>
#include "player.h"

class GameState {
public:
    static const int maxPlayers = 10;

    GameState();

    // Add a player to the game state
    bool addPlayer(const Player& player);

    // Remove a player from the game state by index
    bool removePlayer(int index);

    // Get a player by index
    std::reference_wrapper<Player> getPlayer(int index);

    // Get the index of a player by client ID
    int getPlayerIndexByClientId(int clientId) const;

    // Get the number of players
    int getPlayerCount() const;

    const std::array<Player, maxPlayers>& getPlayers() const;

private:
    // Map the client IDs to the players for updating the game state
    std::unordered_map<int, Player> playerMap;
    std::array<Player, maxPlayers> players;
    int playerCount;
};

#endif // GAME_STATE_H