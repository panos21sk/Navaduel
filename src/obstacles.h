#ifndef OBSTACLES_H
#define OBSTACLES_H

#include "raylib.h"

typedef struct {
    int radius;
    Vector3 center_pos;
    Texture2D sand_tex;
    //https://sketchfab.com/3d-models/low-poly-palm-tree-58f448209beb43659e95ca0e1ad59ac2
    Model palm_tree;
    Model island_sphere;
} Island;

Island CreateIsland(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound);
Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound, int island_count);

#endif //OBSTACLES_H
