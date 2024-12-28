#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"
#include "obstacles.h"

#define MAX_ISLAND_RADIUS 10
#define MAX_ISLANDS 7
#define MIN_ISLANDS 2

extern int winner;
extern int startup_counter;
extern bool is_loaded;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Island* island_list,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Island* island_list,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DrawGameState(Ship ship1, Ship ship2, Camera camera, RenderTexture screenShip, Island* island_list,
        Model water_model, Model sky_model, Ship current_player_ship, Texture2D heart_full, Texture2D heart_empty);
void Update_Variables(Ship* ship1, Ship* ship2, Sound explosion, Island* island_list);

#endif // GAME_H
