#include <array>
#include <optional>

#include "player.h"

class GameState {
public:
    static const int maxPlayers = 10;

    GameState() {
        players.fill(Player());
        playerCount = 0;
    }

    // Add a player to the game state
    bool addPlayer(const Player& player) {
        if (playerCount < maxPlayers) {
            players[playerCount++] = player;
            playerMap[player.getClientId()] = player;
            return true;
        }
        return false;
    }

    // Remove a player from the game state by index
    bool removePlayer(int index) {
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

    // Get a player by index
    std::optional<std::reference_wrapper<Player>> getPlayer(int index) {
        if (index < playerCount) {
            return players[index];
        }
        return std::nullopt;
    }

    // Get the number of players
    int getPlayerCount() const {
        return playerCount;
    }

    const std::array<Player, maxPlayers>& getPlayers() const {
        return players;
    }

private:
    // Map the client IDs to the players for updating the game state
    std::unordered_map<int, Player> playerMap;
    std::array<Player, maxPlayers> players;
    int playerCount;
};
