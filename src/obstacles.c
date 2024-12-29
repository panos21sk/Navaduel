#include "obstacles.h"
#include "game.h"
#include <time.h>
#include <util.h>
#include <stdlib.h>

Island CreateIsland(Texture2D sand_tex, Model palm_tree, Vector2 corner_bound, Vector2 opp_corner_bound){
    Island island_instance;
    island_instance.radius = GenRandomNumBounded(MAX_ISLAND_RADIUS, MAX_ISLAND_RADIUS / 5);
    island_instance.center_pos = (Vector3){
        GenRandomNumBounded(corner_bound.x, opp_corner_bound.x),
        -GenRandomNumBounded(0, (int)(island_instance.radius / 1.5)), //island should hover a bit under the water, depending on radius
        GenRandomNumBounded(corner_bound.y, opp_corner_bound.y)
    };
    island_instance.sand_tex = sand_tex;
    island_instance.palm_tree = palm_tree;
    Mesh sphere_mesh = GenMeshSphere(island_instance.radius, 32, 32);
    island_instance.island_sphere = LoadModelFromMesh(sphere_mesh);
    island_instance.island_sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand_tex;
    return island_instance;
}

Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound, int island_count){
    static Island island_list[MAX_ISLANDS];
    for(int i = 0; i < island_count; i++){
        island_list[i] = CreateIsland(sand_tex, toppings, corner_bound, opp_corner_bound);
    }
    return island_list;
}