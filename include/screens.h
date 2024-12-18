#include <raylib.h>

#ifndef SCREEN_H
#define SCREEN_H

typedef enum {
    MAIN, OPTIONS, GAME, GAME_OVER
} screen;
//Run on startup, once.
void handle_screen(screen screen_name){
    switch(screen_name){
        case MAIN:{
            DrawText("NAVADUEL", 20, 20, 40, LIGHTGRAY);
        };
        default: break;
    }
};

#endif