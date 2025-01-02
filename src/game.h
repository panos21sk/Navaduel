#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"
#include "obstacles.h"
#include "anim.h"

#define MOVEMENT_TIME 6 //time allowed for the current ship to move (turn-based gm, in seconds)
#define FIRE_TIME 3 //time allowed for the current ship to fire (turn-based gm, in seconds)

extern int winner;
extern int startup_counter;
extern int dice_state;
extern int reset_state;
extern int move_time;
extern int fire_time;
extern Ship *current_turn;
extern Ship *next_turn;
extern bool is_loaded;
extern bool has_fired_once;
extern BoundingBox game_bounds;

void DisplayRealTimeGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list);
void DisplayTurnBasedGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list);
void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles, 
                        Model* game_models, Ship current_player_ship, Texture2D* game_textures, Animation* anim_list);
void UpdateVariables(Ship_data ship_data, Sound explosion, Obstacles obstacles, Animation* explosion_anim);

#endif // GAME_H
