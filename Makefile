CC = clang
CFLAGS = -Wall -Wextra -std=c11 -I/Library/Frameworks/SDL2.framework/Headers -I/Library/Frameworks/SDL2_ttf.framework/Headers -F/Library/Frameworks -framework SDL2 -framework SDL2_ttf -fsanitize=address -g -ffast-math
LDFLAGS = -rpath /Library/Frameworks -framework SDL2_ttf
TARGET = blek_3d

all: $(TARGET)

$(TARGET): main.o draw.o game.o client.o
	$(CC) $(CFLAGS) $(LDFLAGS) -o $(TARGET) main.o draw.o game.o client.o
main.o: main.c
	$(CC) $(CFLAGS) -c main.c
draw.o: draw.c
	$(CC) $(CFLAGS) -c draw.c
game.o: game.c
	$(CC) $(CFLAGS) -c game.c
client.o: client.c
	$(CC) $(CFLAGS) -c client.c

clean:
	rm -f $(TARGET) *.o
