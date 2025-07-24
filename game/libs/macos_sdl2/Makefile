

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
	gcc $(SRC) -I$(INCLUDE) -L$(LIB) -lSDL2 -o $(OUTPUT) -arch arm64

clean: 
	rm $(OUTPUT)

run:
	./$(OUTPUT)