typedef struct {
    float x;
    float y;
} Sprite;

#include <stdlib.h>
#include <time.h>

Sprite sprites[10];

#include <stdbool.h>

#define MAX_SPRITES 10

void initializeSprites() {
    srand(time(NULL));
    for (int i = 0; i < 5; i++) {
        sprites[i].x = (float)(rand() % 100); // Random x position between 0 and 99
        sprites[i].y = (float)(rand() % 100); // Random y position between 0 and 99
    }
}

void moveSprite(Sprite *sprite, float deltaX, float deltaY) {
    sprite->x += deltaX;
    sprite->y += deltaY;
}

void setSpritePosition(Sprite *sprite, float x, float y) {
    sprite->x = x;
    sprite->y = y;
}

bool addSprite(float x, float y) {
    for (int i = 0; i < MAX_SPRITES; i++) {
        if (sprites[i].x == 0 && sprites[i].y == 0) { // Assuming (0,0) is an empty slot
            sprites[i].x = x;
            sprites[i].y = y;
            return true;
        }
    }
    return false; // No empty slot found
}

bool removeSprite(int index) {
    if (index >= 0 && index < MAX_SPRITES) {
        sprites[index].x = 0;
        sprites[index].y = 0;
        return true;
    }
    return false; // Invalid index
}
