#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "game.h"
#include "ship.h"
#include "screens.h"
#include "util.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int startup_counter = GAME_STARTUP_COUNTER;
int winner;

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, const Model water_model, Model sky_model, Sound splash, Sound fire)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    HideCursor();

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
    const Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2->yaw - 3.1415f / 2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    ship1->boundary = GetMeshBoundingBox(ship1->model.meshes[0]);
    ship2->boundary = GetMeshBoundingBox(ship2->model.meshes[0]);

    sky_model.transform = MatrixMultiply(sky_model.transform, MatrixScale(1000.0f, 1000.0f, 1000.0f));
    BoundingBox sky_bounding_box = GetMeshBoundingBox(sky_model.meshes[0]);
    sky_bounding_box.min = Vector3Scale(sky_bounding_box.min, 1000.0f);
    sky_bounding_box.max = Vector3Scale(sky_bounding_box.max, 1000.0f);

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            // draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            // TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(ship2->boundary, LIME);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();

        //! DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug, 10, 30, 20, BLACK);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

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

void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, const Model water_model, const Model sky_model, Sound splash, Sound fire)
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

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            // draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            // TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();

        //! DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug, 10, 10, 20, BLACK);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

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