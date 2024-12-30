#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"
#include "obstacles.h"

#define MOVEMENT_TIME 10 //time allowed for the current ship to move (turn-based gm, in seconds)
#define FIRE_TIME 5 //time allowed for the current ship to fire (turn-based gm, in seconds)

extern int winner;
extern int startup_counter;
extern int dice_state;
extern int move_time;
extern int fire_time;
extern Ship *current_turn;
extern Ship *next_turn;
extern bool is_loaded;
extern BoundingBox game_bounds;

void DisplayRealTimeGameScreen(Ship_data ship_data, Obstacles obstacles,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DisplayTurnBasedGameScreen(Ship_data ship_data, Obstacles obstacles,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles,
        Model water_model, Model sky_model, Ship current_player_ship, Texture2D heart_full, Texture2D heart_empty);
void Update_Variables(Ship_data ship_data, Sound explosion, Obstacles obstacles);

#endif // GAME_H
