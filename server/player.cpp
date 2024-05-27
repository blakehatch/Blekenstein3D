#include <cmath>

class Player {
public:
    Player() {
        x = 0.0f;
        y = 0.0f;
        health = 100;
        lastGunFireAngle = 0.0f;
        clientId = 0;
    }

    Player(float x, float y, int health, int clientId) {
        this->x = x;
        this->y = y;
        this->health = health;
        this->lastGunFireAngle = 0.0f;
        this->clientId = clientId;
    }

    // Getters
    float getX() const { return x; }
    float getY() const { return y; }
    int getHealth() const { return health; }
    float getLastGunFireAngle() const { return lastGunFireAngle; }
    int getClientId() const { return clientId; }

    // Setters
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
    void setHealth(int newHealth) { health = newHealth; }
    void setLastGunFireAngle(float angle) { lastGunFireAngle = angle; }

    // Methods
    void move(float deltaX, float deltaY) {
        x += deltaX;
        y += deltaY;
    }

    void fireGun(float angle) {
        lastGunFireAngle = angle;
        // Implement gun firing logic here
    }

private:
    float x;
    float y;
    int health;
    float lastGunFireAngle;
    int clientId;
};
