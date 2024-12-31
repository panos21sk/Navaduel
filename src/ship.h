#ifndef SHIP_H
#define SHIP_H

#include "raylib.h"
#include "screens.h"
#include "obstacles.h"
#include "cJSON.h"
#define MAX_ACCEL 1
#define MAX_TURN 3.1415/9.0f
#define MAX_TURN_UP 3.1415/2.25f
#define ACCEL_STEP 0.005f
#define DEACCEL_STEP 0.01f
#define MIN_ACCEL 0.01f
#define MOVEMENT_STEP 1.0f

typedef struct {
    int right;
    int left;
    int forward;
    int backwards;
    int turn_cannon_left;
    int turn_cannon_right;
    int fire;
} movement_buttons;

typedef struct {
    float r_coefficient; //right
    float l_coefficient; //left
    float f_coefficient; //forwards
    float b_coefficient; //backwards
    float turn_l_coefficient;
    float turn_r_coefficient;
    float fire_coefficient;
} accel_settings;

typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 accel;
    bool has_splashed;
    bool has_hit_enemy;
} Cannonball;

typedef struct {
    Vector3 relative_position;
    Vector3 rotation;
    Model stand_model;
    Model rail_model;
} Cannon;

typedef struct {
    int id; 
    float yaw;
    movement_buttons movement_buttons;
    Vector3 position;
    Vector3 prev_position;
    Camera *camera;
    Model model;
    Cannon* cannon;
    Cannonball cannonball;
    BoundingBox boundary;
    bool can_fire;
    bool can_move;
    int current_health;
    //ship specific
    int initial_health;
    accel_settings accel;
    Vector3 camera_distance_vector_fp;
    Vector3 camera_distance_vector_tp;
    float sphere_hitbox_radius;
} Ship;

typedef struct {
    Ship* ship_list;
    int* type_list;
    int player_count;
} Ship_data;

extern const accel_settings default_accel;
extern Ship ship1;
extern Ship ship2;
extern Camera camera1;
extern Camera camera2;

Ship* SetupShips(int player_count, int* type_list);
Ship_data CreateShipData(int player_count, int* type_list);
void ResetShipsState(Ship_data* ship_data);
void LoadShip(Ship *ship, const cJSON *shipState);
void DestroyShip(const Ship* ship);
void CheckMovement(Ship *ship, Sound fire, bool sfx_en);
void InitializeCannonball(Ship* ship);
void UpdateCannonballState(Cannonball* cannonball, Sound splash, bool sfx_en);
void UpdateShipCamera(const Ship *ship, bool first_person);
void *EndGame(void* arg);
void CheckHit(Ship* player_ship, Ship* enemy_ship, screen* state, Sound explosion, Obstacles obstacles, bool sfx_en);
void CheckCollisionWithBounds(Ship *ship, BoundingBox bound);

#endif // SHIP_H