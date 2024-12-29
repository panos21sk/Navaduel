#include "obstacles.h"
#include "game.h"

Island CreateIsland(Texture2D sand_tex, Model palm_tree, Vector2 corner_bound, Vector2 opp_corner_bound){
    Island island_instance;
    island_instance.radius = GetRandomValue(MAX_ISLAND_RADIUS / 2, MAX_ISLAND_RADIUS);
    island_instance.center_pos = (Vector3){
        GetRandomValue(corner_bound.x, opp_corner_bound.x),
        GetRandomValue(-island_instance.radius / 3, -island_instance.radius/10),
        GetRandomValue(corner_bound.y, opp_corner_bound.y)
    };
    island_instance.sand_tex = sand_tex;
    island_instance.palm_tree = palm_tree;
    Mesh sphere_mesh = GenMeshSphere(island_instance.radius, 128, 128);
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