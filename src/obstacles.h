#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "raylib.h"

#define MAX_ISLAND_RADIUS 24
#define MAX_ISLANDS 15
#define MIN_ISLANDS 5

#define MAX_ROCK_HEIGHT 20
#define MAX_ROCKS 12
#define MIN_ROCKS 4

typedef struct {
    int radius;
    Vector3 center_pos;
    Texture2D sand_tex;
    //https://sketchfab.com/3d-models/low-poly-palm-tree-58f448209beb43659e95ca0e1ad59ac2
    Model palm_tree;
    Model island_sphere;
} Island;

typedef struct {
    int height;
    Vector3 center_pos;
    Vector3 rotation_vec;
    Model model;
    Texture2D rock_tex;
    int geometry_id;
} Rock;

typedef struct {
    Island* island_list;
    int island_count;
    Rock* rock_list;
    int rock_count;
} Obstacles;


Island CreateIsland(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound);
Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound, int island_count);

Rock CreateRock(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound);
Rock* CreateAllRocks(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound, int rock_count);

Obstacles CreateObstactlesInstance(Island* island_list, int island_count, Rock* rock_list, int rock_count);

#endif //OBSTACLES_H  
