#include "obstacles.h"
#include "game.h"
#include <time.h>

Island CreateIsland(Texture2D sand_tex, Model palm_tree, Vector2 corner_bound, Vector2 opp_corner_bound){
    Island island_instance;
    SetRandomSeed(time(NULL)); //seed is unix time
    island_instance.radius = GetRandomValue(0, MAX_ISLAND_RADIUS);
    island_instance.center_pos = (Vector3){
        GetRandomValue(corner_bound.x, opp_corner_bound.x),
        -GetRandomValue(island_instance.radius / 8, island_instance.radius / 1.2), //island should hover a bit under the water, depending on radius
        GetRandomValue(corner_bound.y, opp_corner_bound.y)
    };
    island_instance.sand_tex = sand_tex;
    island_instance.palm_tree = palm_tree;
    island_instance.island_sphere = LoadModelFromMesh(GenMeshSphere(island_instance.radius, 1, 1));
    island_instance.island_sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand_tex;
    return island_instance;
}

Island* CreateAllIslands(Texture2D sand_tex, Model toppings, Vector2 corner_bound, Vector2 opp_corner_bound){
    SetRandomSeed(time(NULL));
    const int island_count = GetRandomValue(MIN_ISLANDS, MAX_ISLANDS);
    static Island island_list[MAX_ISLANDS];
    for(int i = 0; i < island_count; i++){
        island_list[i] = CreateIsland(sand_tex, toppings, corner_bound, opp_corner_bound);
    }
    return island_list;
}
