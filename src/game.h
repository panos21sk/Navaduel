#ifndef GAME_H
#define GAME_H

/* Import the required game headers (first party libraries) */
#include "ship.h"
#include "obstacles.h"
#include "anim.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/* Defines time allowed for the current ship to move (turn-based gamemode, in seconds) */
#define MOVEMENT_TIME 4
/* Defines time allowed for the current ship to fire (turn-based gamemode, in seconds) */
#define FIRE_TIME 4

/* Variable declarations */
extern char* wintext; //Text which declares the winner of a game.
extern int startup_counter; //The start-up counter time, defined in seconds.
extern int dice_state; //The "dice" state. Essentially, if 1 then it declares the first player in turn-based games.
extern int reset_state; //If 1, it resets some game variables to default. Used in turn-based games per turn.
extern int move_time; //The current move time for ships in turn-based games, defined in seconds.
extern int fire_time; //The current fire time for ships in turn-based games, defined in seconds.
extern Ship *current_turn; //The current ship playing in turn-based games.
extern bool is_loaded; //Declares if a game is loaded from a saved game state or not.
extern bool has_fired_once; //Declares if the current ship playing in turn-based games has fired, during its firing session.
extern BoundingBox game_bounds; //The map bounds.

/* Function declarations */
void DisplayRealTimeGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list, Texture2D* water_textures);
void DisplayTurnBasedGameScreen(Ship_data ship_data, Obstacles obstacles, Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list, Texture2D* water_textures);
void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles, char real_or_turn,
                        const Model* game_models, Ship current_player_ship, Texture2D* game_textures, const Animation* anim_list, const Texture2D* water_textures);
void DrawUI(Ship current_player_ship, const Texture2D* game_textures, RenderTexture screenShip);
void UpdateVariables(Ship_data ship_data, Sound explosion, Obstacles obstacles, Animation* explosion_anim);
void *DecreaseTime(void *arg);
void *DecreaseCounter(void *arg);

#endif // GAME_H
