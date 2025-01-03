#ifndef SHIP_H
#define SHIP_H

#include "raylib.h"
#include "screens.h"
#include "anim.h"
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
    int id; //d
    int team; //d
    float yaw; //d
    movement_buttons movement_buttons; //d
    Vector3 position;
    Vector3 prev_position;
    Camera *camera;
    Model model;
    Cannon* cannon;
    Cannonball cannonball;
    BoundingBox boundary;
    bool can_fire;
    bool can_move;
    bool is_spawn_valid;
    int current_health;
    accel_settings default_accel;
    accel_settings accel;
    //ship specific
    float cannonball_power_coefficient;
    int initial_health;
    float max_accel;
    float min_accel;
    float accel_step;
    Vector3 camera_distance_vector_fp;
    Vector3 camera_distance_vector_tp;
    float sphere_hitbox_radius;
} Ship;

typedef struct {
    Ship* ship_list;
    int* type_list;
    int* team_list;
    int player_count;
} Ship_data;

extern const accel_settings default_accel;
extern Ship ship1;
extern Ship ship2;
extern Camera camera1;
extern Camera camera2;
extern Ship_data ship_data;

Ship* SetupShips(int player_count, int* type_list, int* team_list, Obstacles obs);
Ship_data CreateShipData(int player_count, int* type_list, int* team_list, Obstacles obs);
Ship LoadShip(int type, const cJSON *shipState, int playercount);
void DestroyShip(Ship_data* ship_data, int id);
void CheckMovement(Ship *ship, Sound fire, bool sfx_en);
void InitializeCannonball(Ship* ship);
void UpdateCannonballState(Cannonball* cannonball, Sound splash, Animation* splash_anim,bool sfx_en);
void UpdateShipCamera(const Ship *ship, bool first_person);
void *EndGame(void* arg);
void CheckHit(Ship* player_ship, Ship* enemy_ship, screen* state, Sound explosion, Obstacles obstacles, Ship_data* ship_data_addr, bool sfx_en, Animation* explosion_anim);
void CheckCollisionWithBounds(Ship *ship, BoundingBox bound);
void CheckWin(Ship_data ship_data);

#endif // SHIP_H