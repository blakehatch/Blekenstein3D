#ifndef SPRITE_H
#define SPRITE_H

typedef struct {
    float x;
    float y;
} Sprite;

void moveSprite(Sprite *sprite, float deltaX, float deltaY);
void setSpritePosition(Sprite *sprite, float x, float y);

#endif // SPRITE_H
