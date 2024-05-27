#include <stdio.h>
#include <math.h>
#include "game.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

const int MAP[20][20] = {
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
};



int player_x = 7;
int player_y = 6;
float playerXF = 7.0f;
float playerYF = 6.0f;
float player_angle = 0.0f;
int playerFov = 70;

void getPlayerPosition(int *x, int *y, float *xf, float *yf, float *angle) {
    *x = player_x;
    *y = player_y;
    *xf = playerXF;
    *yf = playerYF;
    *angle = player_angle;
}

void movePlayer(int direction) {
    // direction: 0 = up, 1 = right, 2 = down, 3 = left
    float move_step = 0.1f;
    float newXF = playerXF;
    float newYF = playerYF;

    switch (direction) {
        case 0: // up
            newYF -= move_step;
            break;
        case 1: // right
            newXF += move_step;
            break;
        case 2: // down
            newYF += move_step;
            break;
        case 3: // left
            newXF -= move_step;
            break;
        default:
            return; // invalid direction
    }

    int new_x = round(newXF);
    int new_y = round(newYF);

    // Check if the new position is within bounds and not a wall
    if (new_x >= 0 && new_x < 20 && new_y >= 0 && new_y < 20 && MAP[new_y][new_x] == 0) {
        playerXF = newXF;
        playerYF = newYF;
        player_x = new_x;
        player_y = new_y;
    }
}

void movePlayerAtAngle(float angle, float distance) {
    float move_step = 0.1f;
    float newXF = playerXF;
    float newYF = playerYF;

    newXF += distance * cos(angle);
    newYF += distance * sin(angle);

    int new_x = round(playerXF);
    int new_y = round(playerYF);

    //printf("player_x: %d, player_y: %d\n", player_x, player_y);
    //printf("new_x: %d, new_y: %d\n", new_x, new_y);

    // Check if the new position is within bounds and not a wall
    if (new_x >= 0 && new_x < 20 && new_y >= 0 && new_y < 20 && MAP[new_y][new_x] == 0) {
        if (MAP[(int)round(newYF)][(int)round(newXF)] == 0) {
            playerXF = newXF;
            playerYF = newYF;
        }
        player_x = new_x;
        player_y = new_y;
    }
}

void moveForward() {
    movePlayerAtAngle(player_angle, 0.05f); // Move forward in the direction the player is facing
}

void moveBackward() {
    movePlayerAtAngle(player_angle + M_PI, 0.05f); // Move backward in the opposite direction the player is facing
}

void moveLeft() {
    movePlayerAtAngle(player_angle - M_PI_2, 0.05f); // Move left perpendicular to the direction the player is facing
}

void moveRight() {
    movePlayerAtAngle(player_angle + M_PI_2, 0.05f); // Move right perpendicular to the direction the player is facing
}

void rotateLeft() {
    player_angle -= 0.02f; // Rotate left by a small angle
    if (player_angle < 0) {
        player_angle += 2 * M_PI; // Ensure the angle stays within 0 to 2*PI
    }
}

void rotateRight() {
    player_angle += 0.02f; // Rotate right by a small angle
    if (player_angle >= 2 * M_PI) {
        player_angle -= 2 * M_PI; // Ensure the angle stays within 0 to 2*PI
    }
}

