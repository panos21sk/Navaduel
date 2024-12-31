#include "obstacles.h"
#include "game.h"
#include "raymath.h"

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

Rock CreateRock(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound){
    Rock rock_instance;
    rock_instance.height = GetRandomValue(MAX_ROCK_HEIGHT / 3, MAX_ROCK_HEIGHT);
    rock_instance.rotation_vec = (Vector3){
        GetRandomValue(0, 3), GetRandomValue(0, 3), GetRandomValue(0, 3)
    };
    switch (GetRandomValue(1,2))
    {
    case 1:
        rock_instance.model = LoadModelFromMesh(GenMeshCube(
            rock_instance.height / 3 + GetRandomValue(-3, 3) * rock_instance.height / 8,
            rock_instance.height,
            rock_instance.height / 3 + GetRandomValue(-3, 3) * rock_instance.height / 8));
        rock_instance.geometry_id = 1;
        rock_instance.center_pos = (Vector3){
            GetRandomValue(corner_bound.x, opp_corner_bound.x),
            rock_instance.height / 2,// GetRandomValue(-rock_instance.height / 3, -rock_instance.height / 8),
            GetRandomValue(corner_bound.y, opp_corner_bound.y)
        };
        break;
    case 2:
        rock_instance.model = LoadModelFromMesh(GenMeshSphere(rock_instance.height, 64, 64));
        rock_instance.geometry_id = 2;
        rock_instance.center_pos = (Vector3){
            GetRandomValue(corner_bound.x, opp_corner_bound.x),
            GetRandomValue(-1, -4),
            GetRandomValue(corner_bound.y, opp_corner_bound.y)
        };
        break;
    }
    rock_instance.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = rock_tex;
    rock_instance.model.transform = MatrixRotateXYZ(rock_instance.rotation_vec);
    return rock_instance;
}

Rock* CreateAllRocks(Texture2D rock_tex, Vector2 corner_bound, Vector2 opp_corner_bound, int rock_count){
    static Rock rock_list[MAX_ROCKS];
    for(int i = 0; i < rock_count; i++){
        rock_list[i] = CreateRock(rock_tex, corner_bound, opp_corner_bound);
    }
    return rock_list;
}

Obstacles CreateObstactlesInstance(Island* island_list, int island_count, Rock* rock_list, int rock_count){
    Obstacles obj_inst;
    obj_inst.island_count = island_count;
    obj_inst.rock_count = rock_count;
    obj_inst.island_list = island_list;
    obj_inst.rock_list = rock_list;
    return obj_inst;
}