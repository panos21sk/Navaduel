#include "ship.h"
#include "raylib.h"
#include "raymath.h"

void checkMovement(Ship *ship) {
    //Checking axis movement
    if(IsKeyDown(ship->movement_buttons.forward)) {
        ship->position.z += -MOVEMENT_STEP*ship->accel.f_coefficient;
        ship->accel.f_coefficient = (ship->accel.f_coefficient < MAX_ACCEL) ? (ship->accel.f_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.backwards)) {
        ship->position.z += MOVEMENT_STEP*ship->accel.b_coefficient;
        ship->accel.b_coefficient = (ship->accel.b_coefficient < MAX_ACCEL) ? (ship->accel.b_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.left)) {
        ship->position.x += -MOVEMENT_STEP*ship->accel.l_coefficient;
        ship->accel.l_coefficient = (ship->accel.l_coefficient < MAX_ACCEL) ? (ship->accel.l_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.right)) {
        ship->position.x += MOVEMENT_STEP*ship->accel.r_coefficient;
        ship->accel.r_coefficient = (ship->accel.r_coefficient < MAX_ACCEL) ? (ship->accel.r_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }

    //Setting all acceleration coefficients back to std
    //Realistic ship physics while on water (deacceleration)
    //Might change the decrement step (more deacceleration than acceleration)
    if(IsKeyUp(ship->movement_buttons.forward)) {
        if(ship->accel.f_coefficient > MIN_ACCEL) {
            ship->position.z += -MOVEMENT_STEP*ship->accel.f_coefficient;
            ship->accel.f_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.backwards)) {
        if(ship->accel.b_coefficient > MIN_ACCEL) {
            ship->position.z += MOVEMENT_STEP*ship->accel.b_coefficient;
            ship->accel.b_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.left)) {
        if(ship->accel.l_coefficient > MIN_ACCEL) {
            ship->position.x += -MOVEMENT_STEP*ship->accel.l_coefficient;
            ship->accel.l_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.right)) {
        if(ship->accel.r_coefficient > MIN_ACCEL) {
            ship->position.x += MOVEMENT_STEP*ship->accel.r_coefficient;
            ship->accel.r_coefficient -= DEACCEL_STEP;
        }
    }
}

void updateCamera(const Ship *ship, const Vector3 distance_vector) {
    ship->camera->position = Vector3Add(ship->position, distance_vector);
    ship->camera->target = Vector3Add(ship->position, (Vector3){0.0f, 10.0f, 0.0f});
}