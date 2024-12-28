#include "ship.h"
#include "raylib.h"
#include "raymath.h"
#include "screens.h"
#include "pthread.h"
#include "unistd.h"
// #include "stdlib.h" for pseudorandomness
#include "cJSON.h"

const struct accel_settings default_accel = {MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL};

Ship ship1;
Ship ship2;
Cannon cannon1;
Cannon cannon2;
Cannonball initcannonball;
Camera camera1 = {0};
Camera camera2 = {0};

void SetupShips()
{
    // Variable init
    const struct movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S, KEY_E, KEY_Q, KEY_SPACE};
    const struct movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_APOSTROPHE, KEY_SEMICOLON, KEY_ENTER};

    // Cannonball init
    initcannonball.position = (Vector3){0, 1000, 0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();
    initcannonball.has_splashed = true;
    initcannonball.has_hit_enemy = true;

    // Cannon for ship1
    cannon1.relative_position = (Vector3){0, -8, 28};
    cannon1.rotation = (Vector3){0, 0, 0};
    cannon1.stand_model = LoadModel("resources/models/cannon_stand.glb");
    cannon1.rail_model = LoadModel("resources/models/cannon_rail.glb");
    // Ship 1
    ship1.id = 1;
    ship1.camera = &camera1;
    ship1.camera->position = (Vector3){25.0f, 25.0f, 0.0f}; // Camera position
    ship1.camera->target = (Vector3){0.0f, 0.0f, 0.0f};     // Camera looking at point
    ship1.camera->up = (Vector3){0.0f, 0.0f, 1.0f};         // Camera up vector (rotation towards target)
    ship1.camera->fovy = 45.0f;                             // Camera field-of-view Y
    ship1.camera->projection = CAMERA_PERSPECTIVE;
    ship1.model = LoadModel("resources/models/ship1edited.glb");
    ship1.movement_buttons = btns1;
    ship1.accel = default_accel;
    ship1.position = (Vector3){0.0f, 17.0f, -50.0f};
    ship1.cannon = &cannon1;
    ship1.cannonball = initcannonball;
    ship1.yaw = 0;
    ship1.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
    ship1.camera_distance_vector_fp = (Vector3){0.0f, -4.0f, 23.0f};
    ship1.can_move = false;
    ship1.sphere_hitbox_radius = 15;
    ship1.initial_health = 5;
    ship1.current_health = 5;

    // cannon for ship2
    cannon2.relative_position = (Vector3){0, 1, 7};
    cannon2.rotation = (Vector3){0, 0, 0};
    cannon2.stand_model = LoadModel("resources/models/cannon_stand.glb");
    cannon2.rail_model = LoadModel("resources/models/cannon_rail.glb");
    // Ship 2
    ship2.id = 2;
    ship2.camera = &camera2;
    ship2.camera->position = (Vector3){25.0f, 25.0f, 0.0f}; // Camera position
    ship2.camera->target = (Vector3){0.0f, 0.0f, 0.0f};     // Camera looking at point
    ship2.camera->up = (Vector3){0.0f, 0.0f, 1.0f};         // Camera up vector (rotation towards target)
    ship2.camera->fovy = 45.0f;                             // Camera field-of-view Y
    ship2.camera->projection = CAMERA_PERSPECTIVE;
    ship2.model = LoadModel("resources/models/ship2edited.glb");
    ship2.movement_buttons = btns2;
    ship2.accel = default_accel;
    ship2.position = (Vector3){0.0f, 7.5f, 50.0f};
    ship2.cannon = &cannon2;
    ship2.cannonball = initcannonball;
    ship2.yaw = 3.1415f;
    ship2.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
    ship2.camera_distance_vector_fp = (Vector3){0.0f, 5.0f, 3.0f};
    ship2.can_move = false;
    ship2.sphere_hitbox_radius = 10;
    ship2.initial_health = 3;
    ship2.current_health = 3;
}

void LoadShip(Ship *ship, const cJSON *shipState) {
    const struct movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S, KEY_E, KEY_Q, KEY_SPACE};
    const struct movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_APOSTROPHE, KEY_SEMICOLON, KEY_ENTER};

    const cJSON *id = cJSON_GetArrayItem(shipState, 0);
    ship->id = id->valueint;
    ship->movement_buttons = id->valueint == 1 ? btns1 : btns2;
    ship->camera = id->valueint == 1 ? &camera1 : &camera2;

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

void CheckHit(Ship *player_ship, Ship *enemy_ship, screen *state, Sound explosion)
{
    // adding small delay before stopping game to improve game feel. Maybe add game end animation by passing in here a pointer to the game state and changing it
    // to game_end = true for example, and then render in another way
    if (CheckCollisionSpheres(enemy_ship->position, enemy_ship->sphere_hitbox_radius, player_ship->cannonball.position, 1))
    {
        if (player_ship->cannonball.has_hit_enemy == false)
        {
            enemy_ship->current_health -= 1;
            PlaySound(explosion);
            player_ship->cannonball.has_hit_enemy = true;
            if (enemy_ship->current_health <= 0)
            {
                pthread_t wait_before_end;
                pthread_create(&wait_before_end, NULL, EndGame, state);
                pthread_detach(wait_before_end);
            }
        }
    }

    //End game if players crash into each other
    if(CheckCollisionSpheres(player_ship->position, player_ship->sphere_hitbox_radius, enemy_ship->position, enemy_ship->sphere_hitbox_radius)){
        PlaySound(explosion);
        pthread_t wait_before_end;
        pthread_create(&wait_before_end, NULL, EndGame, state);
        pthread_detach(wait_before_end);
    }
}

void *EndGame(void *arg)
{
    screen *state = (screen *)arg;
    usleep(1000000); // in microsec
    *state = GAME_OVER;
}
