#include "player.h"
#include <cmath>

// Constructor definitions
Player::Player() : x(0.0f), y(0.0f), health(100), lastGunFireAngle(0.0f), clientId(0) {}

Player::Player(float x, float y, int health, int clientId) 
    : x(x), y(y), health(health), lastGunFireAngle(0.0f), clientId(clientId) {}

// Getter definitions
float Player::getX() const { return x; }
float Player::getY() const { return y; }
int Player::getHealth() const { return health; }
int Player::getClientId() const { return clientId; }

// Setter definitions
void Player::setX(float newX) { x = newX; }
void Player::setY(float newY) { y = newY; }
void Player::setHealth(int newHealth) { health = newHealth; }
void Player::setLastGunFireAngle(float angle) { lastGunFireAngle = angle; }

// Method definitions
void Player::move(float deltaX, float deltaY) {
    x += deltaX;
    y += deltaY;
}

void Player::fireGun(float angle) {
    lastGunFireAngle = angle;
    // Implement gun firing logic here
}
