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

#define MAX_ISLAND_RADIUS 10
#define MAX_ISLANDS 7
#define MIN_ISLANDS 2

struct movement_buttons {
    int right;
    int left;
    int forward;
    int backwards;
    int turn_cannon_left;
    int turn_cannon_right;
    int fire;
};

struct accel_settings {
    float r_coefficient; //right
    float l_coefficient; //left
    float f_coefficient; //forwards
    float b_coefficient; //backwards
    float turn_l_coefficient;
    float turn_r_coefficient;
    float fire_coefficient;
};

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
    int id; //so far unused, will be in use when scaling to make players
    float yaw;
    struct accel_settings accel;
    struct movement_buttons movement_buttons;
    Vector3 position;
    Vector3 camera_distance_vector_fp;
    Vector3 camera_distance_vector_tp;
    Camera *camera;
    Model model;
    Cannon* cannon;
    Cannonball cannonball;
    BoundingBox boundary;
    bool can_fire;
    bool can_move;
    float sphere_hitbox_radius;
    int current_health;
    int initial_health;
} Ship;

extern const struct accel_settings default_accel;
extern Ship ship1;
extern Ship ship2;
extern Camera camera1;
extern Camera camera2;

void SetupShips();
void LoadShip(Ship *ship, const cJSON *shipState);
void DestroyShip(const Ship* ship);
void CheckMovement(Ship *ship, Sound fire, bool sfx_en);
void InitializeCannonball(Ship* ship);
void UpdateCannonballState(Cannonball* cannonball, Sound splash, bool sfx_en);
void UpdateShipCamera(const Ship *ship, bool first_person);
void* EndGame(void* arg);
void CheckHit(Ship* player_ship, Ship* enemy_ship, screen* state, Sound explosion, Island* island_list);

#endif // SHIP_H