gcc src/game.c src/util.c src/screens.c src/ship.c src/main.c -o main.exe -O1 -Wall -std=c17 -Wno-missing-braces -I include/ -L lib/ -lini -lraylib -lopengl32 -lgdi32 -lwinmm
main.exe