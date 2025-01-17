/* Import the required game headers (first party libraries) */
#include "ship.h"
#include "game.h"
#include "screens.h"
#include "anim.h"
#include "util.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"
#include "raymath.h"

/* Import the required tool headers (third party libraries) */
#include "cJSON.h"
#include <pthread.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/* Variable initialization */
accel_settings bounds_accel;
Cannonball initcannonball;
Ship_data ship_data;
bool game_ended;

/**
 * @brief Sets up the appropriate amount of ships for a game to begin.
 * @param player_count The amount of Players (ships) to be spawned
 * @param type_list An array of integers declaring each ship's type
 * @param team_list An array of integers declaring each ship's team
 * @param obs The Obstacles object, holding data for spawned obstacles
 * @param ship_models An array of Models with the ships' models
 * @return A pointer to the generated Ship object
 */
Ship *SetupShips(int player_count, const int *type_list, const int *team_list, Obstacles obs, Model *ship_models)
{
    static Ship ship_list[8]; // 8 is the num of max players
    // Variable init
    const movement_buttons btns1 = settings.player_one_buttons;
    const movement_buttons btns2 = settings.player_two_buttons;
    const movement_buttons btns3 = settings.player_indep_buttons;

    // Cannonball init
    initcannonball.position = (Vector3){0, 1000, 0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();
    initcannonball.has_splashed = true;
    initcannonball.has_hit_enemy = true;

    for (int i = 0; i < player_count; i++)
    {
        Cannon cannon_inst;
        Cannon *cannon_addr = MemAlloc(sizeof(Cannon));
        Ship ship_inst;
        Camera *camera_addr = MemAlloc(sizeof(Camera));
        float init_y = 0.0f;

        ship_inst.is_spawn_valid = false;

        cannon_inst.rotation = Vector3Zero();
        cannon_inst.stand_model = ship_models[2];
        cannon_inst.rail_model = ship_models[3];
        // init for big ship
        if (type_list[i] == 0)
        {
            init_y = 17.0f; // 17 for ship 1
            cannon_inst.relative_position = (Vector3){0, -8, 20};
            ship_inst.model = ship_models[0];
            ship_inst.max_accel = MAX_ACCEL * 0.7f;
            ship_inst.min_accel = MIN_ACCEL * 0.8f;
            ship_inst.accel_step = ACCEL_STEP * 0.9f;
            ship_inst.cannonball_power_coefficient = 0.7f;

            ship_inst.camera_distance_vector_tp = (Vector3){0.0f, 35.0f, -70.0f};
            ship_inst.camera_distance_vector_fp = (Vector3){0.0f, -4.0f, 16.0f};
            ship_inst.sphere_hitbox_radius = 15;
            ship_inst.initial_health = 4;
        }
        // init for small ship
        else if (type_list[i] == 1)
        {
            init_y = 8.0f;
            cannon_inst.relative_position = (Vector3){0, 1, 7};
            ship_inst.model = ship_models[1];
            ship_inst.max_accel = MAX_ACCEL * 1.5f;
            ship_inst.min_accel = MIN_ACCEL * 1.2f;
            ship_inst.accel_step = ACCEL_STEP * 1.1f;
            ship_inst.cannonball_power_coefficient = 0.3f;

            ship_inst.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
            ship_inst.camera_distance_vector_fp = (Vector3){0.0f, 5.0f, 3.0f};
            ship_inst.sphere_hitbox_radius = 10;
            ship_inst.initial_health = 3;
        }
        ship_inst.default_accel = (accel_settings){ship_inst.min_accel, ship_inst.min_accel, ship_inst.min_accel, ship_inst.min_accel, ship_inst.min_accel, ship_inst.min_accel, ship_inst.min_accel};
        ship_inst.accel = ship_inst.default_accel;
        ship_inst.prev_position = (Vector3){0.0f, 0.0f, 0.0f};
        ship_inst.prev_position_turn = (Vector3){0.0f, 1000.0f, 0.0f};
        ship_inst.prev_shot_release = 0;
        ship_inst.time_to_reload_since_last_shot = 0;
        ship_inst.boundary = GetMeshBoundingBox(ship_inst.model.meshes[0]);
        // VALIDATING SPAWN POS
        while (!ship_inst.is_spawn_valid)
        {
            // randomize position and set spawn to be valid until proven otherwise
            ship_inst.is_spawn_valid = true;
            ship_inst.position = (Vector3){
                (float)GetRandomValue(-320, 320), init_y, (float)GetRandomValue(-320, 320) // add it via ref to bounds later
            };
            // check if ship spawns on island
            for (int i1 = 0; i1 < obs.island_count; i1++)
            {
                if (CheckCollisionSpheres(obs.island_list[i1].center_pos, (float)obs.island_list[i1].radius, ship_inst.position, ship_inst.sphere_hitbox_radius))
                {
                    ship_inst.is_spawn_valid = false;
                }
            }
            // check if ship spawns on rock
            for (int i2 = 0; i2 < obs.rock_count; i2++)
            {
                switch (obs.rock_list[i2].geometry_id)
                {
                case 1: // cube
                    // assuming the origin of the mesh from genmeshcube is the geometrical center, where its relative coordinates are (0,0,0) when drawn in other words
                    if (CheckCollisionBoxSphere((BoundingBox){
                                                    (Vector3){obs.rock_list[i2].center_pos.x - (float)obs.rock_list[i2].height / 6,
                                                              obs.rock_list[i2].center_pos.y - (float)obs.rock_list[i2].height / 2,
                                                              obs.rock_list[i2].center_pos.z - (float)obs.rock_list[i2].height / 6},
                                                    (Vector3){obs.rock_list[i2].center_pos.x + (float)obs.rock_list[i2].height / 6,
                                                              obs.rock_list[i2].center_pos.y + (float)obs.rock_list[i2].height / 2,
                                                              obs.rock_list[i2].center_pos.z + (float)obs.rock_list[i2].height / 6}},
                                                ship_inst.position, ship_inst.sphere_hitbox_radius))
                    {
                        ship_inst.is_spawn_valid = false;
                    }
                    break;
                case 2:
                    if (CheckCollisionSpheres(obs.rock_list[i2].center_pos, obs.rock_list[i2].height, ship_inst.position, ship_inst.sphere_hitbox_radius))
                    {
                        ship_inst.is_spawn_valid = false;
                    }
                    break;
                default:
                    break;
                }
            }
            // check if ship positions overlap
            for (int i3 = 0; i3 < i; i3++)
            {
                /*if(CheckCollisionSpheres(ship_list[i3].position, ship_list[i3].sphere_hitbox_radius, ship_inst.position, ship_inst.sphere_hitbox_radius)){
                    ship_inst.is_spawn_valid = false;
                    printf("\n\ncrashed\n\n");
                }*/
                if ((ship_list[i].position.x <= ship_inst.position.x + ship_inst.sphere_hitbox_radius / 2 && ship_inst.position.x >= ship_inst.position.x - ship_inst.sphere_hitbox_radius / 2) || (ship_list[i].position.z <= ship_inst.position.z + ship_inst.sphere_hitbox_radius / 2 && ship_inst.position.z >= ship_inst.position.z - ship_inst.sphere_hitbox_radius / 2))
                {
                    ship_inst.is_spawn_valid = false;
                }
            }
        }
        // END OF SPAWN POS VALIDATION
        ship_inst.cannon = cannon_addr;
        *ship_inst.cannon = cannon_inst;
        ship_inst.cannonball = initcannonball;
        ship_inst.yaw = (float)GetRandomValue(0, 6) + (float)GetRandomValue(0, 2830) / 10000;
        ship_inst.can_move = false;
        ship_inst.can_fire = true;
        ship_inst.current_health = ship_inst.initial_health;
        ship_inst.id = i;
        ship_inst.is_destroyed = false;
        ship_inst.team = team_list[i];
        ship_inst.camera = camera_addr;
        ship_inst.camera->up = (Vector3){0, 0, 1};
        ship_inst.camera->fovy = 45.0f;
        ship_inst.camera->projection = CAMERA_PERSPECTIVE;

        if (player_count == 2)
        {
            if (i == 1)
            {
                ship_inst.movement_buttons = btns2;
            }
            else
                ship_inst.movement_buttons = btns1;
        }
        else
            ship_inst.movement_buttons = btns3;

        ship_list[i] = ship_inst;
    }
    return ship_list;
}

/**
 * @brief Creates a Ship_data structure, which holds essential information about ships, their type and team indexes.
 * @param player_count The amount of players (ships) to be generated
 * @param type_list An array of integers declaring each ship's type
 * @param team_list An array of integers declaring each ship's team
 * @param obs The Obstacles object, holding data for spawned obstacles
 * @param ship_models An array of Models with the ships' models
 * @return The generated Ship_data structure
 */
Ship_data CreateShipData(int player_count, int *type_list, int *team_list, Obstacles obs, Model *ship_models)
{
    Ship_data ship_data;
    ship_data.ship_list = SetupShips(player_count, type_list, team_list, obs, ship_models);
    ship_data.player_count = player_count;
    ship_data.type_list = type_list;
    ship_data.team_list = team_list;
    return ship_data;
}

/**
 * @brief Loads a ship from a saved game state properly.
 * @param type The ship's type
 * @param shipState A pointer to the cJSON array, which holds its saved state
 * @param playercount The amount of players who started the game (derived from the game state)
 * @return A Ship object, properly set up to continue the game
 */
Ship LoadShip(const int type, const cJSON *shipState, const int playercount)
{
    const movement_buttons btns1 = settings.player_one_buttons;
    const movement_buttons btns2 = settings.player_two_buttons;
    const movement_buttons btns3 = settings.player_indep_buttons;

    Cannon cannon_inst;
    Cannon *cannon_addr = MemAlloc(sizeof(Cannon));
    Camera *camera_addr = MemAlloc(sizeof(Camera));
    Ship ship;

    const cJSON *id = cJSON_GetArrayItem(shipState, 0);
    ship.id = id->valueint;

    const cJSON *teamnum = cJSON_GetArrayItem(shipState, 2);
    ship.team = teamnum->valueint;

    if (playercount == 2)
        ship.movement_buttons = ship.id == 0 ? btns1 : btns2;
    else
        ship.movement_buttons = btns3;

    const cJSON *yaw = cJSON_GetArrayItem(shipState, 3);
    ship.yaw = (float)yaw->valuedouble;

    const cJSON *positionState = cJSON_GetArrayItem(shipState, 4);
    float x = (float)cJSON_GetArrayItem(positionState, 0)->valuedouble;
    float y = (float)cJSON_GetArrayItem(positionState, 1)->valuedouble;
    float z = (float)cJSON_GetArrayItem(positionState, 2)->valuedouble;
    const Vector3 position = (Vector3){x, y, z};
    ship.position = position;

    const cJSON *prev_pos_state = cJSON_GetArrayItem(shipState, 5);
    x = (float)cJSON_GetArrayItem(prev_pos_state, 0)->valuedouble;
    y = (float)cJSON_GetArrayItem(prev_pos_state, 1)->valuedouble;
    z = (float)cJSON_GetArrayItem(prev_pos_state, 2)->valuedouble;
    const Vector3 prev_pos = (Vector3){x, y, z};
    ship.prev_position = prev_pos;

    const cJSON *cannon_rel_pos = cJSON_GetArrayItem(shipState, 6);
    x = (float)cJSON_GetArrayItem(cannon_rel_pos, 0)->valuedouble;
    y = (float)cJSON_GetArrayItem(cannon_rel_pos, 1)->valuedouble;
    z = (float)cJSON_GetArrayItem(cannon_rel_pos, 2)->valuedouble;
    cannon_inst.relative_position = (Vector3){x, y, z};

    const cJSON *health = cJSON_GetArrayItem(shipState, 7);
    ship.current_health = health->valueint;

    const cJSON *prev_shot_r = cJSON_GetArrayItem(shipState, 8);
    ship.prev_shot_release = (float)prev_shot_r->valuedouble;

    const cJSON *reload_time = cJSON_GetArrayItem(shipState, 9);
    ship.time_to_reload_since_last_shot = (float)reload_time->valuedouble;

    const cJSON *is_destr = cJSON_GetArrayItem(shipState, 10);
    ship.is_destroyed = (bool)is_destr->valueint;

    initcannonball.position = (Vector3){0, 1000, 0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();
    initcannonball.has_splashed = true;
    ship.cannonball = initcannonball;

    ship.can_fire = false;
    ship.can_move = false;

    cannon_inst.rotation = Vector3Zero();
    cannon_inst.stand_model = LoadModel("resources/models/cannon_stand.glb");
    cannon_inst.rail_model = LoadModel("resources/models/cannon_rail.glb");
    ship.cannon = cannon_addr;
    *ship.cannon = cannon_inst;
    ship.cannonball = initcannonball;

    ship.camera = camera_addr;
    ship.camera->up = (Vector3){0, 0, 1};
    ship.camera->fovy = 45.0f;
    ship.camera->projection = CAMERA_PERSPECTIVE;

    if (type == 0)
    {
        cannon_inst.relative_position = (Vector3){0, -8, 20};
        ship.model = LoadModel("resources/models/ship1edited.glb");
        ship.max_accel = MAX_ACCEL * 0.7f;
        ship.min_accel = MIN_ACCEL * 0.8f;
        ship.accel_step = ACCEL_STEP * 0.9f;
        ship.cannonball_power_coefficient = 0.7f;

        ship.camera_distance_vector_tp = (Vector3){0.0f, 35.0f, -70.0f};
        ship.camera_distance_vector_fp = (Vector3){0.0f, -4.0f, 16.0f};
        ship.sphere_hitbox_radius = 15;
        ship.initial_health = 4;
    }
    // init for small ship
    else
    {
        cannon_inst.relative_position = (Vector3){0, 1, 7};
        ship.model = LoadModel("resources/models/ship2edited.glb");
        ship.max_accel = MAX_ACCEL * 1.5f;
        ship.min_accel = MIN_ACCEL * 1.2f;
        ship.accel_step = ACCEL_STEP * 1.1f;
        ship.cannonball_power_coefficient = 0.3f;

        ship.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
        ship.camera_distance_vector_fp = (Vector3){0.0f, 5.0f, 3.0f};
        ship.sphere_hitbox_radius = 10;
        ship.initial_health = 3;
    }

    ship.default_accel = (accel_settings){ship.min_accel, ship.min_accel, ship.min_accel, ship.min_accel, ship.min_accel, ship.min_accel, ship.min_accel};
    ship.accel = ship.default_accel;

    return ship;
}

/**
 * @brief Executes commands to properly finish the game.
 * @details This specific function is used in a pthread, in order to NOT interrupt the main game control. It waits for a
 * second (1000000 microseconds) and changes the screen to the game over, to announce the winner.
 * @note The function is NOT defined to return a pointer, please ignore any warnings.
 * @return Nothing, according to its usage in CheckWin procedure
 */
void *EndGame()
{
    usleep(1000000); // in microsec
    current_screen = GAME_OVER;
}

/**
 * @brief Responsible to check who the winner of a game is (either team or solo)
 * @param ship_data The Ship_data object, containing data for the ships
 */
void CheckWin(const Ship_data ship_data)
{
    if (!game_ended)
    {
        int no_team_ids[8] = {0};
        int no_team_count = 0;
        int red_team_count = 0;
        int green_team_count = 0;
        int blue_team_count = 0;
        int yellow_team_count = 0;
        for (int i = 0; i < ship_data.player_count; i++)
        {
            if (!ship_data.ship_list[i].is_destroyed)
            {
                switch (ship_data.ship_list[i].team)
                {
                case 0:
                    no_team_ids[no_team_count] = ship_data.ship_list[i].id;
                    ++no_team_count;
                    break;
                case 1:
                    ++red_team_count;
                    break;
                case 2:
                    ++blue_team_count;
                    break;
                case 3:
                    ++green_team_count;
                    break;
                case 4:
                    ++yellow_team_count;
                    break;
                }
            }
        }
        if (red_team_count == 0 && blue_team_count == 0 && green_team_count == 0 && yellow_team_count == 0 && no_team_count == 1)
        {
            // winner is no_team_ids[0]
            if (wintext == NULL) {
                wintext = (char*)malloc(sizeof(char) * 30);
            } else {
                wintext = (char*)realloc(wintext, sizeof(char) * 30);
            }
            sprintf(wintext, "player %d", no_team_ids[0] + 1);
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
        if (red_team_count == 0 && blue_team_count == 0 && green_team_count == 0 && yellow_team_count == 0 && no_team_count == 0)
        {
            // winner is no_team_ids[0]
            wintext = "no one!";
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
        if (blue_team_count == 0 && green_team_count == 0 && yellow_team_count == 0 && no_team_count == 0 && red_team_count != 0)
        {
            // winner is team red
            wintext = "team RED";
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
        if (red_team_count == 0 && green_team_count == 0 && yellow_team_count == 0 && no_team_count == 0 && blue_team_count != 0)
        {
            // winner is team blue
            wintext = "team BLUE";
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
        if (red_team_count == 0 && blue_team_count == 0 && yellow_team_count == 0 && no_team_count == 0 && green_team_count != 0)
        {
            // winner is team green
            wintext = "team GREEN";
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
        if (red_team_count == 0 && blue_team_count == 0 && green_team_count == 0 && no_team_count == 0 && yellow_team_count != 0)
        {
            // winner is team red
            wintext = "team YELLOW";
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, 0);
            pthread_detach(wait_before_end);
            game_ended = true;
        }
    }
}

/**
 * @brief Calculates the next alive ship's index number inside the ship_data structure
 * @param ship_data The Ship_data object
 * @param start_index The starting index to begin the calculations
 * @return The calculated index (integer)
 */
int FindNextAliveShipIndex(const Ship_data ship_data, const int start_index){
    int index = start_index % ship_data.player_count;
    for(int i = 0; i < 16; i++){
        if( ! ship_data.ship_list[index].is_destroyed){
            return index;
        }  
        index = (index + 1) % ship_data.player_count;
    }
    
    return -1; //arbitrary return val if no alive ship is found, will throw a sigsev
}

/**
 * @brief Responsible to CONSTANTLY check for any button press and apply the change in ships' movement and control.
 * @param ship A pointer to the Ship object required to control
 * @param fire The fire sound to be played in-game
 */
void CheckMovement(Ship *ship, const Sound fire)
{
    if (!ship->is_destroyed)
    {
        if (ship->can_move)
        {
            ship->prev_position = ship->position;
            // Checking axis movement
            if (IsKeyDown(ship->movement_buttons.forward))
            {
                ship->position = Vector3Add(ship->position,
                                            Vector3RotateByAxisAngle(
                                                (Vector3){0, 0, MOVEMENT_STEP * ship->accel.f_coefficient},
                                                (Vector3){0, 1, 0},
                                                ship->yaw));
                ship->accel.f_coefficient = (ship->accel.f_coefficient < ship->max_accel) ? (ship->accel.f_coefficient + ship->accel_step) : ship->max_accel;
            }
            if (IsKeyDown(ship->movement_buttons.backwards))
            {
                ship->position = Vector3Add(ship->position,
                                            Vector3RotateByAxisAngle(
                                                (Vector3){0, 0, -MOVEMENT_STEP * ship->accel.b_coefficient},
                                                (Vector3){0, 1, 0},
                                                ship->yaw));
                ship->accel.b_coefficient = (ship->accel.b_coefficient < ship->max_accel) ? (ship->accel.b_coefficient + ship->accel_step) : ship->max_accel;
            }
            if (IsKeyDown(ship->movement_buttons.left))
            {
                ship->yaw += MOVEMENT_STEP * ship->accel.l_coefficient * 0.02f;
                ship->accel.l_coefficient = (ship->accel.l_coefficient < ship->max_accel) ? (ship->accel.l_coefficient + ship->accel_step) : ship->max_accel;
            }
            if (IsKeyDown(ship->movement_buttons.right))
            {
                ship->yaw += -MOVEMENT_STEP * ship->accel.r_coefficient * 0.02f;
                ship->accel.r_coefficient = (ship->accel.r_coefficient < ship->max_accel) ? (ship->accel.r_coefficient + ship->accel_step) : ship->max_accel;
            }
            // Setting all acceleration coefficients back to std
            // Realistic ship physics while on water (deacceleration)
            // Might change the decrement step (more deacceleration than acceleration)
            if (IsKeyUp(ship->movement_buttons.forward))
            {
                if (ship->accel.f_coefficient > ship->min_accel)
                {
                    ship->position = Vector3Add(ship->position,
                                                Vector3RotateByAxisAngle(
                                                    (Vector3){0, 0, MOVEMENT_STEP * ship->accel.f_coefficient},
                                                    (Vector3){0, 1, 0},
                                                    ship->yaw));
                    ship->accel.f_coefficient -= DEACCEL_STEP;
                }
            }
            if (IsKeyUp(ship->movement_buttons.backwards))
            {
                if (ship->accel.b_coefficient > ship->min_accel)
                {
                    ship->position = Vector3Add(ship->position,
                                                Vector3RotateByAxisAngle(
                                                    (Vector3){0, 0, -MOVEMENT_STEP * ship->accel.b_coefficient},
                                                    (Vector3){0, 1, 0},
                                                    ship->yaw));
                    ship->accel.b_coefficient -= DEACCEL_STEP;
                }
            }
            if (IsKeyUp(ship->movement_buttons.left))
            {
                if (ship->accel.l_coefficient > ship->min_accel)
                {
                    ship->yaw += MOVEMENT_STEP * ship->accel.l_coefficient * 0.02f;
                    ship->accel.l_coefficient -= DEACCEL_STEP;
                }
            }
            if (IsKeyUp(ship->movement_buttons.right))
            {
                if (ship->accel.r_coefficient > ship->min_accel)
                {
                    ship->yaw += -MOVEMENT_STEP * ship->accel.r_coefficient * 0.02f;
                    ship->accel.r_coefficient -= DEACCEL_STEP;
                }
            }
        }

        if (ship->can_fire)
        {
            if (IsKeyDown(ship->movement_buttons.turn_cannon_left))
            {
                ship->cannon->rotation.y = (ship->cannon->rotation.y < MAX_TURN) ? ship->cannon->rotation.y + MOVEMENT_STEP / 10.0f * ship->accel.turn_l_coefficient : (float)MAX_TURN;
                ship->accel.turn_l_coefficient = (ship->accel.turn_l_coefficient < ship->max_accel) ? (ship->accel.turn_l_coefficient + ship->accel_step) : ship->max_accel;
            }
            if (IsKeyDown(ship->movement_buttons.turn_cannon_right))
            {
                ship->cannon->rotation.y = (ship->cannon->rotation.y > -MAX_TURN) ? (ship->cannon->rotation.y - MOVEMENT_STEP / 10.0f * ship->accel.turn_r_coefficient) : (float)-MAX_TURN;
                ship->accel.turn_r_coefficient = (ship->accel.turn_r_coefficient < ship->max_accel) ? (ship->accel.turn_r_coefficient + ship->accel_step) : ship->max_accel;
            }
            if (IsKeyUp(ship->movement_buttons.turn_cannon_left))
            {
                if (ship->accel.turn_l_coefficient > ship->min_accel)
                {
                    if (ship->cannon->rotation.y < MAX_TURN)
                    {
                        ship->cannon->rotation.y += MOVEMENT_STEP / 10 * ship->accel.turn_l_coefficient;
                        ship->accel.turn_l_coefficient -= DEACCEL_STEP;
                    }
                    else
                    {
                        ship->accel.turn_l_coefficient = ship->min_accel;
                    }
                }
            }
            if (IsKeyUp(ship->movement_buttons.turn_cannon_right))
            {
                if (ship->accel.turn_r_coefficient > ship->min_accel)
                {
                    if (ship->cannon->rotation.y > -MAX_TURN)
                    {
                        ship->cannon->rotation.y -= MOVEMENT_STEP / 10 * ship->accel.turn_r_coefficient;
                        ship->accel.turn_r_coefficient -= DEACCEL_STEP;
                    }
                    else
                    {
                        ship->accel.turn_r_coefficient = ship->min_accel;
                    }
                }
            }
            if (IsKeyUp(ship->movement_buttons.fire))
            {
                if (ship->cannon->rotation.x <= 0)
                {
                    ship->cannon->rotation.x += MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
                }
                else
                {
                    ship->can_fire = true;
                    ship->accel.fire_coefficient = ship->min_accel;
                }
            }
        }
        if (IsKeyDown(ship->movement_buttons.fire))
        {
            if (ship->can_fire && ship->cannon->rotation.x >= -MAX_TURN_UP && (ship->cannonball.position.y < 0 || ship->cannonball.position.y > 999))
            {
                ship->cannon->rotation.x -= MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
                ship->accel.fire_coefficient = (ship->accel.fire_coefficient < ship->max_accel)
                                                   ? ship->accel.fire_coefficient + ship->accel_step
                                                   : ship->max_accel;
            }
            else if (ship->cannon->rotation.x <= 0 && !ship->can_fire)
            {
                ship->cannon->rotation.x += MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
            }
        }
        if (IsKeyReleased(ship->movement_buttons.fire))
        {
            if (ship->can_fire && (ship->cannonball.position.y < 0 || ship->cannonball.position.y > 999))
            {
                InitializeCannonball(ship);
                if (settings.enable_sfx)
                    PlaySound(fire);
                ship->can_fire = false;
                ship->prev_shot_release = ship->cannon->rotation.x;
                // assuming that the projectiles initial and final heights are the same, which they nearly are
                // Angle theta is current_player_ship.prev_shot_release, max angle theta is 80deg or max_turn_up
                // u0 is initial vel 1.25f - current_player_ship.cannonball_power_coefficient * current_player_ship.cannon->rotation.x
                // initial h is current_player_ship.position.y + current_player_ship.cannon->relative_position.y
                // g is -current_player_ship.cannonball.accel.y
                float u0 = 1.25f - ship->cannonball_power_coefficient * ship->cannon->rotation.x;
                float theta = ship->prev_shot_release;
                float init_h = ship->position.y + ship->cannon->relative_position.y;
                float g = ship->cannonball.accel.y * 58; // so that it tends to g im guessing
                // float T = u0 * sin(theta) / g + sqrt(2 * init_h / g + pow(u0 * sin(theta) / g, 2));
                float T = 2 * u0 * sin(theta) / g;
                ship->time_to_reload_since_last_shot = T;
            }
        }
        if (ship->time_to_reload_since_last_shot >= 0)
        { // this should be run every frame
            ship->time_to_reload_since_last_shot -= GetFrameTime();
        }
    }
}

/**
 * @brief Initializes the cannonball for the Ship object to use in-game.
 * @param ship A pointer to the Ship object to initialize its cannonball
 */
void InitializeCannonball(Ship *ship)
{
    if (!ship->is_destroyed)
    {
        ship->cannonball.position = Vector3Add(
            ship->position,
            Vector3RotateByAxisAngle(ship->cannon->relative_position, (Vector3){0, 1, 0}, ship->yaw));
        // see comments on the transform of cannon rail
        Matrix speed_transform_matrix = MatrixMultiply(MatrixRotateX(ship->cannon->rotation.x), MatrixRotateY(ship->yaw + ship->cannon->rotation.y));
        ship->cannonball.velocity = Vector3Transform((Vector3){0, 0, 1.25f - ship->cannonball_power_coefficient * ship->cannon->rotation.x}, speed_transform_matrix);
        ship->cannonball.accel = (Vector3){0, -0.005f, 0};
        ship->cannonball.has_splashed = false;
        ship->cannonball.has_hit_enemy = false;
    }
}

/**
 * @brief Updates the given cannonball's firing state.
 * @param cannonball The Cannonball object to update
 * @param splash The splash sound to be played in-game, when the cannonball reaches the water
 * @param splash_anim The splash animation to be played in-game, when the cannonball reaches the water
 */
void UpdateCannonballState(Cannonball *cannonball, const Sound splash, Animation *splash_anim)
{
    if (cannonball->position.y < 0.0f)
    {
        if (!cannonball->has_splashed)
        {
            StartAnim(splash_anim, Vector3Add(cannonball->position, (Vector3){0, splash_anim->tex.height / 4, 0}));
            if (settings.enable_sfx)
            {
                PlaySound(splash);
            }
            cannonball->has_splashed = true;
        }
    }
    else
    {
        cannonball->position = Vector3Add(cannonball->position, cannonball->velocity);
        cannonball->velocity = Vector3Add(cannonball->velocity, cannonball->accel);
    }
}

/**
 * @brief Updates the given Ship's camera to adjust to any movement changes.
 * @param ship A pointer to the Ship to update
 * @param first_person Declares if the camera is in first or third person
 */
void UpdateShipCamera(const Ship *ship, const bool first_person)
{
    if (!ship->is_destroyed)
    {
        if (first_person)
        {
            // first person
            ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_fp, (Vector3){0, 1, 0}, ship->yaw));
            // target the end of the vector pointing 50 units in front of where the ship is facing
            ship->camera->target = Vector3Add(ship->position, Vector3RotateByAxisAngle((Vector3){0, ship->camera_distance_vector_fp.y, 100}, (Vector3){0, 1, 0}, ship->yaw));
            ship->camera->up = (Vector3){0, 1, 0};
        }
        else
        {
            // third person
            if (Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw)).x < -375 || Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw)).z < -375 || Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw)).x > 375 || Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw)).z > 375)
            {
                // hardcoded bounds vals because game_bounds is 325 instead of 375 which is the extent of the skybox
                // camera will be above the ships center which cant go OOB
                ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle((Vector3){0, ship->camera_distance_vector_tp.y, 10}, (Vector3){0, 1, 0}, ship->yaw));
                ship->camera->target = Vector3Add(ship->position, Vector3RotateByAxisAngle((Vector3){0, ship->camera_distance_vector_tp.y, 100}, (Vector3){0, 1, 0}, ship->yaw));
                ship->camera->up = (Vector3){0, 1, 0};
            }
            else
            {
                ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw));
                ship->camera->target = Vector3Add(ship->position, (Vector3){0, 15, 0});
                ship->camera->up = (Vector3){0, 1, 0};
            }
        }
    }
}

/**
 * @brief Checks for any kind of interaction between hitboxes (ships, cannonballs).
 * @param player_ship The current Ship playing
 * @param enemy_ship The enemy Ship
 * @param explosion The explosion sound to be played in-game when an interaction occurs
 * @param obstacles The Obstacles object, holding data for spawned obstacles
 * @param explosion_anim The explosion animation to be played in-game when an interaction occurs
 */
void CheckHit(Ship *player_ship, Ship *enemy_ship, const Sound explosion, const Obstacles obstacles, Animation *explosion_anim)
{
    // adding small delay before stopping game to improve game feel.
    if (CheckCollisionSpheres(enemy_ship->position, enemy_ship->sphere_hitbox_radius, player_ship->cannonball.position, 1))
    {
        if ((player_ship->team != enemy_ship->team || player_ship->team == 0 || enemy_ship->team == 0) && !enemy_ship->is_destroyed)
        { // check if ships are on diff teams
            if (player_ship->cannonball.has_hit_enemy == false)
            {
                enemy_ship->current_health -= 1;
                StartAnim(explosion_anim, enemy_ship->position);
                if (settings.enable_sfx)
                    PlaySound(explosion);
                player_ship->cannonball.has_hit_enemy = true;
                if (enemy_ship->current_health <= 0)
                {
                    enemy_ship->is_destroyed = true;
                }
            }
        }
    }

    // Destroy player if players crash into each other
    if (CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, enemy_ship->position, enemy_ship->sphere_hitbox_radius))
    {
        if(!enemy_ship->is_destroyed){
            if (settings.enable_sfx)
                PlaySound(explosion);
            player_ship->is_destroyed = true;
            enemy_ship->is_destroyed = true;
        }
    }

    // Destroy player if a player hits and island or rock
    for (int i = 0; i < obstacles.island_count; i++)
    {
        if (CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, obstacles.island_list[i].center_pos, obstacles.island_list[i].radius))
        {
            if (settings.enable_sfx && ! player_ship->is_destroyed)
                PlaySound(explosion);
            player_ship->is_destroyed = true;
        }
    }
    for (int i = 0; i < obstacles.rock_count; i++)
    {
        switch (obstacles.rock_list[i].geometry_id)
        {
        case 1: // cube
            // assuming the origin of the mesh from genmeshcube is the geometrical center, where its relative coordinates are (0,0,0) when drawn in other words
            if (CheckCollisionBoxSphere((BoundingBox){
                                            (Vector3){obstacles.rock_list[i].center_pos.x - obstacles.rock_list[i].height / 6,
                                                      obstacles.rock_list[i].center_pos.y - obstacles.rock_list[i].height / 2,
                                                      obstacles.rock_list[i].center_pos.z - obstacles.rock_list[i].height / 6},
                                            (Vector3){obstacles.rock_list[i].center_pos.x + obstacles.rock_list[i].height / 6,
                                                      obstacles.rock_list[i].center_pos.y + obstacles.rock_list[i].height / 2,
                                                      obstacles.rock_list[i].center_pos.z + obstacles.rock_list[i].height / 6}},
                                        player_ship->position, player_ship->sphere_hitbox_radius))
            {
                if (settings.enable_sfx && ! player_ship->is_destroyed)
                    PlaySound(explosion);
                player_ship->is_destroyed = true;
            }
            break;
        case 2: // sphere
            if (CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, obstacles.rock_list[i].center_pos, obstacles.rock_list[i].height))
            {
                if (settings.enable_sfx && ! player_ship->is_destroyed)
                    PlaySound(explosion);
                player_ship->is_destroyed = true;
            }
        }
    }
}

/**
 * @brief CONSTANTLY checks if the given Ship collides with the game boundaries (SkyBox's scaled Bounding Box).
 * @param ship A pointer to the Ship to check for collision
 * @param bound The game boundaries
 */
void CheckCollisionWithBounds(Ship *ship, const BoundingBox bound)
{
    bounds_accel.b_coefficient = MIN_ACCEL;
    bounds_accel.f_coefficient = MIN_ACCEL;
    bounds_accel.l_coefficient = MIN_ACCEL;
    bounds_accel.r_coefficient = MIN_ACCEL;
    bounds_accel.fire_coefficient = ship->accel.fire_coefficient;
    bounds_accel.turn_l_coefficient = ship->accel.turn_l_coefficient;
    bounds_accel.turn_r_coefficient = ship->accel.turn_r_coefficient;

    if (!CheckCollisionBoxSphere(bound, ship->position, ship->sphere_hitbox_radius))
    {
        ship->position = ship->prev_position; // movable pos
        ship->accel = bounds_accel;
    }
}