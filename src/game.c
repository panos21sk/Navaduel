/* Import the required game headers (first party libraries) */
#include "game.h"
#include "ship.h"
#include "anim.h"
#include "screens.h"
#include "obstacles.h"
#include "util.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

/* Import some tool headers (third party libraries) */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>

/* Game independent variables initialization */
int startup_counter = GAME_STARTUP_COUNTER;
bool is_loaded = false;
char* wintext = NULL;
BoundingBox game_bounds;
int allow_next_loop = 1; //Controls loops in time decrement

/* Turn-based gameplay variables initialization */
Cannonball cannonball; //A Cannonball object, used to initialize the cannonball per-turn
int move_time = MOVEMENT_TIME;
int fire_time = FIRE_TIME;
int dice_state = 1;
int reset_state = 1;
bool has_fired_once = false;
Ship *current_turn;

/* Threads */
pthread_t decrement_counter_thread; //Responsible to decrease the start-up counter (in all gamemodes)
pthread_t decrement_move_time_thread; //Responsible to decrease the ship's move time (turn-based gamemode)
pthread_t decrement_fire_time_thread; //Responsible to decrease the ship's fire time (turn-based gamemode)

/**
 * @brief A bug-fixing procedure
 * @details This procedure fixes a bug that occurred when trying to spawn each ship in a rendering screen. Occasionally,
 * the ship's Y position coordinate would "displace" itself to a lower point (between 10-20 points off). Therefore, this
 * procedure forces ships to ALWAYS spawn in the Y position coordinate which responds to their type. This bug is reproducible
 * by removing the call in DisplayRealTimeGameScreen and DisplayTurnBasedGameScreen procedures.
 */
void FixShipPosition() {
    for (int i = 0; i < ship_data.player_count; i++) {
        const float check = ship_data.type_list[i] == 0 ? 17.0f : 8.0f;
        if (ship_data.ship_list[i].position.y < check || ship_data.ship_list[i].position.y > check)
            ship_data.ship_list[i].position.y = check;
    }
}

/**
 * @brief Displays the real-time gameplay screen and manages the game.
 * @param ship_data Holds the spawned ships' data
 * @param obstacles Holds the spawned islands and rocks' data
 * @param game_models Holds the models to be rendered in-game
 * @param game_sounds Holds the sounds to be played in-game
 * @param game_textures Holds the textures to be drawn in-game
 * @param anim_list Holds the animations to be played in-game
 * @param water_textures Holds the water textures for the water animation to be played in-game
 */
void DisplayRealTimeGameScreen(const Ship_data ship_data, const Obstacles obstacles,
                               const Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list, const Texture2D* water_textures)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU; //ESC pressed -> game menu

    FixShipPosition();

    HideCursor();

    /* Checks collision with bounds (skybox) */
    CheckCollisionWithBounds(&ship_data.ship_list[0], game_bounds);
    CheckCollisionWithBounds(&ship_data.ship_list[1], game_bounds);

    /* Checks ships for any change in movement */
    CheckMovement(&ship_data.ship_list[0], game_sounds[0]);
    CheckMovement(&ship_data.ship_list[1], game_sounds[0]);

    /* Update Camera manually */
    UpdateShipCamera(&ship_data.ship_list[0], settings.first_or_third_person_cam);
    UpdateShipCamera(&ship_data.ship_list[1], settings.first_or_third_person_cam);

    /* Update the Cannonball manually */
    UpdateCannonballState(&ship_data.ship_list[0].cannonball, game_sounds[1], &anim_list[0]);
    UpdateCannonballState(&ship_data.ship_list[1].cannonball, game_sounds[1], &anim_list[0]);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height}; //Split screen rectangle

    UpdateVariables(ship_data, game_sounds[2], obstacles, &anim_list[1]);

    UpdateAnim(&anim_list[0]);
    UpdateAnim(&anim_list[1]);

    DrawGameState(ship_data, *ship_data.ship_list[0].camera, screenShip1, obstacles, 'r', game_models, ship_data.ship_list[0], game_textures, anim_list, water_textures);
    DrawGameState(ship_data, *ship_data.ship_list[1].camera, screenShip2, obstacles, 'r',game_models, ship_data.ship_list[1], game_textures, anim_list, water_textures);

    CheckWin(ship_data);

    if (startup_counter > 0) //Starts the counter for a new game
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
            while(allow_next_loop) { //Decreasing startup count
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

/**
 * @brief Displays the turn-based gameplay screen and manages the game.
 * @param ship_data Holds the spawned ships' data
 * @param obstacles Holds the spawned islands and rocks' data
 * @param game_models Holds the models to be rendered in-game
 * @param game_sounds Holds the sounds to be played in-game
 * @param game_textures Holds the textures to be drawn in-game
 * @param anim_list Holds the animations to be played in-game
 * @param water_textures Holds the water textures for the water animation to be played in-game
 */
void DisplayTurnBasedGameScreen(const Ship_data ship_data, const Obstacles obstacles,
        const Model* game_models, const Sound* game_sounds, Texture2D* game_textures, Animation* anim_list, const Texture2D* water_textures)
{
    if(IsKeyPressed(KEY_ESCAPE)) current_screen = GAME_MENU; //ESC pressed -> game menu

    FixShipPosition();

    if(dice_state) { //Declares the first player to play
        srand(time(0));
        const int number = rand() % ship_data.player_count;
        current_turn = &ship_data.ship_list[number];
        dice_state = 0; //Thrown
    }

    if(reset_state) {
        move_time = MOVEMENT_TIME;
        fire_time = FIRE_TIME;
        startup_counter = GAME_STARTUP_COUNTER;
        has_fired_once = false;
        current_turn->accel = current_turn->default_accel;
        reset_state = 0; //Reset
    }

    HideCursor();

    CheckCollisionWithBounds(current_turn, game_bounds);

    /* Checks ships for any change in movement */
    CheckMovement(current_turn, game_sounds[0]);

    /* Update Camera manually */
    UpdateShipCamera(current_turn, settings.first_or_third_person_cam);

    /* Update the Cannonball manually */
    UpdateCannonballState(&current_turn->cannonball, game_sounds[1], &anim_list[0]);

    UpdateVariables(ship_data, game_sounds[2], obstacles, &anim_list[1]);

    UpdateAnim(&anim_list[0]);
    UpdateAnim(&anim_list[1]);

    DrawGameState(ship_data, *(current_turn->camera), screenCurrentShip, obstacles, 't',game_models, *current_turn, game_textures, anim_list, water_textures);

    CheckWin(ship_data);

    const Rectangle screenRec = {0.0f, 0.0f, (float)screenCurrentShip.texture.width, (float)-screenCurrentShip.texture.height};


    if (startup_counter > 0) //Game or new turn start
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            DrawText(TextFormat("%d", startup_counter), WIDTH / 2, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, ReturnColorFromTeamInt(current_turn->team));
        }
        EndDrawing();

        current_turn->can_fire = false;

        if(allow_next_loop) {
            allow_next_loop = 0;
            pthread_create(&decrement_counter_thread, NULL, DecreaseCounter, &startup_counter);
            pthread_detach(decrement_counter_thread);
        }
    }
    else if (startup_counter == 0)
    {
        current_turn->prev_position_turn = current_turn->position;

        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenCurrentShip.texture, screenRec, (Vector2){0.0f, 0.0f}, WHITE);

            DrawText("Begin!", WIDTH / 2 - 70, HEIGHT / 2, 50, WHITE);
            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn->id + 1), WIDTH-150, 30, 20, ReturnColorFromTeamInt(current_turn->team));
            current_turn->can_move = true;
        }
        EndDrawing();

        current_turn->can_fire = false;

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

            DrawText(TextFormat("Move time: %d", move_time), WIDTH-200, HEIGHT/2, 20, ORANGE);
            DrawText(TextFormat("Fire time: %d", fire_time), WIDTH-200, HEIGHT/2+50, 20, ORANGE);
            DrawText(TextFormat("Player %d", current_turn-> id + 1), WIDTH-150, 30, 20, ReturnColorFromTeamInt(current_turn->team));
        }
        EndDrawing();

        if(has_fired_once) current_turn->can_fire = false; //Prevents the ships to shoot more than once

        while(move_time > 0 && allow_next_loop) { //Moving stage
            current_turn->can_fire = false;
            current_turn->cannonball.position.y = 1100;
            current_turn->cannonball.velocity.y = 0;
            current_turn->cannonball.accel.y = 0; //Reset cannonball y to ensure can_fire resets
            allow_next_loop = 0;
            /* Decrease the move time through a thread */
            pthread_create(&decrement_move_time_thread, NULL, DecreaseTime, &move_time);
            pthread_detach(decrement_move_time_thread);
        }
        if(move_time == 0) { //End of moving stage
            current_turn->can_move = false;
            if(!has_fired_once) current_turn->can_fire = true;
        }

        if(current_turn->cannonball.position.y < 0) has_fired_once = true; //Checks if the current ship has shot
        while(move_time == 0 && fire_time > 0 && allow_next_loop) { //Firing stage
            allow_next_loop = 0;
            pthread_create(&decrement_fire_time_thread, NULL, DecreaseTime, &fire_time);
            pthread_detach(decrement_fire_time_thread);
        }
        if(fire_time == 0) { //End of firing time
            current_turn->can_fire = false;
        }
        if(current_turn->cannonball.has_splashed && move_time == 0 && fire_time == 0) { //Changes the turn
            //find next available ship
            current_turn = &ship_data.ship_list[FindNextAliveShipIndex(ship_data, current_turn->id + 1)];

            cannonball.position = (Vector3){0, 1000, 0};
            cannonball.velocity = Vector3Zero();
            cannonball.accel = Vector3Zero();
            cannonball.has_splashed = true;
            cannonball.has_hit_enemy = true;

            reset_state = 1;
        }
    }
}

/**
 * @brief Schedules the textures, models and animations to be displayed in any gamemode and updated to camera
 * @param ship_data Holds the spawned ships' data
 * @param camera The Camera object in which everything is displayed and updated
 * @param screenShip The screen in which textures will be drawn
 * @param obstacles Holds the spawned islands and rocks' data
 * @param real_or_turn Determines if the gamemode is real-time or turn-based
 * @param game_models Holds the models to be rendered in-game
 * @param current_player_ship The screen ship's instance
 * @param game_textures Holds the textures to be drawn in-game
 * @param anim_list Holds the animations to be played in-game
 * @param water_textures Holds the water textures for the water animation to be played in-game
 */
void DrawGameState(const Ship_data ship_data, const Camera camera, const RenderTexture screenShip, const Obstacles obstacles, const char real_or_turn,
                    const Model* game_models, const Ship current_player_ship, Texture2D* game_textures, const Animation* anim_list, const Texture2D* water_textures){
    static int water_anim_index = 0;
    static int frame = 0;
    int delay_per_frame = 24;
    ++frame;
    if(frame >= delay_per_frame - 1){
        frame = 0;
        ++water_anim_index;
    }
    if(water_anim_index >= 5){
        water_anim_index = 0;
    }
    game_models[0].materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_textures[water_anim_index];

    BeginTextureMode(screenShip);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera);
        {
            DrawModel(game_models[0], (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(game_models[1], (Vector3){0.0f, 350.0f, 0.0f}, 750.0f, WHITE);
            rlEnableBackfaceCulling();

            for(int i = 0; i < ship_data.player_count; i++){
                if(i == current_player_ship.id || real_or_turn == 'r'){
                    if( ! ship_data.ship_list[i].is_destroyed){
                        DrawModel(ship_data.ship_list[i].model, ship_data.ship_list[i].position, 1.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                        DrawModel(ship_data.ship_list[i].cannon->rail_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                        DrawModel(ship_data.ship_list[i].cannon->stand_model, Vector3Add(ship_data.ship_list[i].position, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                        DrawSphere(ship_data.ship_list[i].cannonball.position, 1, BLACK);
                    }
                } else {
                    if(!Vector3Equals(ship_data.ship_list[i].prev_position_turn, (Vector3){0, 1000, 0})){
                        if( ! ship_data.ship_list[i].is_destroyed) {
                            DrawModelWires(ship_data.ship_list[i].model, ship_data.ship_list[i].prev_position_turn, 1.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                            DrawModelWires(ship_data.ship_list[i].cannon->rail_model, Vector3Add(ship_data.ship_list[i].prev_position_turn, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                            DrawModelWires(ship_data.ship_list[i].cannon->stand_model, Vector3Add(ship_data.ship_list[i].prev_position_turn, Vector3RotateByAxisAngle(ship_data.ship_list[i].cannon->relative_position, (Vector3){0, 1, 0}, ship_data.ship_list[i].yaw)), 5.0f, ReturnColorFromTeamInt(ship_data.ship_list[i].team));
                        }
                    }
                }
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

            if(settings.show_reticle){
                //draw sightline: starting point is cannons position at y = 0.1, end point is, from start point, the result of 
                // the addition of start point and 0,0,2000, rotated such that it aligns with where cannon points
                DrawLine3D(
                    Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, 
                                Vector3Add(current_player_ship.position, 
                                            Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw))), 
                    Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, 
                                    Vector3Add(Vector3RotateByAxisAngle((Vector3){0,0,1000}, (Vector3){0,1,0}, current_player_ship.yaw + current_player_ship.cannon->rotation.y), 
                                                Vector3Add(current_player_ship.position, 
                                                            Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw)))), 
                    PURPLE);
                //endpoint offset by +-0.8 x, to thicken line
                DrawLine3D(Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, Vector3Add(current_player_ship.position, Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw))), Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, Vector3Add(Vector3RotateByAxisAngle((Vector3){1,0,1000}, (Vector3){0,1,0}, current_player_ship.yaw + current_player_ship.cannon->rotation.y), Vector3Add(current_player_ship.position, Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw)))), PURPLE);
                DrawLine3D(Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, Vector3Add(current_player_ship.position, Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw))), Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, Vector3Add(Vector3RotateByAxisAngle((Vector3){-1,0,1000}, (Vector3){0,1,0}, current_player_ship.yaw + current_player_ship.cannon->rotation.y), Vector3Add(current_player_ship.position, Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw)))), PURPLE);

                //for the reticle itself, assume that the initial height is equal to the final height
                //Approach 1: simulate particle motion. Approach 2: Asynchronously calculate pos using a looping thread.
                //Formula: R = 2*tanf*u0^2*cos^2(f) / g -> R = u0^2*sin2f/g, T = 2*sinf*u0/g 
                float u0 = 1.25f - current_player_ship.cannonball_power_coefficient * current_player_ship.cannon->rotation.x;
                float g = -0.005f; //hardcoded to avoid conflict with initcannonball, since accel never changes anyways
                DrawSphereWires( 
                    /*center_pos:*/ 
                    Vector3Add(/*origin*/Vector3Add((Vector3){0, - current_player_ship.cannon->relative_position.y - current_player_ship.position.y + 6, 0}, 
                                                    Vector3Add(current_player_ship.position, 
                                                               Vector3RotateByAxisAngle(current_player_ship.cannon->relative_position, (Vector3){0, 1, 0}, current_player_ship.yaw))),
                                /*displacement*/Vector3RotateByAxisAngle((Vector3){0,0, pow(u0, 2) * sin(2 * current_player_ship.cannon->rotation.x) / g}, (Vector3){0,1,0},current_player_ship.yaw + current_player_ship.cannon->rotation.y)
                    ),1, 4, 4, LIME);
            }
        }
        EndMode3D();

        DrawUI(current_player_ship, game_textures, screenShip);
        
    }
    EndTextureMode();
}

/**
 * @brief Part of the DrawGameState procedure
 * @details Specifically, draws UI-related elements, such as the FPS text, the power and reload bars
 * @param current_player_ship The screen ship's instance
 * @param game_textures Holds the textures to be drawn in-game
 * @param screenShip The screen in which textures will be drawn
 */
void DrawUI(const Ship current_player_ship, const Texture2D* game_textures, const RenderTexture screenShip){
    if(settings.show_fps) DrawFPS(
            screenShip.texture.width - 100,
            gamemode == GAME_REAL ? 20 : 60);

        //For i between 0, ship.current_health exclusive, render full hearts spaces 55px apart (48px width), for i between 0, inital - current health, render black hearts
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
        DrawLine(powerbar_rec.x + powerbar_rec.width * (current_player_ship.prev_shot_release / - MAX_TURN_UP) * 5, powerbar_rec.y,
                  powerbar_rec.x + powerbar_rec.width * (current_player_ship.prev_shot_release / - MAX_TURN_UP) * 5, powerbar_rec.y + powerbar_rec.height,
                  YELLOW);

        Rectangle reload_rec = (Rectangle){screenShip.texture.width - 85, screenShip.texture.height - 55, 80, 20};
        DrawRectangleLinesEx((Rectangle){reload_rec.x - 2, reload_rec.y - 2, reload_rec.width + 4, reload_rec.height + 4}, 2, BLUE);
        //Angle theta is current_player_ship.prev_shot_release, max angle theta is 80deg or max_turn_up
        //u0 is initial vel 1.25f - current_player_ship.cannonball_power_coefficient * current_player_ship.cannon->rotation.x
        //initial h is current_player_ship.position.y + current_player_ship.cannon->relative_position.y
        //g is -current_player_ship.cannonball.accel.y
        float u0 = 1.25f - current_player_ship.cannonball_power_coefficient * current_player_ship.cannon->rotation.x;
        float g = -current_player_ship.cannonball.accel.y * 58;
        
        float max_T = 2 * u0 * sin(MAX_TURN_UP) / g;
        float min_T = 0;
        float percentage = (current_player_ship.time_to_reload_since_last_shot - min_T)/(max_T-min_T) * 0.6;
        DrawRectangleRec((Rectangle){reload_rec.x, reload_rec.y, 
                        reload_rec.width * percentage
                        , reload_rec.height}, YELLOW);
}

/**
 * @brief A procedure which checks for changes and updates the ships' state (attributes)
 * @param ship_data Holds the spawned ships' data
 * @param explosion The explosion sound to be played in-game
 * @param obstacles Holds the spawned islands and rocks' data
 * @param explosion_anim The explosion animation to be played in-game
 */
void UpdateVariables(Ship_data ship_data, const Sound explosion, const Obstacles obstacles, Animation* explosion_anim){
    // rotate ships
    for(int i = 0; i < ship_data.player_count; i++){
        ship_data.ship_list[i].model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw, 0});
        ship_data.ship_list[i].cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship_data.ship_list[i].yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
        /* Rotate cannon */
        // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
        // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
        Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship_data.ship_list[i].cannon->rotation.x), MatrixRotateY(ship_data.ship_list[i].yaw - 3.1415f / 2 + ship_data.ship_list[i].cannon->rotation.y));
        ship_data.ship_list[i].cannon->rail_model.transform = cannon_transform1;
        for(int j = 0; j < ship_data.player_count; j++){
            if(i!=j) CheckHit(&ship_data.ship_list[i], &ship_data.ship_list[j], explosion, obstacles, explosion_anim);
        }
    }
}

/**
 * @brief Decreases a time variable by 1 per second
 * @details Used in threads for every gamemode
 * @note The function is NOT defined to return a pointer, please ignore any warnings.
 * @param arg The time variable to decrease. This argument is passed through the pthread_create function
 * @return Nothing, according to its usage in DisplayRealTimeGameScreen and DisplayTurnBasedGameScreen
 */
void *DecreaseTime(void *arg) {
    int *input = (int *)arg;
    (*input)--;
    sleep(1); //1 second off
    allow_next_loop = 1;
}

/**
 * @brief Decreases a counter variable by 1 per second
 * @details Used in threads for every gamemode
 * @note The function is NOT defined to return a pointer, please ignore any warnings.
 * @param arg The counter variable to decrease. This argument is passed through the pthread_create function
 * @return Nothing, according to its usage in DisplayRealTimeGameScreen and DisplayTurnBasedGameScreen
 */
void *DecreaseCounter(void *arg) {
    int *input = (int *)arg;
    sleep(1);
    (*input)--;
    allow_next_loop = 1;
}
