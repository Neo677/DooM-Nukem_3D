

ifeq ($(OS),Windows_NT)
	SDL_PLATFORM_DIR=win_sdl2
	OUTPUT=game.exe
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S), linux)
	  	SDL_PLATFORM_DIR=linux_sdl2
	endif
	ifeq ($(UNAME_S), Darwin)
		SDL_PLATFORM_DIR=macos_sdl2
	endif
	OUTPUT=game
endif

SRC=*.c
INCLUDE=./include
LIB=./lib

all:
	gcc $(SRC) -I$(INCLUDE) -L./lib -lSDL2 -lSDL2main -o $(OUTPUT) -framework Cocoa -framework IOKit -framework CoreVideo -arch x86_64

clean: 
	rm $(OUTPUT)

run:
	./$(OUTPUT)