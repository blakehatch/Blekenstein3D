# Blekenstein3D

## A Multiplayer 90s-Style Pseudo-3D FPS

**Inspired by the great John Carmack!**

Game Client is implemented in C and Game Server is implemented in C++.

## Instructions for Running the Game

1. Open a terminal and navigate to the `server` directory.
2. Run the following command to compile the game server:
   ```
   make
   ```
   This will generate the `game_server` executable.

3. Open another terminal and navigate to the main directory.
4. Run the following command to compile the game client:
   ```
   make
   ```
   This will generate the `blek_3d` executable.

5. In the terminal where you compiled the server, start the game server by running:
   ```
   ./game_server
   ```

6. In the terminal where you compiled the client, start the game client by running:
   ```
   ./blek_3d
   ```

By following these steps, you will have the game server running in one terminal and the game client running in another terminal, allowing you to start the game.

## External Dependencies

The only external dependency required to run this game is SDL2. You can find the installation instructions for SDL2 at the following link:

[SDL2 Installation Instructions](https://wiki.libsdl.org/SDL2/Installation)

