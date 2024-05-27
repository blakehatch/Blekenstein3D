#ifndef GAME_H
#define GAME_H

extern const int map[20][20];

extern int depthBuffer[100];

extern int player_x;
extern int player_y;
extern float player_angle;
extern int playerFov;

void getPlayerPosition(int *x, int *y, float *fx, float *fy, float *angle);
void movePlayer(int direction);

void moveForward();
void moveBackward();
void moveLeft();
void moveRight();
void rotateLeft();
void rotateRight();


#endif // GAME_H
