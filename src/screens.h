#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "ship.h"
#ifndef bool
#include <stdbool.h>
#endif //bool
#define WIDTH 1280
#define HEIGHT 700
#define GAME_STARTUP_COUNTER 5 // in seconds
#define CAMERA_DISTANCE_VECTOR_TP (Vector3){0.0f, 25.0f, -50.0f}
#define CAMERA_DISTANCE_VECTOR_FP (Vector3){0.0f, 15.0f, 25.0f}

typedef enum
{
    MAIN, // entry screen
    OPTIONS, // options
    GAME_REAL, // main game screen (real-time)
    GAME_TURN, // main game screen (turn-based)
    GAME_OVER, // game over screen - when a ship gets destroyed
    ABOUT, // credits and basic gameplay
    GAME_MENU, // pressing esc in-game
    GAMEMODES // choosing between real-time gameplay and turn-based gameplay
} screen;

extern int gamemode; // 0 for real-time, 1 for turn-based
extern screen current_screen;
extern Rectangle play_button;

typedef struct {
    bool show_reticle;
    bool first_or_third_person_cam;
} setting;
extern setting settings;

void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen();
void DisplayGamemodesScreen();
void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model);
void DisplayGameOverScreen();
void DisplayOptionsScreen();
void DisplayAboutScreen();
void DisplayGameMenuScreen();

#endif //SCREENS_H
