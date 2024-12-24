#ifndef SHIP_H
#define SHIP_H

#include "raylib.h"
#define MAX_ACCEL 1
#define MAX_TURN 3.1415/9
#define MAX_TURN_UP 3.1415/2.25
#define ACCEL_STEP 0.005f
#define DEACCEL_STEP 0.01f //same with accel_step for now
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
    Model cannonball_model;
} Cannonball;

typedef struct {
    Vector3 relative_position;
    Vector3 rotation;
    Model stand_model;
    Model rail_model;
} Cannon;

typedef struct {
    Camera *camera;
    Vector3 position;
    float yaw;
    struct accel_settings accel;
    Model model;
    struct movement_buttons movement_buttons;
    Cannon* cannon;
    Vector3 camera_distance_vector_fp;
    Vector3 camera_distance_vector_tp;
    bool can_fire;
} Ship;

extern const struct accel_settings default_accel;
extern Ship ship1;
extern Ship ship2;
extern Camera camera1;
extern Camera camera2;

void SetupShips();
void DestroyShip(Ship* ship);
void CheckMovement(Ship *ship);
void CreateCannonball(Ship ship); //pass by value since firing a cannonball shouldnt affect the ship
void UpdateCannonballState(Cannonball* cannonball);
void UpdateShipCamera(const Ship *ship, bool first_person);

#endif //SHIP_H