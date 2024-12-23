#include "ship.h"
#include "raylib.h"
#include "raymath.h"

const struct accel_settings default_accel = {MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL};

Ship ship1;
Ship ship2;
Camera camera1 = {0};
Camera camera2 = {0};

void SetupShips() {
    //Variable init
    const struct movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S};
    const struct movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN};

    //Ship 1
    ship1.camera = &camera1;
    ship1.camera->position = (Vector3){0.0f, 25.0f, 50.0f}; // Camera position
    ship1.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
    ship1.camera->up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
    ship1.camera->fovy = 45.0f;									// Camera field-of-view Y
    ship1.camera->projection = CAMERA_PERSPECTIVE;
    ship1.model = LoadModel("resources/models/ship1.glb");
    ship1.movement_buttons = btns1;
    ship1.accel = default_accel;
    ship1.position = (Vector3){0.0f, 0.0f, 0.0f};

    //Ship 2
    ship2.camera = &camera2;
    ship2.camera->position = (Vector3){25.0f, 25.0f, 0.0f};// Camera position
    ship2.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
    ship2.camera->up = (Vector3){0.0f, 0.0f, -1.0f};		// Camera up vector (rotation towards target)
    ship2.camera->fovy = 45.0f;									// Camera field-of-view Y
    ship2.camera->projection = CAMERA_PERSPECTIVE;
    ship2.model = LoadModel("resources/models/ship2.glb");
    ship2.movement_buttons = btns2;
    ship2.accel = default_accel;
    ship2.position = (Vector3){0.0f, 0.0f, 0.0f};
}

void CheckMovement(Ship *ship) {
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

void UpdateShipCamera(const Ship *ship, const Vector3 distance_vector) {
    ship->camera->position = Vector3Add(ship->position, distance_vector);
    ship->camera->target = Vector3Add(ship->position, (Vector3){0.0f, 10.0f, 0.0f});
}