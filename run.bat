gcc src/main.c -o main.exe -O1 -Wall -std=c17 -Wno-missing-braces -I include/ -L lib/ -lraylib -lopengl32 -lgdi32 -lwinmm
main.exe