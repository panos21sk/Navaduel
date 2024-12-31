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
Cannonball cannonball;
int move_time = MOVEMENT_TIME;
int fire_time = FIRE_TIME;
Ship *current_turn;
Ship *next_turn;
int dice_state = 1; //dice = choosing randomly a player for turn-based gameplay
int reset_state = 1;
bool has_fired_once = false;
void *DecreaseTime(void *arg);
void *DecreaseCounter(void *arg);

/* Threads */
pthread_t decrement_counter_thread;
pthread_t decrement_move_time_thread;
pthread_t decrement_fire_time_thread;

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
            while(allow_next_loop) { //decreasing startup count
                allow_next_loop = 0;
                pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
                pthread_detach(decrement_counter_thread);
            }
        }
        EndDrawing();
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
            ship1->can_fire = true;
            ship2->can_move = true;
            ship2->can_fire = true;

            pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
            pthread_detach(decrement_counter_thread);
        }
        EndDrawing();
    }
    else
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            ship1->can_fire = true;
            ship2->can_fire = true;
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
void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, Obstacles obstacles,
        const Model water_model, const Model sky_model, Sound splash, Sound fire, Sound explosion, Texture2D heart_full, Texture2D heart_empty)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    if(dice_state) { //"throws" a D10 (0-9) dice to declare the first turn
        srand(time(0));
        int number = rand()%10;
        current_turn = isEvenNumber(number) ? ship2 : ship1;
        next_turn = isEvenNumber(number) ? ship1 : ship2;
        dice_state = 0; //thrown
    }

    if(reset_state) { //on 1, resets move_time and fire_time to default
        cannonball.position = (Vector3){0, 1000, 0};
        cannonball.velocity = Vector3Zero();
        cannonball.accel = Vector3Zero();
        cannonball.has_splashed = true;
        cannonball.has_hit_enemy = true;

        move_time = MOVEMENT_TIME;
        fire_time = FIRE_TIME;
        startup_counter = GAME_STARTUP_COUNTER; //may remove
        has_fired_once = false;
        current_turn->accel = default_accel;
        next_turn->accel = default_accel;
        next_turn->cannonball = cannonball;
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

    Update_Variables(ship1, ship2, explosion, obstacles);

    DrawGameState(*ship1, *ship2, *(current_turn->camera), screenCurrentShip, obstacles, water_model, sky_model, *current_turn, heart_full, heart_empty);

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
            DrawText(current_turn->id == 1 ? "Player 1" : "Player 2", WIDTH-150, 30, 20, RED);
        }
        EndDrawing();
        free(text);

        if(allow_next_loop) {
            allow_next_loop = 0;
            pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
            pthread_detach(decrement_counter_thread);
        }
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
            DrawText(current_turn->id == 1 ? "Player 1" : "Player 2", WIDTH-150, 30, 20, RED);
            current_turn->can_move = true;
        }
        EndDrawing();

        if(allow_next_loop) {
            allow_next_loop = 0;
            pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
            pthread_detach(decrement_counter_thread);
        }
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
        }
        EndDrawing();

        while(move_time > 0 && allow_next_loop) {
            current_turn->can_fire = false;
            allow_next_loop = 0;
            pthread_create(&decrement_move_time_thread, NULL, DecreaseTime, &move_time);
            pthread_detach(decrement_move_time_thread);
        }
        pthread_cancel(decrement_move_time_thread);
        if(move_time == 0) {
            current_turn->can_move = false;
            current_turn->can_fire = true;
        }

        if(current_turn->cannonball.position.y < 0) {
            has_fired_once = true;
        }
        while(move_time == 0 && fire_time > 0 && allow_next_loop) {
            allow_next_loop = 0;
            pthread_create(&decrement_fire_time_thread, NULL, DecreaseTime, &fire_time);
            pthread_detach(decrement_fire_time_thread);
        }
        pthread_cancel(decrement_fire_time_thread);
        if(fire_time == 0 || has_fired_once) {
            current_turn->can_fire = false;
        }
        if(current_turn->cannonball.has_splashed && move_time == 0 && fire_time == 0) {
            void *temp = current_turn;
            current_turn = next_turn;
            next_turn = temp;
            reset_state = 1;
        }
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

void *DecreaseTime(void *arg) {
    int *input = (int *)arg;
    (*input)--;
    sleep(1); //1 second off
    allow_next_loop = 1;
}

void *DecreaseCounter(void *arg) {
    int *input = (int *)arg;
    sleep(1);
    (*input)--;
    allow_next_loop = 1;
}