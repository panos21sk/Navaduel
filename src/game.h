#ifndef GAME_H
#define GAME_H

#include "raylib.h"
#include "ship.h"

#define MAX_ISLAND_RADIUS 10
#define MAX_ISLANDS 7
#define MIN_ISLANDS 2

extern int winner;
extern int startup_counter;
extern bool is_loaded;

typedef struct {
        float radius;
        Vector3 cetner_pos; 
        Texture2D sand_tex;
        //https://sketchfab.com/3d-models/low-poly-palm-tree-58f448209beb43659e95ca0e1ad59ac2
        Model palm_tree;
        Model island_sphere;
} Island;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Island* island_list,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Island* island_list,
        Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty);
void DrawGameState(Ship ship1, Ship ship2, Camera camera, RenderTexture screenShip, Island* island_list,
        Model water_model, Model sky_model, Ship current_player_ship, Texture2D heart_full, Texture2D heart_empty);
void Update_Variables(Ship* ship1, Ship* ship2, Sound explosion, Island* island_list);
Island CreateIsland(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound);
Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound);

#endif // GAME_H
