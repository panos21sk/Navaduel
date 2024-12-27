#ifndef SCREENS_H
#define SCREENS_H

#include "raylib.h"
#include "ship.h"
#include <setjmp.h>
#ifndef bool
#include <stdbool.h>
#endif //bool
#define WIDTH 1280
#define HEIGHT 700
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

extern screen gamemode; // current gamemode
extern screen current_screen;
extern Rectangle play_button;
extern int winner;
extern int success_save;

typedef struct {
    bool show_reticle;
    bool first_or_third_person_cam;
    bool fullscreen;
    bool enable_sfx;
    bool enable_bgm;
} setting;
extern setting settings;

void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen(Sound click);
void DisplayGamemodesScreen(Sound click);
void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model, Sound splash, Sound fire);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Model water_model, Model sky_model, Sound splash, Sound fire);
void DisplayGameOverScreen(int winnerId, Sound click);
void DisplayOptionsScreen(Sound click, bool* bgm_en);
void DisplayControlsScreen(Sound click);
void DisplayAboutScreen(Sound click);
void DisplayGameMenuScreen(Sound click);

#endif //SCREENS_H
