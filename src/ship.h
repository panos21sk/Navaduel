#ifndef SHIP_H
#define SHIP_H

/* Import the required game headers (first party libraries) */
#include "anim.h"
#include "obstacles.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/* Import the required tool headers (third party libraries) */
#include "cJSON.h"

/* Defines the maximum acceleration factor, used for each ship type's special statistics */
#define MAX_ACCEL 1
/* Defines the maximum turn factor for every cannon */
#define MAX_TURN 3.1415/9.0f
/* Defines the maximum turn up factor for every cannon */
#define MAX_TURN_UP 3.1415/2.25f
/* Defines the acceleration factor, used for each ship type's special statistics */
#define ACCEL_STEP 0.005f
/* Defines the deacceleration factor, used for each ship type's special statistics */
#define DEACCEL_STEP 0.01f
/* Defines the minimum acceleration factor, used for each ship type's special statistics */
#define MIN_ACCEL 0.01f
/* Defines the velocity factor, used for each ship type's special statistics */
#define MOVEMENT_STEP 1.0f

/* Movement buttons structure */
typedef struct {
    int right;
    int left;
    int forward;
    int backwards;
    int turn_cannon_left;
    int turn_cannon_right;
    int fire;
} movement_buttons;

/* Acceleration settings structure */
typedef struct {
    float r_coefficient; //right
    float l_coefficient; //left
    float f_coefficient; //forwards
    float b_coefficient; //backwards
    float turn_l_coefficient;
    float turn_r_coefficient;
    float fire_coefficient;
} accel_settings;

/* Cannonball structure */
typedef struct {
    Vector3 position;
    Vector3 velocity;
    Vector3 accel;
    bool has_splashed;
    bool has_hit_enemy;
} Cannonball;

/* Cannon structure */
typedef struct {
    Vector3 relative_position;
    Vector3 rotation;
    Model stand_model;
    Model rail_model;
} Cannon;

/* Ship structure */
typedef struct {
    int id;
    int team;
    float yaw;
    movement_buttons movement_buttons;
    Vector3 position;
    Vector3 prev_position;
    Vector3 prev_position_turn;
    float prev_shot_release;
    float time_to_reload_since_last_shot;
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
    bool is_destroyed; //
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

/* Ships' data structure */
typedef struct {
    Ship* ship_list;
    int* type_list;
    int* team_list;
    int player_count;
} Ship_data;

/* Variable declarations */
extern const accel_settings default_accel;
extern Ship_data ship_data;
extern bool game_ended;
extern Cannonball initcannonball;

/* Function declarations */
Ship* SetupShips(int player_count, const int* type_list, const int* team_list, Obstacles obs, Model* ship_models);
Ship_data CreateShipData(int player_count, int* type_list, int* team_list, Obstacles obs, Model* ship_models);
Ship LoadShip(int type, const cJSON *shipState, int playercount);
void *EndGame();
void CheckWin(Ship_data ship_data);
int FindNextAliveShipIndex(Ship_data ship_data, int start_index);
void CheckMovement(Ship *ship, Sound fire);
void InitializeCannonball(Ship* ship);
void UpdateCannonballState(Cannonball* cannonball, Sound splash, Animation* splash_anim);
void UpdateShipCamera(const Ship *ship, bool first_person);
void CheckHit(Ship* player_ship, Ship* enemy_ship, Sound explosion, Obstacles obstacles, Animation* explosion_anim);
void CheckCollisionWithBounds(Ship *ship, BoundingBox bound);

#endif // SHIP_H