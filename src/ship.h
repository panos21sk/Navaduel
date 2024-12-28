#ifndef SHIP_H
#define SHIP_H

#include "raylib.h"
#include "cJSON.h"
#define MAX_ACCEL 1
#define MAX_TURN 3.1415/9.0f
#define MAX_TURN_UP 3.1415/2.25f
#define ACCEL_STEP 0.005f
#define DEACCEL_STEP 0.01f
#define MIN_ACCEL 0.01f
#define MOVEMENT_STEP 1.0f

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
} Cannonball;

typedef struct {
    Vector3 relative_position;
    Vector3 rotation;
    Model stand_model;
    Model rail_model;
} Cannon;

typedef struct {
    int id; //to be saved .
    float yaw; //to be saved
    struct accel_settings accel; //NOT to be saved (default values to MIN_ACCEL)
    struct movement_buttons movement_buttons; //NOT to be saved (default values to settings->TODO) .
    Vector3 position; //to be saved
    Vector3 camera_distance_vector_fp; //NOT to be saved (default values to CAMERA_DISTANCE_VECTOR_FP)
    Vector3 camera_distance_vector_tp; //NOT to be saved (default values to CAMERA_DISTANCE_VECTOR_TP)
    Camera *camera; //NOT to be saved (default settings)
    Model model; //NOT to be saved (depends on the ID) .
    Cannon* cannon; //to be saved (state only)
    Cannonball cannonball; //NOT to be saved (std)
    BoundingBox boundary; //NOT to be saved (derived)
    bool can_fire; //NOT to be saved (default to false)
    bool can_move; //NOT to be saved (default to false)
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

#endif //SHIP_H