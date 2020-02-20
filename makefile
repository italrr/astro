CC=g++
CFLAGS = -std=c++11
OBJ = src/main.cpp src/engine/Graphics.cpp src/engine/Log.cpp
LIBS = -lglfw3 -pthread -framework Cocoa -framework OpenGL -framework IOKit -framework CoreVideo
LDIR = ./

%.o: %.c
	$(CC) -c -o $@ $< $(CFLAGS) -L$(LDIR)

astro: $(OBJ)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS) -L$(LDIR)

.PHONY: clean

clean:
	rm -rf ./astro