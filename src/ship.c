#include "ship.h"
#include "screens.h"
#include "raylib.h"
#include "raymath.h"

const struct accel_settings default_accel = {MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL, MIN_ACCEL};

Ship ship1;
Ship ship2;
Cannon cannon1;
Cannon cannon2;
Cannonball initcannonball;
Camera camera1 = {0};
Camera camera2 = {0};

void SetupShips() {
    //Variable init
    const struct movement_buttons btns1 = {KEY_D, KEY_A, KEY_W, KEY_S, KEY_E, KEY_Q, KEY_SPACE};
    const struct movement_buttons btns2 = {KEY_RIGHT, KEY_LEFT, KEY_UP, KEY_DOWN, KEY_APOSTROPHE, KEY_SEMICOLON, KEY_ENTER};

    //Cannonball init
    initcannonball.position = (Vector3){0,-10,0};
    initcannonball.velocity = Vector3Zero();
    initcannonball.accel = Vector3Zero();

    //cannon for ship1
    cannon1.relative_position = (Vector3){0, -8, 28};
    cannon1.rotation = (Vector3){0,0,0};
    cannon1.stand_model = LoadModel("resources/models/cannon_stand.glb");
    cannon1.rail_model = LoadModel("resources/models/cannon_rail.glb");
    //Ship 1
    ship1.camera = &camera1;
    ship1.camera->position = (Vector3){25.0f, 25.0f, 0.0f}; // Camera position
    ship1.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
    ship1.camera->up = (Vector3){0.0f, 0.0f, 1.0f};		// Camera up vector (rotation towards target)
    ship1.camera->fovy = 45.0f;									// Camera field-of-view Y
    ship1.camera->projection = CAMERA_PERSPECTIVE;
    ship1.model = LoadModel("resources/models/ship1edited.glb");
    ship1.movement_buttons = btns1;
    ship1.accel = default_accel;
    ship1.position = (Vector3){0.0f, 15.0f, -50.0f};
    ship1.cannon = &cannon1;
    ship1.cannonball = initcannonball;
    ship1.yaw = 0;
    ship1.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
    ship1.camera_distance_vector_fp = (Vector3){0.0f, -4.0f, 23.0f};

    //cannon for ship2
    cannon2.relative_position = (Vector3){0, 1, 7};
    cannon2.rotation = (Vector3){0,0,0};
    cannon2.stand_model = LoadModel("resources/models/cannon_stand.glb");
    cannon2.rail_model = LoadModel("resources/models/cannon_rail.glb");
    //Ship 2
    ship2.camera = &camera2;
    ship2.camera->position = (Vector3){25.0f, 25.0f, 0.0f};// Camera position
    ship2.camera->target = (Vector3){0.0f, 0.0f, 0.0f};	// Camera looking at point
    ship2.camera->up = (Vector3){0.0f, 0.0f, 1.0f};		// Camera up vector (rotation towards target)
    ship2.camera->fovy = 45.0f;									// Camera field-of-view Y
    ship2.camera->projection = CAMERA_PERSPECTIVE;
    ship2.model = LoadModel("resources/models/ship2edited.glb");
    ship2.movement_buttons = btns2;
    ship2.accel = default_accel;
    ship2.position = (Vector3){0.0f, 6.8f, 50.0f};
    ship2.cannon = &cannon2;
    ship2.cannonball = initcannonball;
    ship2.yaw = 3.1415;
    ship2.camera_distance_vector_tp = (Vector3){0.0f, 25.0f, -50.0f};
    ship2.camera_distance_vector_fp = (Vector3){0.0f, 5.0f, 3.0f};
}

void DestroyShip(Ship* ship){
    UnloadModel(ship->model);
    UnloadModel(ship->cannon->rail_model);
    UnloadModel(ship->cannon->stand_model);
}

void CheckMovement(Ship *ship) {
    //Checking axis movement
    if(IsKeyDown(ship->movement_buttons.forward)) {
        ship->position = Vector3Add(ship->position, 
                                    Vector3RotateByAxisAngle(
                                        (Vector3){0, 0, MOVEMENT_STEP*ship->accel.f_coefficient}, 
                                        (Vector3){0, 1, 0},
                                        ship->yaw));
        ship->accel.f_coefficient = (ship->accel.f_coefficient < MAX_ACCEL) ? (ship->accel.f_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.backwards)) {
        ship->position = Vector3Add(ship->position, 
                                    Vector3RotateByAxisAngle(
                                        (Vector3){0, 0, -MOVEMENT_STEP*ship->accel.b_coefficient}, 
                                        (Vector3){0, 1, 0},
                                        ship->yaw));
        ship->accel.b_coefficient = (ship->accel.b_coefficient < MAX_ACCEL) ? (ship->accel.b_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.left)) {
        ship->yaw += MOVEMENT_STEP*ship->accel.l_coefficient*0.02;
        ship->accel.l_coefficient = (ship->accel.l_coefficient < MAX_ACCEL) ? (ship->accel.l_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.right)) {
        ship->yaw += -MOVEMENT_STEP*ship->accel.r_coefficient*0.02;
        ship->accel.r_coefficient = (ship->accel.r_coefficient < MAX_ACCEL) ? (ship->accel.r_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.turn_cannon_left)){
        ship->cannon->rotation.y = (ship->cannon->rotation.y > -MAX_TURN) ? ship->cannon->rotation.y - MOVEMENT_STEP/10*ship->accel.turn_l_coefficient : -MAX_TURN;
        ship->accel.turn_l_coefficient = (ship->accel.l_coefficient < MAX_ACCEL) ? (ship->accel.turn_l_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.turn_cannon_right)){
        ship->cannon->rotation.y = (ship->cannon->rotation.y < MAX_TURN) ? ship->cannon->rotation.y + MOVEMENT_STEP/10*ship->accel.turn_r_coefficient : MAX_TURN;
        ship->accel.turn_r_coefficient = (ship->accel.r_coefficient < MAX_ACCEL) ? (ship->accel.turn_r_coefficient + ACCEL_STEP) : MAX_ACCEL;
    }
    if(IsKeyDown(ship->movement_buttons.fire)){
        if(ship->can_fire && ship->cannon->rotation.x > -MAX_TURN_UP){
            ship->cannon->rotation.x -= MOVEMENT_STEP / 10 * ship->accel.fire_coefficient;
            ship->cannon->rotation.x = (ship->cannon->rotation.x < -MAX_TURN_UP) ? -MAX_TURN_UP : ship->cannon->rotation.x;
            ship->accel.fire_coefficient = (ship->accel.fire_coefficient < MAX_ACCEL) 
                               ? ship->accel.fire_coefficient + ACCEL_STEP 
                               : MAX_ACCEL;
        }
        else {
        if(ship->cannon->rotation.x <= 0){ship->cannon->rotation.x += MOVEMENT_STEP/10*ship->accel.fire_coefficient;}
        }
    } 

    //Setting all acceleration coefficients back to std
    //Realistic ship physics while on water (deacceleration)
    //Might change the decrement step (more deacceleration than acceleration)
    if(IsKeyUp(ship->movement_buttons.forward)) {
        if(ship->accel.f_coefficient > MIN_ACCEL) {
            ship->position = Vector3Add(ship->position, 
                                    Vector3RotateByAxisAngle(
                                        (Vector3){0, 0, MOVEMENT_STEP*ship->accel.f_coefficient}, 
                                        (Vector3){0, 1, 0},
                                        ship->yaw));
            ship->accel.f_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.backwards)) {
        if(ship->accel.b_coefficient > MIN_ACCEL) {
            ship->position = Vector3Add(ship->position, 
                                    Vector3RotateByAxisAngle(
                                        (Vector3){0, 0, -MOVEMENT_STEP*ship->accel.b_coefficient}, 
                                        (Vector3){0, 1, 0},
                                        ship->yaw));
            ship->accel.b_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.left)) {
        if(ship->accel.l_coefficient > MIN_ACCEL) {
            ship->yaw += MOVEMENT_STEP*ship->accel.l_coefficient*0.02;
            ship->accel.l_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.right)) {
        if(ship->accel.r_coefficient > MIN_ACCEL) {
            ship->yaw += -MOVEMENT_STEP*ship->accel.r_coefficient*0.02;
            ship->accel.r_coefficient -= DEACCEL_STEP;
        }
    }
    if(IsKeyUp(ship->movement_buttons.turn_cannon_left)){
        if(ship->accel.turn_l_coefficient > MIN_ACCEL) {
            if(ship->cannon->rotation.y > -MAX_TURN){
                ship->cannon->rotation.y -= MOVEMENT_STEP/10*ship->accel.turn_l_coefficient;
                ship->accel.turn_l_coefficient -= DEACCEL_STEP;
            } else {
                ship->accel.turn_l_coefficient = MIN_ACCEL;
            }
        }
    }
    if(IsKeyUp(ship->movement_buttons.turn_cannon_right)){
        if(ship->accel.turn_r_coefficient > MIN_ACCEL) {
            if(ship->cannon->rotation.y < MAX_TURN){
                ship->cannon->rotation.y += MOVEMENT_STEP/10*ship->accel.turn_r_coefficient;
                ship->accel.turn_r_coefficient -= DEACCEL_STEP;
            } else {
                ship->accel.turn_r_coefficient = MIN_ACCEL;
            }
        }
    }
    if(IsKeyReleased(ship->movement_buttons.fire)){
        if(ship->can_fire){
            InitializeCannonball(ship);
            ship->can_fire = false;
        }
    }
    if(IsKeyUp(ship->movement_buttons.fire)){
        if(ship->cannon->rotation.x <= 0){
            ship->cannon->rotation.x += MOVEMENT_STEP/10*ship->accel.fire_coefficient;
        } else {
            ship->can_fire = true;
            ship->accel.fire_coefficient = MIN_ACCEL;
        }
    }
}

//Dynamic
void InitializeCannonball(Ship* ship){
    ship->cannonball.position = Vector3Add(
                ship->position, 
                Vector3RotateByAxisAngle(ship->cannon->relative_position, (Vector3){0,1,0}, ship->yaw));
    //see commemts on the transform of cannon rail
    Matrix speed_transform_matrix = MatrixMultiply(MatrixRotateX(ship->cannon->rotation.x), MatrixRotateY(ship->yaw + ship->cannon->rotation.y));
    ship->cannonball.velocity = Vector3Transform((Vector3){0,0,1.2}, speed_transform_matrix); 
    ship->cannonball.accel = (Vector3){0, -0.005, 0};
}

void UpdateCannonballState(Cannonball* cannonball){
    if(cannonball->position.y >= -5){
        cannonball->position = Vector3Add(cannonball->position, cannonball->velocity);
        cannonball->velocity = Vector3Add(cannonball->velocity, cannonball->accel);
    }
}

void UpdateShipCamera(const Ship *ship, bool first_person) {
    if(first_person){
        //first person
        ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_fp, (Vector3){0, 1, 0}, ship->yaw));
        //target the end of the vector pointing 50 units in front of where the ship is facing
        ship->camera->target = Vector3Add(ship->position, Vector3RotateByAxisAngle((Vector3){0,ship->camera_distance_vector_fp.y,100}, (Vector3){0, 1, 0}, ship->yaw));
        ship->camera->up = (Vector3){0,1,0};
    } else {
        //third person
        ship->camera->position = Vector3Add(ship->position, Vector3RotateByAxisAngle(ship->camera_distance_vector_tp, (Vector3){0, 1, 0}, ship->yaw));
        ship->camera->target = Vector3Add(ship->position, (Vector3){0, 10, 0});
        ship->camera->up = (Vector3){0,1,0};
    }
    
}   
