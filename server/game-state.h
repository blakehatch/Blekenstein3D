#ifndef GAME_STATE_H
#define GAME_STATE_H

#include <array>
#include "player.h"

class GameState {
public:
    static const int maxPlayers = 10;

    GameState() {
        players.fill(Player());
        playerCount = 0;
    }

    // Add a player to the game state
    bool addPlayer(const Player& player);

    // Remove a player from the game state by index
    bool removePlayer(int index);

    // Get a player by index
    std::optional<std::reference_wrapper<Player>> getPlayer(int index);

    // Get the number of players
    int getPlayerCount() const;

    // Get the array of players
    const std::array<Player, maxPlayers>& getPlayers() const;

private:
    std::unordered_map<int, Player> playerMap;
    std::array<Player, 10> players;
    int playerCount;
};

#endif // GAME_STATE_H
