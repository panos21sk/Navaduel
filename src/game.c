#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "game.h"
#include "ship.h"
#include "screens.h"
#include "util.h"
#include "obstacles.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

int startup_counter = GAME_STARTUP_COUNTER;
int winner;
bool is_loaded = false;
BoundingBox game_bounds;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, Obstacles obstacles,
        const Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    HideCursor();

    //! Checks collision with bounds (skybox)
    CheckCollisionWithBounds(ship1, game_bounds);
    CheckCollisionWithBounds(ship2, game_bounds);

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1, fire, settings.enable_sfx);
    CheckMovement(ship2, fire, settings.enable_sfx);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball, splash, settings.enable_sfx);
    UpdateCannonballState(&ship2->cannonball, splash, settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    Update_Variables(ship1, ship2, explosion, obstacles);

    DrawGameState(*ship1, *ship2, camera1, screenShip1, obstacles, water_model, sky_model, *ship1, heart_full, heart_empty);
    DrawGameState(*ship1, *ship2, camera2, screenShip2, obstacles, water_model, sky_model, *ship2, heart_full, heart_empty);

    if (startup_counter > 0)
    {
        char *text = malloc(sizeof(char) * 2); // with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH / 4, HEIGHT / 2, 50, WHITE);
            DrawText(text, 3 * WIDTH / 4, HEIGHT / 2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    }
    else if (startup_counter == 0)
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            DrawText("Begin!", 3 * WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; // game starts
        }
        EndDrawing();
        sleep(1); //TODO: Find solution without using sleep
    }
    else
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);
        }
        EndDrawing();
    }
}

void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Obstacles obstacles,
        const Model water_model, const Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    HideCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1, fire, settings.enable_sfx);
    CheckMovement(ship2, fire, settings.enable_sfx);

    // Update Camera manually
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball, splash, settings.enable_sfx);
    UpdateCannonballState(&ship2->cannonball, splash, settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    Update_Variables(ship1, ship2, explosion, obstacles);

    DrawGameState(*ship1, *ship2, camera1, screenShip1, obstacles, water_model, sky_model, *ship1, heart_full, heart_empty);
    DrawGameState(*ship1, *ship2, camera2, screenShip2, obstacles,water_model, sky_model, *ship2, heart_full, heart_empty);

    if (startup_counter > 0)
    {
        char *text = malloc(sizeof(char) * 2); // with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH / 4, HEIGHT / 2, 50, WHITE);
            DrawText(text, 3 * WIDTH / 4, HEIGHT / 2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    }
    else if (startup_counter == 0)
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            DrawText("Begin!", 3 * WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; // game starts
        }
        EndDrawing();
        sleep(1);
    }
    else
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);
        }
        EndDrawing();
    }
}

void DrawGameState(Ship ship1, Ship ship2, Camera camera, RenderTexture screenShip, Obstacles obstacles,
                    Model water_model, Model sky_model, Ship current_player_ship, Texture2D heart_full, Texture2D heart_empty){
    BeginTextureMode(screenShip);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        {
            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 350.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1.model, ship1.position, 1.0f, WHITE);
            DrawModel(ship1.cannon->rail_model, Vector3Add(ship1.position, Vector3RotateByAxisAngle(ship1.cannon->relative_position, (Vector3){0, 1, 0}, ship1.yaw)), 5.0f, WHITE);
            DrawModel(ship1.cannon->stand_model, Vector3Add(ship1.position, Vector3RotateByAxisAngle(ship1.cannon->relative_position, (Vector3){0, 1, 0}, ship1.yaw)), 5.0f, WHITE);
            DrawSphere(ship1.cannonball.position, 1, BLACK);

            DrawModel(ship2.model, ship2.position, 1.0f, WHITE);
            DrawModel(ship2.cannon->rail_model, Vector3Add(ship2.position, Vector3RotateByAxisAngle(ship2.cannon->relative_position, (Vector3){0, 1, 0}, ship2.yaw)), 5.0f, WHITE);
            DrawModel(ship2.cannon->stand_model, Vector3Add(ship2.position, Vector3RotateByAxisAngle(ship2.cannon->relative_position, (Vector3){0, 1, 0}, ship2.yaw)), 5.0f, WHITE);
            DrawSphere(ship2.cannonball.position, 1, BLACK);

            for(int i = 0; i < obstacles.island_count; i++){
                DrawModel(obstacles.island_list[i].island_sphere, obstacles.island_list[i].center_pos, 1, WHITE);
                DrawModel(obstacles.island_list[i].palm_tree, Vector3Add(
                    obstacles.island_list[i].center_pos, (Vector3){0, obstacles.island_list[i].radius, 0}), 1, WHITE);
            }
            for(int i = 0; i < obstacles.rock_count; i++){
                DrawModel(obstacles.rock_list[i].model, obstacles.rock_list[i].center_pos, 1, WHITE);
            }
        }
        EndMode3D();

        if(settings.show_fps) DrawFPS(WIDTH/2-100, HEIGHT-30);

        //for i between 0, ship.current_health exclusive, render full hearts spaces 55px apart (48px width), for i between 0, inital - current health, render black hearts
        for(int i = 0; i < current_player_ship.initial_health; i++){
            if(i < current_player_ship.current_health){
                DrawTexture(heart_full, 5 + 55*i, 5, WHITE); //each heart is anchored 55px from the prev, and img width is 48px.
            } else {
                DrawTexture(heart_empty, 5 + 55 * i, 5, WHITE); //hearts empty in those indices
            }
        }
    }
    //Insert debugging text here when needed
    DrawText(TextFormat("%d", obstacles.rock_count), 5, HEIGHT - 30, 20, LIME);
    for(int i = 0; i < obstacles.rock_count; i++){
        DrawText(TextFormat("%d", obstacles.rock_list[i].rotation_vec.x), 25*i, HEIGHT - 50, 20, LIME);
    }
    EndTextureMode();
}

void Update_Variables(Ship* ship1, Ship* ship2, Sound explosion, Obstacles obstacles){
    // rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0});
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
    // rotate cannon
    //! No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //! Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1->yaw - 3.1415f / 2 + ship1->cannon->rotation.y));
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0});
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415f / 2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415f / 2, 0}));
    Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2->yaw - 3.1415f / 2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    CheckHit(ship1, ship2, &current_screen, explosion, obstacles, settings.enable_sfx);
    CheckHit(ship2, ship1, &current_screen, explosion, obstacles, settings.enable_sfx);
}