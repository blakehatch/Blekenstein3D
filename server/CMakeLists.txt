# CMakeLists.txt for the server

cmake_minimum_required(VERSION 3.10)
project(GameServer)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)

# Include directories
include_directories(${CMAKE_SOURCE_DIR})

# Add the server executable
add_executable(game_server
    tcp-game-server.cpp
    game-state.cpp
    player.cpp
)

# Link pthread and other necessary libraries to the server
target_link_libraries(game_server
    pthread
)