#include <SDL.h>
#include <stdbool.h>
#include "draw.h"

// Function to draw a pixel at (x, y) with a given color
void drawPixel(SDL_Renderer *renderer, int x, int y, SDL_Color color) {
    // Set the drawing color
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    // Draw the pixel
    SDL_RenderDrawPoint(renderer, x, y);
}

SDL_Color getPixelColor(SDL_Renderer *renderer, SDL_Color *pixels, int textureWidth, int textureHeight, int x, int y) {
    SDL_Color color;

    // Ensure x and y are within bounds
    if (x < 0 || x >= textureWidth || y < 0 || y >= textureHeight) {
        // Return a default color (e.g., black) if out of bounds
        color.r = 0;
        color.g = 0;
        color.b = 0;
        color.a = 255;
        return color;
    }

    color = pixels[(y * textureWidth) + x];

    // Extract the color components
    //printf("Extracted Pixel Color: %d, %d, %d, %d\n", color.r, color.g, color.b, color.a);

    return color;
}

// Function to draw a vertical ray (column of pixels) at x with a given color
// void drawColumn(SDL_Renderer *renderer, SDL_Color *pixels, int x, int screenHeight, float depth, SDL_Color topBottomColor, SDL_Texture *wallTexture, int fixedTextureX) {
//     int lineHeight = (int)(screenHeight / depth);

//     // Calculate the start and end points of the line
//     int drawStart = -lineHeight / 2 + screenHeight / 2;
//     if (drawStart < 0) drawStart = 0;
//     int drawEnd = lineHeight / 2 + screenHeight / 2;
//     if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

//     // Draw the top part (bright blue)
//     SDL_SetRenderDrawColor(renderer, 0, 0, 255, topBottomColor.a); // Bright blue color
//     SDL_RenderDrawLine(renderer, x, 0, x, drawStart);

//     // Draw the bottom part (brownish green)
//     SDL_SetRenderDrawColor(renderer, 139, 69, 19, topBottomColor.a); // Brownish green color
//     SDL_RenderDrawLine(renderer, x, drawEnd, x, screenHeight);

//     int textureWidth = 24 * 10;
//     int textureHeight = 62 * 10;
//     //SDL_QueryTexture(wallTexture, NULL, NULL, &textureWidth, &textureHeight);

//     int prevY = drawStart;
//     SDL_Color prevColor = getPixelColor(renderer, pixels, textureWidth, textureHeight, fixedTextureX % textureWidth, 0);

//     for (int y = drawStart; y <= drawEnd; y++) {
//         int textureY = ((y - drawStart) * textureHeight) / (drawEnd - drawStart + 1);
//         SDL_Color dotColor = getPixelColor(renderer, pixels, textureWidth, textureHeight, fixedTextureX % textureWidth, textureY);

//         float distanceFactor = depth / 10.0f; // Make the blend stronger by reducing the divisor
//         if (distanceFactor > 1.0f) distanceFactor = 1.0f;
//         SDL_Color blendedColor = {
//             (Uint8)(dotColor.r * (1.0f - distanceFactor) + 128 * distanceFactor),
//             (Uint8)(dotColor.g * (1.0f - distanceFactor) + 128 * distanceFactor),
//             (Uint8)(dotColor.b * (1.0f - distanceFactor) + 128 * distanceFactor),
//             dotColor.a
//         };

//         if (y > drawStart && (blendedColor.r != prevColor.r || blendedColor.g != prevColor.g || blendedColor.b != prevColor.b || blendedColor.a != prevColor.a)) {
//             SDL_SetRenderDrawColor(renderer, prevColor.r, prevColor.g, prevColor.b, prevColor.a);
//             SDL_RenderDrawLine(renderer, x, prevY, x, y - 1);
//             prevY = y;
//             prevColor = blendedColor;
//         }
//     }

//     // Draw the last segment
//     SDL_SetRenderDrawColor(renderer, prevColor.r, prevColor.g, prevColor.b, prevColor.a);
//     SDL_RenderDrawLine(renderer, x, prevY, x, drawEnd);
// }

void drawColumn(SDL_Renderer *renderer, SDL_Color *pixels, int x, int screenHeight, float depth, SDL_Color topBottomColor, SDL_Texture *wallTexture, int fixedTextureX) {
    int lineHeight = (int)(screenHeight / depth);

    // Calculate the start and end points of the line
    int drawStart = -lineHeight / 2 + screenHeight / 2;
    if (drawStart < 0) drawStart = 0;
    int drawEnd = lineHeight / 2 + screenHeight / 2;
    if (drawEnd >= screenHeight) drawEnd = screenHeight - 1;

    // Draw the top part (bright blue)
    SDL_SetRenderDrawColor(renderer, 0, 0, 255, 255); // Bright blue color
    SDL_RenderDrawLine(renderer, x, 0, x, drawStart);

    // Draw the bottom part (brownish green)
    SDL_SetRenderDrawColor(renderer, 139, 69, 19, 255); // Brownish green color
    SDL_RenderDrawLine(renderer, x, drawEnd, x, screenHeight);

    // Draw the middle part (darker green getting closer to grey with distance)
    float distanceFactor = depth / 10.0f; // Adjust the divisor to control the rate of blending
    if (distanceFactor > 1.0f) distanceFactor = 1.0f;
    Uint8 green = (Uint8)(128 * (1.0f - distanceFactor) + 128 * distanceFactor);
    Uint8 grey = (Uint8)(128 * distanceFactor);
    SDL_SetRenderDrawColor(renderer, 0, green, grey, 255);
    SDL_RenderDrawLine(renderer, x, drawStart, x, drawEnd);
}



// Function to check if the line from (x0, y0) to (x1, y1) intersects a wall
bool lineIntersectsWall(int x0, int y0, int x1, int y1, int map[20][20]) {
    int dx = abs(x1 - x0);
    int dy = abs(y1 - y0);
    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        int mapX = x0 / 64; // Assuming each map tile is 64x64 pixels
        int mapY = y0 / 64;
        if (mapX < 0 || mapX >= 20 || mapY < 0 || mapY >= 20) return true; // Out of bounds
        if (map[mapY][mapX] == 1) return true; // Wall tile

        if (x0 == x1 && y0 == y1) break;
        int e2 = 2 * err;
        if (e2 > -dy) { err -= dy; x0 += sx; }
        if (e2 < dx) { err += dx; y0 += sy; }
    }
    return false;
}

// A billboard in this context is a 2D object that always faces the camera, giving the illusion of a 3D object.
// void drawBillboard(SDL_Renderer *renderer, int screenWidth, int screenHeight, float spriteX, float spriteY, int playerX, int playerY, float playerAngle, SDL_Color color, SDL_Texture *spriteTexture, int map[20][20]) {
//     // Calculate the direction from the player to the sprite
//     // float spriteDir = atan2(spriteY - playerY, spriteX - playerX) - playerAngle;
//     // while (spriteDir > M_PI) spriteDir -= 2 * M_PI;
//     // while (spriteDir < -M_PI) spriteDir += 2 * M_PI;

//     // // Calculate the depth from the player to the sprite
//     // float depth = sqrt((spriteX - playerX) * (spriteX - playerX) + (spriteY - playerY) * (spriteY - playerY));

//     // // Calculate the size of the sprite on the screen
//     // int spriteScreenSize = (int)(screenHeight / depth);
//     // int hOffset = (int)((spriteDir) * (screenWidth / 2) / (M_PI / 3) + (screenWidth / 2) / 2 - spriteScreenSize / 2);
//     // int vOffset = screenHeight / 2 - spriteScreenSize / 2;

//     // //if (!line"IntersectsWall((int)(playerX * 64), (int)(playerY * 64), (int)(spriteX * 64), (int)(spriteY * 64), map)) {
//     //     printf("Drawing sprite at (%f, %f)\n", spriteX, spriteY);
//     //     SDL_Rect destRect = { hOffset, vOffset, spriteScreenSize, spriteScreenSize };
//     //     SDL_RenderCopy(renderer, spriteTexture, NULL, &destRect);

//     if (spriteTexture == NULL) {
//         printf("Failed to load texture: %s\n", SDL_GetError());
//     }

//     // Render the BMP in the middle of the screen
//     int spriteScreenSize = 100; // Example size
//     int hOffset = (screenWidth - spriteScreenSize) / 2;
//     int vOffset = (screenHeight - spriteScreenSize) / 2;
//     SDL_Rect destRect = { hOffset, vOffset, spriteScreenSize, spriteScreenSize };
//     SDL_RenderCopy(renderer, spriteTexture, NULL, &destRect);
//     //}
// }


void drawMiniMap(SDL_Renderer *renderer, int map[20][20], float playerXF, float playerYF, int playerX, int playerY, float playerAngle, int playerFov, int mapTileSize, int mapWidth, int mapHeight, int screenWidth, int screenHeight, float depthBuffer[1920]) {
    // Set the color for the map
    SDL_Color mapColor = { 255, 255, 255, 255 };
    SDL_Color playerColor = { 255, 0, 0, 255 };
    SDL_Color wallColor = { 0, 0, 0, 255 };
    SDL_Color floorColor = { 255, 255, 255, 255 };


    // Calculate the top-right corner position
    int offsetX = screenWidth - (mapWidth * mapTileSize) - 10;
    int offsetY = 10;

    // Draw the map
    for (int y = 0; y < mapHeight; y++) {
        for (int x = 0; x < mapWidth; x++) {
            // Get the tile value
            int tile = map[y][x];
            // Set the color based on the tile value
            SDL_Color color = (tile == 1) ? wallColor : floorColor;
            // Draw the tile
            for (int i = 0; i < mapTileSize; i++) {
                for (int j = 0; j < mapTileSize; j++) {
                    drawPixel(renderer, offsetX + x * mapTileSize + i, offsetY + y * mapTileSize + j, color);
                }
            }
        }
    }

    // Draw the player
    int radius = mapTileSize / 3;
    int centerX = offsetX + playerXF * mapTileSize + mapTileSize / 2;
    int centerY = offsetY + playerYF * mapTileSize + mapTileSize / 2;

    for (int w = 0; w < mapTileSize * 2; w++) {
        for (int h = 0; h < mapTileSize * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                drawPixel(renderer, centerX + dx, centerY + dy, playerColor);
            }
        }
    }

    // Call the function to draw rays
    //drawRays(renderer, playerX, playerY, angle, playerFov, map, mapTileSize, mapWidth, mapHeight, screenWidth, screenHeight);
    int numRays = 1920; // Number of rays to draw on the minimap
    float angleStep = 0.0005;
    float startAngle = playerAngle - (playerFov * M_PI / 180.0f) / 4.0f;

    for (int i = 0; i < numRays; i++) {
        float rayAngle = startAngle + i * angleStep;
        float rayX = playerXF; // Start from the center of the player's tile
        float rayY = playerYF;
        float rayDirX = cos(rayAngle);
        float rayDirY = sin(rayAngle);

        // Cast the ray until it hits a wall or goes out of bounds
        while (rayX >= 0 && rayX < mapWidth && rayY >= 0 && rayY < mapHeight) {
            int mapX = (int)rayX;
            int mapY = (int)rayY;

            if (map[mapY][mapX] == 1) {
                // Hit a wall, draw the ray on the minimap
                SDL_Color rayColor = { 255, 0, 0, 255 }; // Red color for the ray
                int contactX = offsetX + (int)(rayX * mapTileSize);
                int contactY = offsetY + (int)(rayY * mapTileSize);
                // drawPixel(renderer, contactX, contactY, rayColor);

                // Draw a line between the contact point and the player
                int playerScreenX = offsetX + (int)((playerXF + 0.5f) * mapTileSize);
                int playerScreenY = offsetY + (int)((playerYF + 0.5f) * mapTileSize);
                SDL_SetRenderDrawColor(renderer, rayColor.r, rayColor.g, rayColor.b, rayColor.a);
                SDL_RenderDrawLine(renderer, playerScreenX, playerScreenY, contactX, contactY);

                // Figure out how to track contact points so it always draws the same texture contacting
                // the same wall

                // INSERT_YOUR_CODE
                // Calculate the distance from the player to the wall
                float distance = sqrt((rayX - (playerXF + 0.5f)) * (rayX - (playerXF + 0.5f)) + (rayY - (playerYF + 0.5f)) * (rayY - (playerYF + 0.5f)));

                // Calculate the screen position of the ray
                int screenX = (int)((rayAngle - startAngle) / (playerFov * M_PI / 180.0f) * screenWidth);

                // Ensure screenX is within bounds
                depthBuffer[i] = distance;



                // INSERT_YOUR_CODE

                break;
            }

            rayX += rayDirX * 0.1f; // Move the ray forward
            rayY += rayDirY * 0.1f;
        }
    }


    // Draw the player angle ray as green

    // Calculate the end point of the ray at the player angle
    // float rayEndX = playerX + cos(playerAngle) * 100; // 100 is an arbitrary length for the ray
    // float rayEndY = playerY + sin(playerAngle) * 100;

    // // Convert the player's position to screen coordinates
    // int playerScreenX = offsetX + (int)((playerX + 0.5f) * mapTileSize);
    // int playerScreenY = offsetY + (int)((playerY + 0.5f) * mapTileSize);

    // // Convert the ray end point to screen coordinates
    // int rayEndScreenX = offsetX + (int)(rayEndX * mapTileSize);
    // int rayEndScreenY = offsetY + (int)(rayEndY * mapTileSize);

    // // Set the color for the ray
    // SDL_Color rayColor = { 0, 255, 0, 255 }; // Green color for the ray

    // // Draw the ray from the player to the end point
    // SDL_SetRenderDrawColor(renderer, rayColor.r, rayColor.g, rayColor.b, rayColor.a);
    // SDL_RenderDrawLine(renderer, playerScreenX, playerScreenY, rayEndScreenX, rayEndScreenY);
}

void drawPlayer(SDL_Renderer *renderer, int x, int y, float angle, SDL_Color color) {
    // Draw the player
    int radius = 10 / 3; // Assuming mapTileSize is 10
    int centerX = x * 10 + 10 / 2;
    int centerY = y * 10 + 10 / 2;

    for (int w = 0; w < 10 * 2; w++) {
        for (int h = 0; h < 10 * 2; h++) {
            int dx = radius - w; // horizontal offset
            int dy = radius - h; // vertical offset
            if ((dx * dx + dy * dy) <= (radius * radius)) {
                drawPixel(renderer, centerX + dx, centerY + dy, color);
            }
        }
    }
}

void drawGUI(SDL_Renderer *renderer, int screenWidth, int screenHeight) {
    // Draw the GUI
    SDL_Color guiColor = { 0, 0, 0, 255 };
    SDL_Rect guiRect = { 0, screenHeight - 50, screenWidth, 50 };
    SDL_SetRenderDrawColor(renderer, guiColor.r, guiColor.g, guiColor.b, guiColor.a);
    SDL_RenderFillRect(renderer, &guiRect);
}