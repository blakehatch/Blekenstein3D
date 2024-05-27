# Makefile for tcp-echo-server

CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -pedantic -ggdb
LDFLAGS = -rpath /Library/Frameworks -lpthread
EXECUTABLE = game_server

all: $(EXECUTABLE)

$(EXECUTABLE): tcp-game-server.o game-state.o player.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(LDFLAGS) -o $(EXECUTABLE) tcp-game-server.o game-state.o player.o

tcp-game-server.o: tcp-game-server.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c tcp-game-server.cpp

game-state.o: game-state.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c game-state.cpp

player.o: player.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c player.cpp

clean:
	rm -f $(EXECUTABLE) *.o

.PHONY: all clean