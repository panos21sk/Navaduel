#include <raylib.h>
#include <state.h>

#ifndef GAME_H
#define GAME_H

void game(state current_state){  
    //Iniatializing Models for rendering
    static Model ship = LoadModel("../../resources/models/ship2.glb");
    static Texture2D water_tex = LoadTexture("../../resources/sprites/water.png");
    static Mesh water_cube = GenMeshCube(300, 1, 300);
    static Model water_model = LoadModelFromMesh(water_cube);
    water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex;


    //! Input Handling:


    //Vector3 CurrentCamPos = current_state.camera1.position; // Store old camera1 position
    //Vector3 oldShip1Pos = current_state.ship1pos;

    UpdateCameraPro(&current_state.camera1, (Vector3){0, 0, 0}, (Vector3){0, 0, 0}, 0);

    //! Rendering:
    BeginDrawing();
        ClearBackground(RAYWHITE);
        BeginMode3D(current_state.camera1);

            // Draw the ship
            DrawModel(ship, current_state.ship1pos, 1.0f, WHITE);
            //Draw water surface as a thin cube mesh
            DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
        EndMode3D();
    // DrawRectangleRec((Rectangle){10, 10, 10, 10}, BLUE); Displays rectangle outside of 3d view
    EndDrawing();
}

#endif