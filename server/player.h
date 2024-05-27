#ifndef PLAYER_H
#define PLAYER_H

class Player {
public:
    Player();
    Player(float x, float y, int health, int clientId);

    // Getters
    float getX() const;
    float getY() const;
    int getHealth() const;
    float setLastGunFireAngle() const;
    int getClientId() const;

    // Setters
    void setX(float newX);
    void setY(float newY);
    void setHealth(int newHealth);
    void setLastGunFireAngle(float angle);

    // Methods
    void move(float deltaX, float deltaY);
    void fireGun(float angle);

private:
    float x;
    float y;
    int health;
    float lastGunFireAngle;
    int clientId;
};

#endif // PLAYER_H