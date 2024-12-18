#include "raylib.h"

#ifndef MAIN_SCREEN_H
#define MAIN_SCREEN_H

void render_main_menu(){
    BeginDrawing();
        DrawText("NAVADUEL", 20, 20, 30, BLUE);
    EndDrawing();
}

#endif