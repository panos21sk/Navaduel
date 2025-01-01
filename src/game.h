#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"
#include "obstacles.h"

#define MOVEMENT_TIME 6 //time allowed for the current ship to move (turn-based gm, in seconds)
#define FIRE_TIME 3 //time allowed for the current ship to fire (turn-based gm, in seconds)

extern int winner;
extern int startup_counter;
extern int dice_state;
extern int move_time;
extern int fire_time;
extern Ship *current_turn;
extern Ship *next_turn;
extern bool is_loaded;
extern BoundingBox game_bounds;

void DisplayRealTimeGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, Sound* game_sounds, Texture2D* game_textures);
void DisplayTurnBasedGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, Sound* game_sounds, Texture2D* game_textures);
void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles, Model* game_models, Ship current_player_ship, Texture2D* game_textures);
void Update_Variables(Ship_data ship_data, Sound explosion, Obstacles obstacles);

#endif // GAME_H
