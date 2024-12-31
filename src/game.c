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
#include <pthread.h>

/* Game independant variables */
int startup_counter = GAME_STARTUP_COUNTER;
bool is_loaded = false;
int winner;
BoundingBox game_bounds;
int allow_next_loop = 1; //controls loops, DO NOT DELETE IN ANY CASE (the program will hang up)

/* Turn-based gameplay declarations */
// move_time and fire_time are external -> to be saved in every game state
int move_time = MOVEMENT_TIME;
int fire_time = FIRE_TIME;
Ship *current_turn;
Ship *next_turn;
int dice_state = 1; //dice = choosing randomly a player for turn-based gameplay
int reset_state = 0;
void *DecreaseTime(void *arg);

/**
 * @brief Description to-do
 * @param ship1
 * @param ship2
 * @param obstacles
 * @param water_model
 * @param sky_model
 * @param splash
 * @param fire
 * @param explosion
 * @param heart_full
 * @param heart_empty
 * @returns void
 */
void DisplayRealTimeGameScreen(Ship_data ship_data, Obstacles obstacles,
        const Model water_model, Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    HideCursor();

    //! Checks collision with bounds (skybox)
    CheckCollisionWithBounds(&ship_data.ship_list[0], game_bounds);
    CheckCollisionWithBounds(&ship_data.ship_list[1], game_bounds);

    //! Input Handling:
    // Ship Movement
    CheckMovement(&ship_data.ship_list[0], fire, settings.enable_sfx);
    CheckMovement(&ship_data.ship_list[1], fire, settings.enable_sfx);

    // Update Camera manually
    UpdateShipCamera(&ship_data.ship_list[0], settings.first_or_third_person_cam);
    UpdateShipCamera(&ship_data.ship_list[1], settings.first_or_third_person_cam);

    UpdateCannonballState(&ship_data.ship_list[0].cannonball, splash, settings.enable_sfx);
    UpdateCannonballState(&ship_data.ship_list[1].cannonball, splash, settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    Update_Variables(ship_data, explosion, obstacles);

    DrawGameState(ship_data, *ship_data.ship_list[0].camera, screenShip1, obstacles, water_model, sky_model, ship_data.ship_list[0], heart_full, heart_empty);
    DrawGameState(ship_data, *ship_data.ship_list[1].camera, screenShip2, obstacles, water_model, sky_model, ship_data.ship_list[1], heart_full, heart_empty);

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
            ship_data.ship_list[0].can_move = true;
            ship_data.ship_list[1].can_move = true;
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

/**
 * @brief Description to-do
 *
 * @param ship1
 * @param ship2
 * @param obstacles
 * @param water_model
 * @param sky_model
 * @param splash
 * @param fire
 * @param explosion
 * @param heart_full
 * @param heart_empty
 * @returns void
 */
void DisplayTurnBasedGameScreen(Ship_data ship_data, Obstacles obstacles,
        const Model water_model, const Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    if(dice_state) { //"throws" a D10 (0-9) dice to declare the first turn
        srand(time(0));
        int number = rand() % ship_data.player_count;
        dice_state = 0; //thrown
        current_turn = &ship_data.ship_list[number];
    }

    if(reset_state) { //on 1, resets move_time and fire_time to default
        move_time = MOVEMENT_TIME;
        fire_time = FIRE_TIME;
        startup_counter = GAME_STARTUP_COUNTER; //may remove
        current_turn->accel = default_accel;
        reset_state = 0; //reset
    }

    HideCursor();

    CheckCollisionWithBounds(current_turn, game_bounds);

    //! Input Handling:
    // Ship Movement
    CheckMovement(current_turn, fire, settings.enable_sfx);

    // Update Camera manually
    UpdateShipCamera(current_turn, settings.first_or_third_person_cam);

    UpdateCannonballState(&current_turn->cannonball, splash, settings.enable_sfx);

    Update_Variables(ship_data, explosion, obstacles);

    DrawGameState(ship_data, *(current_turn->camera), screenCurrentShip, obstacles, water_model, sky_model, *current_turn, heart_full, heart_empty);

    const Rectangle screenRec = {0.0f, 0.0f, (float)screenCurrentShip.texture.width, (float)-screenCurrentShip.texture.height};

    //Game start
    if (startup_counter > 0)
    {
        char *text = malloc(sizeof(char) * 2); // with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH / 2, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, RED);
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

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            DrawText("Begin!", WIDTH / 2 - 70, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, RED);
            current_turn->can_move = true;
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

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            DrawText(current_turn->id == 1 ? "Player 1" : "Player 2", WIDTH-150, 30, 20, RED);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);

            while(move_time > 0 && allow_next_loop) {
                current_turn->can_fire = false;
                allow_next_loop = 0;
                pthread_t decrement_thread;
                pthread_create(&decrement_thread, NULL, DecreaseTime, &move_time);
                pthread_detach(decrement_thread);
            }
            if(move_time == 0) {
                current_turn->can_move = false;
                if(fire_time != 0) current_turn->can_fire = true;
            }

            while(move_time == 0 && fire_time > 0 && allow_next_loop) {
                allow_next_loop = 0;
                pthread_t decrement_thread;
                pthread_create(&decrement_thread, NULL, DecreaseTime, &fire_time);
                pthread_detach(decrement_thread);
            }
            if(fire_time == 0) {
                current_turn->can_fire = false;
            }
            if(current_turn->cannonball.has_splashed && move_time == 0 && fire_time == 0) {
                current_turn = &ship_data.ship_list[current_turn->id + 1];
                reset_state = 1;
            }
        }
        EndDrawing();
    }
}

void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles,
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

            for(int i = 0; i < ship_data.player_count; i++){
                DrawModel(ship_data.ship_list[i].model, ship_data.ship_list[i].position, 1.0f, WHITE);
                DrawModel(ship_data.ship_list[i].cannon->rail_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, WHITE);
                DrawModel(ship_data.ship_list[i].cannon->stand_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, WHITE);
                DrawSphere(ship_data.ship_list[i].cannonball.position, 1, BLACK);
            }

            for(int i = 0; i < obstacles.island_count; i++){
                DrawModel(obstacles.island_list[i].island_sphere, obstacles.island_list[i].center_pos, 1, WHITE);
                DrawModel(obstacles.island_list[i].palm_tree, Vector3Add(
                    obstacles.island_list[i].center_pos, (Vector3){0, (float)obstacles.island_list[i].radius, 0}), 1, WHITE);
            }

            for(int i = 0; i < obstacles.rock_count; i++){
                DrawModel(obstacles.rock_list[i].model, obstacles.rock_list[i].center_pos, 1, WHITE);
            }
        }
        EndMode3D();

        if(settings.show_fps) DrawFPS(
            gamemode == GAME_REAL ? WIDTH/2-100 : WIDTH-100,
            HEIGHT-30);

        //for i between 0, ship.current_health exclusive, render full hearts spaces 55px apart (48px width), for i between 0, inital - current health, render black hearts
        for(int i = 0; i < current_player_ship.initial_health; i++){
            if(i < current_player_ship.current_health){
                DrawTexture(heart_full, 5 + 55*i, 5, WHITE); //each heart is anchored 55px from the prev, and img width is 48px.
            } else {
                DrawTexture(heart_empty, 5 + 55 * i, 5, WHITE); //hearts empty in those indices
            }
        }

        /*//Insert debugging text here when needed
        DrawText(TextFormat("%d", obstacles.rock_count), 5, HEIGHT - 30, 20, LIME);
        for(int i = 0; i < obstacles.rock_count; i++){
            DrawText(TextFormat("%d", obstacles.rock_list[i].rotation_vec.x), 25*i, HEIGHT - 50, 20, LIME);
        }*/
    }
    EndTextureMode();
}

void Update_Variables(Ship_data ship_data, Sound explosion, Obstacles obstacles){
    // rotate ships
    for(int i = 0; i < ship_data.player_count; i++){
        ship_data.ship_list[i].model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw, 0});
        ship_data.ship_list[i].cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
        // rotate cannon
        //! No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
        //! Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
        // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
        // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
        Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship_data.ship_list[i].cannon->rotation.x), MatrixRotateY(ship_data.ship_list[i].yaw - 3.1415f / 2 + ship_data.ship_list[i].cannon->rotation.y));
        ship_data.ship_list[i].cannon->rail_model.transform = cannon_transform1;
        for(int j = 0; j < ship_data.player_count; j++){
            if(i!=j) CheckHit(&ship_data.ship_list[i], &ship_data.ship_list[j], &current_screen, explosion, obstacles, settings.enable_sfx);
        }
    }
}

void *DecreaseTime(void *arg) {
    int *input = (int *)arg;
    (*input)--;
    sleep(1); //1 second off
    allow_next_loop = 1;
}