#ifndef SHIP_H
#define SHIP_H

#include "raylib.h"
#define MAX_ACCEL 1
#define ACCEL_STEP 0.005f
#define DEACCEL_STEP 0.005f //same with accel_step for now
#define MIN_ACCEL 0.01f
#define MOVEMENT_STEP 1.0f

struct movement_buttons {
    int right;
    int left;
    int forward;
    int backwards;
};

struct accel_settings {
    float r_coefficient; //right
    float l_coefficient; //left
    float f_coefficient; //forwards
    float b_coefficient; //backwards
};

typedef struct {
    Camera *camera;
    Vector3 position;
    struct accel_settings accel;
    Model model;
    struct movement_buttons movement_buttons;
} Ship;

extern void checkMovement(Ship *ship);
extern void updateCamera(const Ship *ship, Vector3 distance_vector);

#endif //SHIP_H