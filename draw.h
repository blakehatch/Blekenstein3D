#include <SDL.h>

// Function to draw a pixel at (x, y) with a given color
void drawPixel(SDL_Renderer *renderer, int x, int y, SDL_Color color);

// Function to draw a vertical ray (column of pixels) at x with a given color
void drawColumn(SDL_Renderer *renderer, SDL_Color *pixels, int x, int screenHeight, float depth, SDL_Color topBottomColor, SDL_Texture *texture, int fixedTextureX);

//void drawBillboard(SDL_Renderer *renderer, int screenWidth, int screenHeight, float spriteX, float spriteY, float playerXF, float playerYF, int playerX, int playerY, float playerAngle, SDL_Color color, SDL_Texture *spriteTexture, int map[20][20]);

void drawMiniMap(SDL_Renderer *renderer, int map[20][20], float xf, float yf, int x, int y, float angle, 
    int playerFov, int mapTileSize, int mapWidth, int mapHeight, int screenWidth, 
    int screenHeight, float depthBuffer[1920]);

void drawPlayer(SDL_Renderer *renderer, int x, int y, float angle, SDL_Color color);
