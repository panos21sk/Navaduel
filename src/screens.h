#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#ifndef bool
#include <stdbool.h>
#endif //bool
#define DISPLAY GetCurrentMonitor()
#define WIDTH 1600
#define HEIGHT 700 //setting to 700 cuz shit doesnt fit to my screen T-T
#define GAME_STARTUP_COUNTER 3 // in seconds
#define CAMERA_DISTANCE_VECTOR_TP (Vector3){0.0f, 25.0f, -50.0f}
#define CAMERA_DISTANCE_VECTOR_FP (Vector3){0.0f, 15.0f, 25.0f}

typedef enum
{
    MAIN, // entry screen
    OPTIONS, // options
    CONTROLS, // controls
    GAME_REAL, // main game screen (real-time)
    GAME_TURN, // main game screen (turn-based)
    GAME_OVER, // game over screen - when a ship gets destroyed
    ABOUT, // credits and basic gameplay
    GAME_MENU, // pressing esc in-game
    GAMEMODES // choosing between real-time gameplay and turn-based gameplay
} screen;

extern int success_save;
extern int success_load;
extern bool exit_window;
extern screen gamemode; // current gamemode
extern screen current_screen;
extern Rectangle play_button;
extern RenderTexture screenShip1;
extern RenderTexture screenShip2;
extern RenderTexture screenCurrentShip;

void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen(Sound click);
void DisplayGamemodesScreen(Sound click);
void DisplayGameOverScreen(int winnerId, Sound click);
void DisplayOptionsScreen(Sound click, bool* bgm_en);
void DisplayControlsScreen(Sound click);
void DisplayAboutScreen(Sound click);
void DisplayGameMenuScreen(Sound click);

#endif //SCREENS_H
