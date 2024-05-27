#include <SDL.h>

#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <sys/select.h>

#include "draw.h"
#include "game.h"
#include "client.h"

SDL_Surface* loadBMP(const char* filePath) {
    SDL_Surface* surface = SDL_LoadBMP(filePath);
    if (!surface) {
        printf("Failed to load BMP file: %s\n", SDL_GetError());
        return NULL;
    }
    return surface;
}

SDL_Texture* createTextureFromBMP(SDL_Renderer* renderer, const char* filePath) {
    SDL_Surface* surface = loadBMP(filePath);
    if (!surface) {
        return NULL;
    }
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    if (!texture) {
        printf("Failed to create texture: %s\n", SDL_GetError());
        return NULL;
    }
    return texture;
}

int setNonBlocking(int socket_fd) {
    int flags = fcntl(socket_fd, F_GETFL, 0);
    if (flags == -1) {
        return -1;
    }
    return fcntl(socket_fd, F_SETFL, flags | O_NONBLOCK);
}

/* The main() function displays a BMP file on the screen and waits for
 * the user to press the X to close the window.
 *
 * There are no error checks for brevity.  If something goes wrong,
 * it's easy to add a check and a call to SDL_GetError() to figure out
 * why.
 */
int main( int argc, char *argv[] ) {

  /* The width, height and framerate of our window.
   */
  int screenWidth = 1920, screenHeight = 1080, f = 100;

  /* On a POSIX system we'd use getopt() but on Windows we have to
   * roll our own command line parser.  See the README file for how
   * to use these flags.
   */
  for ( int i = 1; i < argc; i++ ) {
    if ( strcmp( argv[ i ], "-x" ) == 0 && argc > i + 1 ) {
      screenWidth = (int) strtol( argv[ ++i ], NULL, 10 );
    } else if ( strcmp( argv[ i ], "-y" ) == 0 && argc > i + 1 ) {
      screenHeight = (int) strtol( argv[ ++i ], NULL, 10 );
    } else if ( strcmp( argv[ i ], "-f" ) == 0 && argc > i + 1 ) {
      f = (int) strtol( argv[ ++i ], NULL, 10 );
    }
  }

  /* Initialize the socket connection */
  int socket_fd = create_and_connect_socket();
  if (socket_fd == -1) {
    printf("Failed to create and connect socket\n");
    return 1;
  }

  if (setNonBlocking(socket_fd) == -1) {
    printf("Failed to set socket to non-blocking mode\n");
    return 1;
  }

  /* We initialize the SDL video and event subsystems.  If we forget
   * or leave ot the SDL_INIT_EVENTS, the SDL_CreateWindow() will
   * initialize it for us.  There is no error checking in our example
   * code, and in a real game we might explicitly check if this fails
   * and try to notify the user somehow.
   */
  SDL_Init( SDL_INIT_VIDEO | SDL_INIT_EVENTS );

  /* We create a new window to display our message.  Here we pass in *
   * the title text, where it should be on the screen (which is *
   * undefined) and the size as x and y, and finally a flag to say *
   * it's shown.
   */
  SDL_Window *window = SDL_CreateWindow( "Blekenstein 3D", 
					 SDL_WINDOWPOS_UNDEFINED,
					 SDL_WINDOWPOS_UNDEFINED, 
					 screenWidth, screenHeight, SDL_WINDOW_SHOWN );

  /* We load the bitmap we're going to display as our "hello world"
   * message.
   */
  SDL_Surface *surface = SDL_LoadBMP( "hello.bmp" );

  /* We create a hardware accelerated renderer.  This is liable to
   * fail on some systems, and again there's no error checking.
   * 
   * We pass in the window we created earlier, and use -1 for the
   * rendering driver to get the first one available.
   *
   * We explicitly ask for hardware redering with
   * SDL_RENDERER_ACCELERATED.
   */
  SDL_Renderer *renderer = 
    SDL_CreateRenderer( window, -1, SDL_RENDERER_ACCELERATED );

  /* We create the texture we want to disaply from the surface we
   * loaded earlier.
   */  
  SDL_Texture *texture = SDL_CreateTextureFromSurface( renderer, surface );

  /* We're now done with the surface, so we free the resources.
   */
  SDL_FreeSurface( surface );
  /* And set the pointer to NULL so it won't accidentally be used for
   * something else.
   */
  surface = NULL;

  SDL_Texture *deerTexture = createTextureFromBMP(renderer, "sprites/Deer_Enemy_Sprite.bmp");
  if (!deerTexture) {
      printf("Failed to load deer texture: %s\n", SDL_GetError());
      return 1;
  }

  SDL_Texture *wallTexture = createTextureFromBMP(renderer, "sprites/Tree_Wall_Texture.bmp");
  if (!wallTexture) {
      printf("Failed to load wall texture: %s\n", SDL_GetError());
      return 1;
  }

  /* The quit flag.  We run the game loop until this flag becomes
   * true.
   */
  SDL_bool quit = SDL_FALSE;
  /* The event we're going to receive.  Initialised to all zeros.
   */
  SDL_Event e = { 0 };

  extern const int MAP[20][20];
  int x, y;
  float fx, fy;
  float playerAngle;
  int playerFov = 90;

  const DB_SIZE = screenWidth;

  float depthBuffer[DB_SIZE];

  /* Here is a very simple game loop, with a soft framerate.  There is
   * a chance an individual frame will take longer, and this happens
   * predictably when the user presses the X button to close the
   * window, and the delay routine can oversleep because of operating
   * system scheduling.
   */
  bool isMoving = false;
  bool isFiring = false;
  float bobbingOffset = 0.0f;
  float bobbingSpeed = 0.01f; // Slower bobbing speed
  float bobbingAmplitude = -5.0f; // Move down then back up
  float spriteX = 5.0f;
  float spriteY = 5.0f;

  // Send the player position to the server
  static float lastFx = 0.0f, lastFy = 0.0f, lastPlayerAngle = 0.0f;
  static bool lastIsFiring = false;
  static Uint32 lastUpdateTime = 0;
    int textureWidth = 24 * 10;
    int textureHeight = 62 * 10;

  // SDL_Surface* wallSurface = SDL_CreateRGBSurfaceWithFormat(0, textureWidth, textureHeight, 32, SDL_PIXELFORMAT_RGBA32);
  // if (!wallSurface) {
  //     printf("Failed to create surface: %s\n", SDL_GetError());
  //     return 1;
  // }

  SDL_SetRenderTarget(renderer, wallTexture);

  Uint32 *pixels;

  // Define the rectangle for the texture
  SDL_Rect rect = {0, 0, textureWidth, textureHeight};

  // Allocate memory for pixels
  pixels = (Uint32*)malloc(rect.w * rect.h * sizeof(Uint32));

  

  if (!pixels) {
      printf("Failed to allocate memory for pixels\n");
      return 1;
  }

  // Render the texture with transformations (rotation, flipping, translation)
  SDL_RenderCopyEx(renderer, wallTexture, NULL, &rect, 0, NULL, SDL_FLIP_NONE);

  // Read the pixels from the renderer
  if (SDL_RenderReadPixels(renderer, &rect, SDL_PIXELFORMAT_RGBA32, pixels, rect.w * sizeof(Uint32)) != 0) {
      printf("Failed to read pixels from renderer: %s\n", SDL_GetError());
      free(pixels); // Free the allocated memory
      return 1;
  }

  SDL_Color *colorBuffer = (SDL_Color*)malloc(rect.w * rect.h * sizeof(SDL_Color));

if (!colorBuffer) {
    printf("Failed to allocate memory for color buffer\n");
    free(pixels); // Free the allocated memory for pixels
    return 1;
}

// Populate the color buffer with SDL_Colors
for (int y = 0; y < rect.h; y++) {
    for (int x = 0; x < rect.w; x++) {
        Uint32 pixel = pixels[y * rect.w + x];
        SDL_Color color;
        SDL_GetRGBA(pixel, SDL_AllocFormat(SDL_PIXELFORMAT_RGBA32), &color.r, &color.g, &color.b, &color.a);
        colorBuffer[y * rect.w + x] = color;
    }
}



  while ( !quit ) {
    Uint32 start = SDL_GetTicks();

    const Uint8 *state = SDL_GetKeyboardState(NULL);
    while ( SDL_PollEvent( &e ) ) {
      if ( e.type == SDL_QUIT ) {
        quit = SDL_TRUE;
      }
    }

    Uint32 currentTime = SDL_GetTicks();

    isMoving = state[SDL_SCANCODE_W] || state[SDL_SCANCODE_A] || state[SDL_SCANCODE_S] || state[SDL_SCANCODE_D];

    if (state[SDL_SCANCODE_W]) {
      moveForward();
    }
    if (state[SDL_SCANCODE_A]) {
      moveLeft();
    }
    if (state[SDL_SCANCODE_S]) {
      moveBackward();
    }
    if (state[SDL_SCANCODE_D]) {
      moveRight();
    }
    if (state[SDL_SCANCODE_Q]) {
      rotateLeft();
    }
    if (state[SDL_SCANCODE_E]) {
      rotateRight();
    }
    if (state[SDL_SCANCODE_SPACE]) {
      isFiring = true;
    }

    SDL_RenderClear( renderer );

    int segmentWidth = screenWidth / DB_SIZE;
    for (int i = 0; i < DB_SIZE; i++) {
        SDL_Color topBotColor = { 128, 128, 128, 255 };
        int screenXStart = i * segmentWidth;
        int screenXEnd = (i + 1) * segmentWidth;
        float blendFactor = depthBuffer[i] / 20.0f;
        if (blendFactor < 0.0f) blendFactor = 0.0f;
        if (blendFactor > 1.0f) blendFactor = 1.0f;
        SDL_Color middleColor = {
            (Uint8)(0 * (1.0f - blendFactor) + topBotColor.r * blendFactor),
            (Uint8)(128 * (1.0f - blendFactor) + topBotColor.g * blendFactor),
            (Uint8)(0 * (1.0f - blendFactor) + topBotColor.b * blendFactor),
            255
        };    

        drawColumn(renderer, colorBuffer, i, screenHeight, depthBuffer[i], topBotColor, wallTexture, i);
    }

    getPlayerPosition(&x, &y, &fx, &fy, &playerAngle);



    // Send the player position to the server
    if ((fx != lastFx || fy != lastFy || playerAngle != lastPlayerAngle || isFiring != lastIsFiring) && (currentTime - lastUpdateTime >= 100)) {
        fd_set writefds;
        struct timeval tv;
        FD_ZERO(&writefds);
        FD_SET(socket_fd, &writefds);
        tv.tv_sec = 0;
        tv.tv_usec = 0;

        int result = select(socket_fd + 1, NULL, &writefds, NULL, &tv);
        if (result > 0 && FD_ISSET(socket_fd, &writefds)) {
            createAndSendUpdateMessage(socket_fd, fx, fy, playerAngle, isFiring);
            // Read update messages from the server
            readUpdateMessages(socket_fd);
            lastFx = fx;
            lastFy = fy;
            lastPlayerAngle = playerAngle;
            lastIsFiring = isFiring;
            lastUpdateTime = currentTime;
        }
    }

    // Recieve the player position from the server and set the sprites to the x and y values of them


    float spriteDir = atan2(spriteY - fy, spriteX - fx) - playerAngle;
    while (spriteDir > M_PI) spriteDir -= 2 * M_PI;
    while (spriteDir < -M_PI) spriteDir += 2 * M_PI;

    float spriteFov = 35.0f * (M_PI / 180.0f); // 35 degrees in radians

    // Move the sprite towards the player at 0.1f
    float directionX = fx - spriteX;
    float directionY = fy - spriteY;

    float length = sqrt(directionX * directionX + directionY * directionY);
    if (length != 0) {
        directionX /= length;
        directionY /= length;
    }

    static float initialSpriteX = 0.0f;
    static float initialSpriteY = 0.0f;

    if (length > 2.0f) {
        spriteX += directionX * 0.01f;
        spriteY += directionY * 0.01f;
    } else {
        // Move sideways instead of forwards
        spriteX += directionY * 0.005f;
        spriteY -= directionX * 0.005f;
    }

    if (fabs(spriteDir) < spriteFov) {
        float depth = sqrt((spriteX - fx) * (spriteX - fx) + (spriteY - fy) * (spriteY - fy));
        int spriteScreenSize = (int)(screenHeight / depth);
        int spriteScreenX = (int)((screenWidth / 2) * (1 + spriteDir / spriteFov));

        int hOffset = spriteScreenX - spriteScreenSize / 2;
        int vOffset = screenHeight / 2 - spriteScreenSize / 2;
        SDL_Rect destRect = { hOffset, vOffset, spriteScreenSize, spriteScreenSize };
        SDL_RenderCopy(renderer, deerTexture, NULL, &destRect);

        for (int j = hOffset; j < hOffset + spriteScreenSize; j++) {
            if (j >= 0 && j < screenWidth && depthBuffer[j] < depth) {
                SDL_Color topBotColor = { 128, 128, 128, 255 };
                float blendFactor = depthBuffer[j] / 20.0f;
                if (blendFactor < 0.0f) blendFactor = 0.0f;
                if (blendFactor > 1.0f) blendFactor = 1.0f;
                SDL_Color middleColor = {
                    (Uint8)(0 * (1.0f - blendFactor) + topBotColor.r * blendFactor),
                    (Uint8)(128 * (1.0f - blendFactor) + topBotColor.g * blendFactor),
                    (Uint8)(0 * (1.0f - blendFactor) + topBotColor.b * blendFactor),
                    255
                };
                drawColumn(renderer, colorBuffer, j, screenHeight, depthBuffer[j], topBotColor, wallTexture, j);
            }
        }
    }

    drawMiniMap(renderer, MAP, fx, fy, x, y, playerAngle, playerFov, 10, 20, 20, screenWidth, screenHeight, depthBuffer);

    SDL_Texture *gunTexture = createTextureFromBMP(renderer, "sprites/Rifle_GUI_Sprite.bmp");
    if (gunTexture == NULL) {
        printf("Failed to load gun texture: %s\n", SDL_GetError());
    } else {
        if (isMoving) {
            bobbingOffset = bobbingAmplitude * sin(SDL_GetTicks() * bobbingSpeed);
        } else {
            bobbingOffset = 0.0f;
        }

        int gunWidth = 600;
        int gunHeight = 600;
        int gunX = screenWidth - gunWidth;
        int gunY = screenHeight - gunHeight + (int)bobbingOffset + 10;

        SDL_Rect gunRect = { gunX, gunY, gunWidth, gunHeight };
        SDL_RenderCopy(renderer, gunTexture, NULL, &gunRect);

        SDL_DestroyTexture(gunTexture);
    }

    //drawPlayer(renderer, x, y, playerAngle, (SDL_Color){255, 0, 0, 255});

    /* Then copy the texture we created on to the entire screen.
     *
     * That's what the NULL, NULL means: use the entire texture on the
     * entire screen.
     */
    SDL_RenderCopy( renderer, texture, NULL, NULL );
    /* And finally tell the renderer to display on screen what we've
     * drawn so far.
     */
    SDL_RenderPresent( renderer );

    /* We take a note of the time at the end of our game loop, so we
     * can calculate below how long it took.
     */
    Uint32 end = SDL_GetTicks();

    /* We subtract end - start to get the time spent in our game loop.
     * This is very basic clock usage.  We then calculate how long we
     * should wait in milliseconds given the current framerate f, and
     * subtract again from that our previous results, to get the time
     * we should spend waiting.
     * 
     * If end - start is larger than 1000 / f, this will turn
     * negative; in which case we don't want to wait at all.
     */
    int wait_for = 1000 / f - ( end - start );

    /* If we calculated positive milliseconds to wait_for, we wait.
     */
    if ( wait_for > 0 ) {
      SDL_Delay( wait_for );
    }
  }

  /* Clean up after ourselves before we quit.
   */
  SDL_DestroyTexture( texture );
  SDL_DestroyTexture( deerTexture );
  SDL_DestroyTexture( wallTexture );
  SDL_DestroyRenderer( renderer );
  SDL_DestroyWindow( window );
  SDL_Quit();

  /* Close the socket connection */
  close_socket(socket_fd);

  return 0;
}
