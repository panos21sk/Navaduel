#ifndef OBSTACLES_H
#define OBSTACLES_H

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/* Defines the islands sphere's radius */
#define MAX_ISLAND_RADIUS 24
/* Defines the maximum number of generated Islands */
#define MAX_ISLANDS 15
/* Defines the minimum number of generated Islands */
#define MIN_ISLANDS 5

/* Defines the maximum height of a Rock */
#define MAX_ROCK_HEIGHT 20
/* Defines the maximum number of generated Rocks */
#define MAX_ROCKS 12
/* Defines the minimum number of generated Rocks */
#define MIN_ROCKS 4

/* Island structure */
typedef struct {
    int radius;
    Vector3 center_pos;
    Texture2D sand_tex;
    //https://sketchfab.com/3d-models/low-poly-palm-tree-58f448209beb43659e95ca0e1ad59ac2
    Model palm_tree;
    Model island_sphere;
} Island;

/* Rock structure */
typedef struct {
    int height;
    int model_coefficient;
    Vector3 center_pos;
    Vector3 rotation_vec;
    Model model;
    Texture2D rock_tex;
    int geometry_id;
} Rock;

/* Obstacles structure */
typedef struct {
    Island* island_list;
    int island_count;
    Rock* rock_list;
    int rock_count;
} Obstacles;

/* Function declarations */
Island CreateIsland(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound);
Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound, int island_count);

Rock CreateRock(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound);
Rock* CreateAllRocks(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound, int rock_count);

Obstacles CreateObstactlesInstance(Island* island_list, int island_count, Rock* rock_list, int rock_count);

Obstacles init_obs(Texture2D sand_tex, Texture2D rock_tex, Model palm_tree);

#endif //OBSTACLES_H  
