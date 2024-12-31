#include "ship.h"
#include "game.h"
#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "pthread.h"
#include "unistd.h"
// #include "stdlib.h" for pseudorandomness
#include <stdio.h>

#include "cJSON.h"

const accel_settings default_accel = {MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL};
accel_settings bounds_accel;


Ship ship1;
Ship ship2;
Cannon cannon1;
Cannon cannon2;
Cannonball initcannonball;

Ship* SetupShips(int player_count, int* type_list)
{
    static Ship ship_list[8]; //8 is the num of max players
    // Variable init
    const movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S, KEY_E, KEY_Q, KEY_SPACE};
    const movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_APOSTROPHE, KEY_SEMICOLON, KEY_ENTER};

    // Cannonball init
    initcannonball.position = (Vector3){0, 1000, 0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();
    initcannonball.has_splashed = true;
    initcannonball.has_hit_enemy = true;

    for(int i = 0; i < player_count; i++){
        Cannon cannon_inst;
        Cannon* cannon_addr = (Cannon*)MemAlloc(sizeof(Cannon));
        Ship ship_inst;
        Camera* camera_addr = (Camera*)MemAlloc(sizeof(Camera));

        cannon_inst.rotation = Vector3Zero();
        cannon_inst.stand_model = LoadModel("resources/models/cannon_stand.glb");
        cannon_inst.rail_model = LoadModel("resources/models/cannon_rail.glb");
        //init for big ship
        if(type_list[i] == 0){
            cannon_inst.relative_position = (Vector3){0, -8, 20};
            ship_inst.model = LoadModel("resources/models/ship1edited.glb");
            ship_inst.accel = (accel_settings){default_accel.r_coefficient / 1.2, default_accel.l_coefficient / 1.2,
                                                default_accel.f_coefficient / 1.2, default_accel.b_coefficient / 1.2,
                                                default_accel.turn_l_coefficient / 1.2, default_accel.turn_r_coefficient / 1.2,
                                                default_accel.fire_coefficient / 1.2}; 

            ship_inst.camera_distance_vector_tp = (Vector3){0.0f, 35.0f, -70.0f};
            ship_inst.camera_distance_vector_fp = (Vector3){0.0f, -4.0f, 16.0f};
            ship_inst.sphere_hitbox_radius = 15;
            ship_inst.initial_health = 4;
        } 
        //init for small ship
        else if(type_list[i] == 1){
            cannon_inst.relative_position = (Vector3){0, 1, 7};
            ship_inst.model = LoadModel("resources/models/ship2edited.glb");
            ship_inst.accel = (accel_settings){default_accel.r_coefficient * 1.2, default_accel.l_coefficient * 1.2,
                                                default_accel.f_coefficient * 1.2, default_accel.b_coefficient * 1.2,
                                                default_accel.turn_l_coefficient * 1.2, default_accel.turn_r_coefficient * 1.2,
                                                default_accel.fire_coefficient * 1.2}; 

            ship_inst.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
            ship_inst.camera_distance_vector_fp = (Vector3){0.0f, 5.0f, 3.0f};
            ship_inst.sphere_hitbox_radius = 10;
            ship_inst.initial_health = 3;
        }
        ship_inst.position = (Vector3){
                GetRandomValue(-500, 500), 17, GetRandomValue(-500, 500) //add it via ref to bounds later
            };
        ship_inst.prev_position = (Vector3){0.0f, 0.0f, 0.0f};
        ship_inst.cannon = cannon_addr;
        *ship_inst.cannon = cannon_inst;
        ship_inst.cannonball = initcannonball;
        ship_inst.yaw = GetRandomValue(0, 6) + GetRandomValue(0, 2830) / 10000;
        ship_inst.can_move = false;
        ship_inst.current_health = ship_inst.initial_health;
        ship_inst.id = i;
        ship_inst.camera = camera_addr;
        ship_inst.camera->up = (Vector3){0,0,1};
        ship_inst.camera->fovy = 45.0f;
        ship_inst.camera->projection = CAMERA_PERSPECTIVE;

        if(player_count == 2){
            if(i == 1){
                ship_inst.movement_buttons = btns2;
            } else ship_inst.movement_buttons = btns1;
        } else ship_inst.movement_buttons = btns1;

        ship_list[i] = ship_inst;
    }
    return ship_list;
}

Ship_data CreateShipData(int player_count, int* type_list){
	Ship_data ship_data;
	ship_data.ship_list = SetupShips(player_count, type_list);
	ship_data.player_count = player_count;
	ship_data.type_list = type_list;
    return ship_data;
}

void ResetShipsState(Ship_data* ship_data) {
    for(int i = 0; i < ship_data->player_count; i++){
        if(ship_data->type_list[i] == 0){
            ship_data->ship_list[i].cannon->relative_position = (Vector3){0, -8, 20};
            ship_data->ship_list[i].accel = (accel_settings){default_accel.r_coefficient / 1.2, default_accel.l_coefficient / 1.2,
                                                default_accel.f_coefficient / 1.2, default_accel.b_coefficient / 1.2,
                                                default_accel.turn_l_coefficient / 1.2, default_accel.turn_r_coefficient / 1.2,
                                                default_accel.fire_coefficient / 1.2};
        } else if(ship_data->type_list[i] == 1){
            ship_data->ship_list[i].cannon->relative_position = (Vector3){0, 1, 7};
            ship_data->ship_list[i].accel = (accel_settings){default_accel.r_coefficient * 1.2, default_accel.l_coefficient * 1.2,
                                                default_accel.f_coefficient * 1.2, default_accel.b_coefficient * 1.2,
                                                default_accel.turn_l_coefficient * 1.2, default_accel.turn_r_coefficient * 1.2,
                                                default_accel.fire_coefficient * 1.2};
        }
        ship_data->ship_list[i].position = (Vector3){
            GetRandomValue(-500, 500), 17, GetRandomValue(-500, 500) //add it via ref to bounds later
        };
        ship_data->ship_list[i].prev_position = Vector3Zero();
        ship_data->ship_list[i].current_health = ship_data->ship_list[i].initial_health;
        ship_data->ship_list[i].yaw = GetRandomValue(0, 6) + GetRandomValue(0, 2830) / 10000;
        ship_data->ship_list[i].can_move = false;
    }
}

void LoadShip(Ship *ship, const cJSON *shipState) {
    const cJSON *id = cJSON_GetArrayItem(shipState, 0);
    ship = id->valueint == 1 ? &ship1 : &ship2;

    const cJSON *yaw = cJSON_GetArrayItem(shipState, 1);
    ship->yaw = (float)yaw->valuedouble;

    const cJSON *positionState = cJSON_GetArrayItem(shipState, 2);
    const float x = (float)cJSON_GetArrayItem(positionState, 0)->valuedouble;
    const float y = (float)cJSON_GetArrayItem(positionState, 1)->valuedouble;
    const float z = (float)cJSON_GetArrayItem(positionState, 2)->valuedouble;
    const Vector3 position = {x, y, z};
    ship->position = position;

    const cJSON *cannon_rel_pos = cJSON_GetArrayItem(shipState, 3);
    Cannon *cannon = id->valueint == 1 ? &cannon1 : &cannon2;
    const float rx = (float)cJSON_GetArrayItem(cannon_rel_pos, 0)->valuedouble;
    const float ry = (float)cJSON_GetArrayItem(cannon_rel_pos, 1)->valuedouble;
    const float rz = (float)cJSON_GetArrayItem(cannon_rel_pos, 2)->valuedouble;
    cannon->relative_position = (Vector3){rx, ry, rz};
    ship->cannon = cannon;

    const cJSON *health = cJSON_GetArrayItem(shipState, 4);
    ship->current_health = health->valueint;

    initcannonball.position = (Vector3){0,1000,0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();
    initcannonball.has_splashed = true;
    ship->cannonball = initcannonball;

    ship->can_move = false;
}

void DestroyShip(const Ship* ship){
    UnloadModel(ship->model);
    UnloadModel(ship->cannon->rail_model);
    UnloadModel(ship->cannon->stand_model);
}

void CheckMovement(Ship *ship, const Sound fire, const bool sfx_en)
{
    if(ship->can_move) {
        ship->prev_position = ship->position;
        // Checking axis movement
        if (IsKeyDown(ship->movement_buttons.forward))
        {
            ship->position = Vector3Add(ship->position,
                                        Vector3RotateByAxisAngle(
                                            (Vector3){0, 0, MOVEMENT_STEP * ship->accel.f_coefficient},
                                            (Vector3){0, 1, 0},
                                            ship->yaw));
            ship->accel.f_coefficient = (ship->accel.f_coefficient < MAX_ACCEL) ? (ship->accel.f_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        if (IsKeyDown(ship->movement_buttons.backwards))
        {
            ship->position = Vector3Add(ship->position,
                                        Vector3RotateByAxisAngle(
                                            (Vector3){0, 0, -MOVEMENT_STEP * ship->accel.b_coefficient},
                                            (Vector3){0, 1, 0},
                                            ship->yaw));
            ship->accel.b_coefficient = (ship->accel.b_coefficient < MAX_ACCEL) ? (ship->accel.b_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        if (IsKeyDown(ship->movement_buttons.left))
        {
            ship->yaw += MOVEMENT_STEP * ship->accel.l_coefficient * 0.02f;
            ship->accel.l_coefficient = (ship->accel.l_coefficient < MAX_ACCEL) ? (ship->accel.l_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        if (IsKeyDown(ship->movement_buttons.right))
        {
            ship->yaw += -MOVEMENT_STEP * ship->accel.r_coefficient * 0.02f;
            ship->accel.r_coefficient = (ship->accel.r_coefficient < MAX_ACCEL) ? (ship->accel.r_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        // Setting all acceleration coefficients back to std
        // Realistic ship physics while on water (deacceleration)
        // Might change the decrement step (more deacceleration than acceleration)
        if (IsKeyUp(ship->movement_buttons.forward))
        {
            if (ship->accel.f_coefficient > MIN_ACCEL)
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
            if (ship->accel.b_coefficient > MIN_ACCEL)
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
            if (ship->accel.l_coefficient > MIN_ACCEL)
            {
                ship->yaw += MOVEMENT_STEP * ship->accel.l_coefficient * 0.02f;
                ship->accel.l_coefficient -= DEACCEL_STEP;
            }
        }
        if (IsKeyUp(ship->movement_buttons.right))
        {
            if (ship->accel.r_coefficient > MIN_ACCEL)
            {
                ship->yaw += -MOVEMENT_STEP * ship->accel.r_coefficient * 0.02f;
                ship->accel.r_coefficient -= DEACCEL_STEP;
            }
        }
    }

    if(ship->can_fire) {
        if (IsKeyDown(ship->movement_buttons.turn_cannon_left))
        {
            ship->cannon->rotation.y = (ship->cannon->rotation.y > -MAX_TURN) ? (ship->cannon->rotation.y - MOVEMENT_STEP / 10.0f * ship->accel.turn_l_coefficient) : (float)-MAX_TURN;
            ship->accel.turn_l_coefficient = (ship->accel.l_coefficient < MAX_ACCEL) ? (ship->accel.turn_l_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        if (IsKeyDown(ship->movement_buttons.turn_cannon_right))
        {
            ship->cannon->rotation.y = (ship->cannon->rotation.y < MAX_TURN) ? ship->cannon->rotation.y + MOVEMENT_STEP / 10.0f * ship->accel.turn_r_coefficient : (float)MAX_TURN;
            ship->accel.turn_r_coefficient = (ship->accel.r_coefficient < MAX_ACCEL) ? (ship->accel.turn_r_coefficient + ACCEL_STEP) : MAX_ACCEL;
        }
        if (IsKeyUp(ship->movement_buttons.turn_cannon_left))
        {
            if (ship->accel.turn_l_coefficient > MIN_ACCEL)
            {
                if (ship->cannon->rotation.y > -MAX_TURN)
                {
                    ship->cannon->rotation.y -= MOVEMENT_STEP / 10 * ship->accel.turn_l_coefficient;
                    ship->accel.turn_l_coefficient -= DEACCEL_STEP;
                }
                else
                {
                    ship->accel.turn_l_coefficient = MIN_ACCEL;
                }
            }
        }
        if (IsKeyUp(ship->movement_buttons.turn_cannon_right))
        {
            if (ship->accel.turn_r_coefficient > MIN_ACCEL)
            {
                if (ship->cannon->rotation.y < MAX_TURN)
                {
                    ship->cannon->rotation.y += MOVEMENT_STEP / 10 * ship->accel.turn_r_coefficient;
                    ship->accel.turn_r_coefficient -= DEACCEL_STEP;
                }
                else
                {
                    ship->accel.turn_r_coefficient = MIN_ACCEL;
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
                ship->accel.fire_coefficient = MIN_ACCEL;
            }
        }
    }
    if (IsKeyDown(ship->movement_buttons.fire))
    {
        if (ship->can_fire && ship->cannon->rotation.x > -MAX_TURN_UP && (ship->cannonball.position.y < 0 || ship->cannonball.position.y > 999))
        {
            ship->cannon->rotation.x -= MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
            ship->cannon->rotation.x = (ship->cannon->rotation.x < -MAX_TURN_UP) ? (float)-MAX_TURN_UP : ship->cannon->rotation.x;
            ship->accel.fire_coefficient = (ship->accel.fire_coefficient < MAX_ACCEL)
                                               ? ship->accel.fire_coefficient + ACCEL_STEP
                                               : MAX_ACCEL;
        }
        else
        {
            if (ship->cannon->rotation.x <= 0)
                ship->cannon->rotation.x += MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
        }
    }
    if (IsKeyReleased(ship->movement_buttons.fire))
    {
        if (ship->can_fire && (ship->cannonball.position.y < 0 || ship->cannonball.position.y > 999))
        {
            InitializeCannonball(ship);
            if (sfx_en)
                PlaySound(fire);
            ship->can_fire = false;
        }
    }
}

// Dynamic
void InitializeCannonball(Ship *ship)
{
    ship->cannonball.position = Vector3Add(
        ship->position,
        Vector3RotateByAxisAngle(ship->cannon->relative_position, (Vector3){0, 1, 0}, ship->yaw));
    // see commemts on the transform of cannon rail
    Matrix speed_transform_matrix = MatrixMultiply(MatrixRotateX(ship->cannon->rotation.x), MatrixRotateY(ship->yaw + ship->cannon->rotation.y));
    ship->cannonball.velocity = Vector3Transform((Vector3){0,0,1.2f}, speed_transform_matrix);
    ship->cannonball.accel = (Vector3){0, -0.005, 0};
    ship->cannonball.has_splashed = false;
    ship->cannonball.has_hit_enemy = false;
}

void UpdateCannonballState(Cannonball *cannonball, Sound splash, bool sfx_en)
{
    if (cannonball->position.y < 0.0f)
    {
        if (!cannonball->has_splashed)
        {
            if (sfx_en)
                PlaySound(splash);
            cannonball->has_splashed = true;
        }
    }
    else
    {
        cannonball->position = Vector3Add(cannonball->position, cannonball->velocity);
        cannonball->velocity = Vector3Add(cannonball->velocity, cannonball->accel);
    }
}

void UpdateShipCamera(const Ship *ship, const bool first_person)
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
        ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw));
        ship->camera->target = Vector3Add(ship->position, (Vector3){0, 10, 0});
        ship->camera->up = (Vector3){0, 1, 0};
    }
}

void *EndGame(void *arg)
{
    screen *state = (screen *)arg;
    usleep(1000000); // in microsec
    *state = GAME_OVER;
}

//TODO: Make function return int specifying player id of winner
void CheckHit(Ship *player_ship, Ship *enemy_ship, screen *state, Sound explosion, Obstacles obstacles, bool sfx_en)
{
    // adding small delay before stopping game to improve game feel. Maybe add game end animation by passing in here a pointer to the game state and changing it
    // to game_end = true for example, and then render in another way
    if (CheckCollisionSpheres(enemy_ship->position, enemy_ship->sphere_hitbox_radius, player_ship->cannonball.position, 1))
    {
        if (player_ship->cannonball.has_hit_enemy == false)
        {
            enemy_ship->current_health -= 1;
            if(sfx_en)PlaySound(explosion);
            player_ship->cannonball.has_hit_enemy = true;
            if (enemy_ship->current_health <= 0)
            {
                winner = player_ship->id;
                pthread_t wait_before_end;
                pthread_create(&wait_before_end, NULL, EndGame, state);
                pthread_detach(wait_before_end);
            }
        }
    }

    //End game if players crash into each other
    if(CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, enemy_ship->position, enemy_ship->sphere_hitbox_radius)){
        winner = 0; //NO ONE
        if(sfx_en)PlaySound(explosion);
        pthread_t wait_before_end;
        pthread_create(&wait_before_end, NULL, EndGame, state);
        pthread_detach(wait_before_end);
    }

    //End game if a player hits and island
    for(int i = 0; i < obstacles.island_count; i++){
        if(CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, obstacles.island_list[i].center_pos, obstacles.island_list[i].radius)){
            winner = enemy_ship->id;
            if(sfx_en)PlaySound(explosion);
            pthread_t wait_before_end;
            pthread_create(&wait_before_end, NULL, EndGame, state);
            pthread_detach(wait_before_end);
        }
    }
    for(int i = 0; i < obstacles.rock_count; i++){
        switch(obstacles.rock_list[i].geometry_id)
        {   case 1: //cube
            //assuming the origin of the mesh from genmeshcube is the geometrical center, where its relative coordinates are (0,0,0) when drawn in other words 
                if(CheckCollisionBoxSphere((BoundingBox){
                    (Vector3){obstacles.rock_list[i].center_pos.x - obstacles.rock_list[i].height / 6,
                            obstacles.rock_list[i].center_pos. y - obstacles.rock_list[i].height / 2,
                            obstacles.rock_list[i].center_pos.z - obstacles.rock_list[i].height / 6}, 
                    (Vector3){obstacles.rock_list[i].center_pos.x + obstacles.rock_list[i].height / 6,
                            obstacles.rock_list[i].center_pos. y + obstacles.rock_list[i].height / 2,
                            obstacles.rock_list[i].center_pos.z + obstacles.rock_list[i].height / 6}
                }, 
                player_ship->position, player_ship->sphere_hitbox_radius)){
                    winner = enemy_ship->id;
                    if(sfx_en)PlaySound(explosion);
                    pthread_t wait_before_end;
                    pthread_create(&wait_before_end, NULL, EndGame, state);
                    pthread_detach(wait_before_end);
                }
            break;
            case 2: //sphere
                if(CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, obstacles.rock_list[i].center_pos, obstacles.rock_list[i].height)){
                    winner = enemy_ship->id;
                    if(sfx_en)PlaySound(explosion);
                    pthread_t wait_before_end;
                    pthread_create(&wait_before_end, NULL, EndGame, state);
                    pthread_detach(wait_before_end);
                }
        }
    }
}

void CheckCollisionWithBounds(Ship *ship, const BoundingBox bound) {
    bounds_accel.b_coefficient = MIN_ACCEL;
    bounds_accel.f_coefficient = MIN_ACCEL;
    bounds_accel.l_coefficient = MIN_ACCEL;
    bounds_accel.r_coefficient = MIN_ACCEL;
    bounds_accel.fire_coefficient = ship->accel.fire_coefficient;
    bounds_accel.turn_l_coefficient = ship->accel.turn_l_coefficient;
    bounds_accel.turn_r_coefficient = ship->accel.turn_r_coefficient;

    if(!CheckCollisionBoxSphere(bound, ship->position, ship->sphere_hitbox_radius)) {
        ship->position = ship->prev_position; //movable pos
        ship->accel = bounds_accel;
    }
}