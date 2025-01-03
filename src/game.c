#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "game.h"
#include "ship.h"
#include "anim.h"
#include "screens.h"
#include "obstacles.h"
#include "util.h"

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

void FixShipPosition() { // potential fix for ship position offset
    for(int i = 0; i<ship_data.player_count; i++) {
        const float check = ship_data.type_list[i] == 0 ? 17.0f : 8.0f;
        if(ship_data.ship_list[i].position.y < check) ship_data.ship_list[i].position.y = check;
    }
}

void DisplayRealTimeGameScreen(const Ship_data ship_data, const Obstacles obstacles,
        Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    FixShipPosition();

    HideCursor();

    //! Checks collision with bounds (skybox)
    CheckCollisionWithBounds(&ship_data.ship_list[0], game_bounds);
    CheckCollisionWithBounds(&ship_data.ship_list[1], game_bounds);

    //! Input Handling:
    // Ship Movement
    CheckMovement(&ship_data.ship_list[0], game_sounds[0], settings.enable_sfx);
    CheckMovement(&ship_data.ship_list[1], game_sounds[0], settings.enable_sfx);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(&ship_data.ship_list[0], settings.first_or_third_person_cam);
    UpdateShipCamera(&ship_data.ship_list[1], settings.first_or_third_person_cam);

    UpdateCannonballState(&ship_data.ship_list[0].cannonball, game_sounds[1], &anim_list[0], settings.enable_sfx);
    UpdateCannonballState(&ship_data.ship_list[1].cannonball, game_sounds[1], &anim_list[0], settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    UpdateVariables(ship_data, game_sounds[2], obstacles, &anim_list[1]);

    UpdateAnim(&anim_list[0]);
    UpdateAnim(&anim_list[1]);

    DrawGameState(ship_data, *ship_data.ship_list[0].camera, screenShip1, obstacles, game_models, ship_data.ship_list[0], game_textures, anim_list);
    DrawGameState(ship_data, *ship_data.ship_list[1].camera, screenShip2, obstacles, game_models, ship_data.ship_list[1], game_textures, anim_list);

    if (startup_counter > 0)
    {
        ship_data.ship_list[0].can_fire = false;
        ship_data.ship_list[1].can_fire = false;
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText(TextFormat("%d", startup_counter), WIDTH / 4, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("%d", startup_counter), 3 * WIDTH / 4, HEIGHT / 2, 50, WHITE);
            while(allow_next_loop) { //decreasing startup count
                allow_next_loop = 0;
                pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
                pthread_detach(decrement_counter_thread);
            }
        }
        EndDrawing();
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
            ship_data.ship_list[0].can_fire = true;
            ship_data.ship_list[1].can_fire = true;

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

            ship_data.ship_list[0].can_fire = true;
            ship_data.ship_list[1].can_fire = true;
        }
        EndDrawing();
    }
}

void DisplayTurnBasedGameScreen(const Ship_data ship_data, const Obstacles obstacles,
        Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU;

    FixShipPosition();

    if(dice_state) {
        srand(time(0));
        const int number = rand() % ship_data.player_count;
        current_turn = &ship_data.ship_list[number];
        next_turn = &ship_data.ship_list[number + 1];
        dice_state = 0; //thrown
    }

    if(reset_state) { //on 1, resets move_time and fire_time to default
        move_time = MOVEMENT_TIME;
        fire_time = FIRE_TIME;
        startup_counter = GAME_STARTUP_COUNTER; //may remove
        has_fired_once = false;
        current_turn->accel = current_turn->default_accel;
        reset_state = 0; //reset
    }

    HideCursor();

    CheckCollisionWithBounds(current_turn, game_bounds);

    //! Input Handling:
    // Ship Movement
    CheckMovement(current_turn, game_sounds[0], settings.enable_sfx);

    // Update Camera manually
    UpdateShipCamera(current_turn, settings.first_or_third_person_cam);

    UpdateCannonballState(&current_turn->cannonball, game_sounds[1], &anim_list[0], settings.enable_sfx);

    UpdateVariables(ship_data, game_sounds[2], obstacles, &anim_list[1]);

    UpdateAnim(&anim_list[0]);
    UpdateAnim(&anim_list[1]);

    DrawGameState(ship_data, *(current_turn->camera), screenCurrentShip, obstacles, game_models, *current_turn, game_textures, anim_list);

    const Rectangle screenRec = {0.0f, 0.0f, (float)screenCurrentShip.texture.width, (float)-screenCurrentShip.texture.height};

    //Game start
    if (startup_counter > 0)
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            DrawText(TextFormat("%d", startup_counter), WIDTH / 2, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, RED);
        }
        EndDrawing();

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
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, RED);
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

            DrawText(TextFormat("Player %d", current_turn-> id + 1), WIDTH-150, 30, 20, RED);
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
        if(fire_time == 0 || has_fired_once) {
            current_turn->can_fire = false;
        }
        if(current_turn->cannonball.has_splashed && move_time == 0 && fire_time == 0) {
            next_turn = current_turn;
            if(current_turn->id < ship_data.player_count - 1){
                current_turn = &ship_data.ship_list[current_turn->id + 1];
            } else {
                current_turn = &ship_data.ship_list[0];
            }
            cannonball.position = (Vector3){0, 1000, 0};
            cannonball.velocity = Vector3Zero();
            cannonball.accel = Vector3Zero();
            cannonball.has_splashed = true;
            cannonball.has_hit_enemy = true;

            next_turn->accel = next_turn->default_accel;
            next_turn->cannonball = cannonball;

            reset_state = 1;
        }
    }
}

void DrawGameState(Ship_data ship_data, Camera camera, RenderTexture screenShip, Obstacles obstacles,
                    Model* game_models, Ship current_player_ship, Texture2D* game_textures, Animation* anim_list){
    BeginTextureMode(screenShip);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        {
            DrawModel(game_models[0], (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(game_models[1], (Vector3){0.0f, 350.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            for(int i = 0; i < ship_data.player_count; i++){
                DrawModel(ship_data.ship_list[i].model, ship_data.ship_list[i].position, 1.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                DrawModel(ship_data.ship_list[i].cannon->rail_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                DrawModel(ship_data.ship_list[i].cannon->stand_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
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

            DrawAnim(anim_list[0], *current_player_ship.camera);
            DrawAnim(anim_list[1], *current_player_ship.camera);
        }
        EndMode3D();

        DrawUI(current_player_ship, game_textures, screenShip);
        
    }
    EndTextureMode();
}

void DrawUI(Ship current_player_ship, Texture2D* game_textures, RenderTexture screenShip){
    if(settings.show_fps) DrawFPS(
            gamemode == GAME_REAL ? WIDTH/2-100 : WIDTH-100,
            20);

        //for i between 0, ship.current_health exclusive, render full hearts spaces 55px apart (48px width), for i between 0, inital - current health, render black hearts
        for(int i = 0; i < current_player_ship.initial_health; i++){
            if(i < current_player_ship.current_health){
                DrawTexture(game_textures[1], 5 + 55*i, 5, WHITE); //each heart is anchored 55px from the prev, and img width is 48px.
            } else {
                DrawTexture(game_textures[0], 5 + 55 * i, 5, WHITE); //hearts empty in those indices
            }
        }

        Rectangle powerbar_rec = (Rectangle){screenShip.texture.width - 85, screenShip.texture.height - 25, 80, 20};
        DrawRectangleLinesEx((Rectangle){powerbar_rec.x - 2, powerbar_rec.y - 2, powerbar_rec.width + 4, powerbar_rec.height + 4}, 2, LIME);
        //percentage=(value−min)/(max−min) ​×100
        DrawRectangleRec((Rectangle){powerbar_rec.x, powerbar_rec.y, 
                        powerbar_rec.width * (current_player_ship.cannon->rotation.x / - MAX_TURN_UP) * 5
                        , powerbar_rec.height}, MAROON);

        //Insert debugging text here when needed
        //DrawText(TextFormat("%f", current_player_ship.cannon->rotation.x), 5, HEIGHT - 30, 20, RED);
}

void UpdateVariables(Ship_data ship_data, Sound explosion, Obstacles obstacles, Animation* explosion_anim){
    // rotate ships
    for(int i = 0; i < ship_data.player_count; i++){
        ship_data.ship_list[i].model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw, 0});
        ship_data.ship_list[i].cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
        // rotate cannon
        // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
        // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
        Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship_data.ship_list[i].cannon->rotation.x), MatrixRotateY(ship_data.ship_list[i].yaw - 3.1415f / 2 + ship_data.ship_list[i].cannon->rotation.y));
        ship_data.ship_list[i].cannon->rail_model.transform = cannon_transform1;
        for(int j = 0; j < ship_data.player_count; j++){
            if(i!=j) CheckHit(&ship_data.ship_list[i], &ship_data.ship_list[j], &current_screen, explosion, obstacles, &ship_data, settings.enable_sfx, explosion_anim);
        }
    }
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
