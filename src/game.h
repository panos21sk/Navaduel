#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"

extern int winner;
extern int startup_counter;
extern bool is_loaded;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, 
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, 
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DrawGameState(Ship ship1, Ship ship2, Camera camera, RenderTexture screenShip, 
        Model water_model, Model sky_model, Ship current_player_ship, Texture2D heart_full, Texture2D heart_empty);
void Update_Variables(Ship* ship1, Ship* ship2, Sound explosion);

#endif //GAME_H
