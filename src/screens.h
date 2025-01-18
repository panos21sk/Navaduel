#ifndef SCREENS_H
#define SCREENS_H

/* Import the required game headers (first party libraries) */
#include "obstacles.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/* Import the required tool headers (third party libraries) */
#ifndef bool
#include <stdbool.h>
#endif //bool

/* Defines the width of the Main window */
#define WIDTH 1920
/* Defines the height of the Main window */
#define HEIGHT 1080 //setting to 700 cuz shit doesnt fit to my screen T-T
/* Defines the start-up counter time, in seconds */
#define GAME_STARTUP_COUNTER 3

/* screen structure */
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
    GAMEMODES, // choosing between real-time gameplay and turn-based gameplay
    SHIP_SELECT, // ship selection screen for all players
    TEAM_SELECT // ship selection screen for all players
} screen;

/* Variable declarations */
extern int success_save; //Determines if a game save procedure is success or not
extern int success_load; //Determines if a game load procedure is success or not
extern bool exit_window; //Determines if the game should close or not
extern screen gamemode; //Declares the current gamemode
extern screen current_screen; //Declares the current screen
extern RenderTexture screenShip1; //The screen for Player 1 in real-time gamemode
extern RenderTexture screenShip2; //The screen for Player 2 in real-time gamemode
extern RenderTexture screenCurrentShip; //The screen for the current Player in turn-based gamemode

/* Function declarations */
void InitMainWindow();
void DeinitMainWindow();
void DisplayMainScreen(Sound click, Obstacles *obstacles, Texture2D sand_tex, Model palm_tree, Texture2D rock_tex);
void DisplayGamemodesScreen(Sound click, int* player_count_addr, char* real_or_turn_addr);
void DisplayShipSelectScreen(Sound click, int* type_list, int player_count, char real_or_turn);
void DisplayTeamSelectScreen(Sound click, int* team_list, int player_count, char real_or_turn);
void DisplayGameOverScreen(char* wintext, Sound click);
void DisplayOptionsScreen(Sound click, bool* bgm_en);
void DisplayControlsScreen(Sound click);
void DisplayAboutScreen(Sound click);
void DisplayGameMenuScreen(Sound click, Obstacles obstacles);

#endif //SCREENS_H
