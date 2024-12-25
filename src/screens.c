#include "screens.h"
#include "ship.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include <stdio.h> //for snprintf for debugging
#include <unistd.h>
#include <stdlib.h>

int startup_counter = GAME_STARTUP_COUNTER;
int gamemode;

Vector2 mouse_point;

screen current_screen = MAIN;

Rectangle play_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle options_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 60, 160, 40}; // diff: 40px height
Rectangle exit_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 140, 160, 40};
Rectangle about_button = {(float)WIDTH - 165, (float)HEIGHT - 45, 160, 40};
Rectangle real_time_1v1_button = {(float)WIDTH/2-180, (float)HEIGHT/2-20, 170, 40};
Rectangle turn_based_1v1_button = {(float)WIDTH/2+10, (float)HEIGHT/2-20, 180, 40};
Rectangle github_jim_button = {(float)WIDTH/2+90, 210, 160, 40};
Rectangle github_panos_button = {(float)WIDTH/2+430, 210, 160, 40};
Rectangle return_to_main_button = {20, HEIGHT - 60, 260, 40};

RenderTexture screenShip1;
RenderTexture screenShip2;

setting settings;

void InitMainWindow()
{
    // Tell the window to use vsync and work on high DPI displays
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
    // Create the window and OpenGL context
    InitWindow(WIDTH, HEIGHT, "NavalDuel");
    // Set Framerate
    SetTargetFPS(60);
    // So we can use ESC for the game menu
    SetExitKey(0);

    screenShip1 = LoadRenderTexture(WIDTH / 2, HEIGHT);
    screenShip2 = LoadRenderTexture(WIDTH / 2, HEIGHT);

    // initial settings
    settings.show_reticle = false;
    settings.first_or_third_person_cam = true; // false is third person
}

void DisplayMainScreen()
{
    mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVALDUEL", 20, 20, 30, BLUE);

        DrawRectangleRec(play_button, BLACK);
        DrawRectangleRec(options_button, BLACK);
        DrawRectangleRec(exit_button, BLACK);
        DrawRectangleRec(about_button, BLACK);

        if (CheckCollisionPointRec(mouse_point, play_button)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = GAMEMODES;
            DrawRectangleRec(play_button, RED);
        }
        if (CheckCollisionPointRec(mouse_point, options_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = OPTIONS;
            DrawRectangleRec(options_button, RED);
        }
        if (CheckCollisionPointRec(mouse_point, exit_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) CloseWindow(); // TODO: Causes SIGSEV: Address Boundary error
            DrawRectangleRec(exit_button, RED);
        }
        if (CheckCollisionPointRec(mouse_point, about_button)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = ABOUT;
            DrawRectangleRec(about_button, RED);
        }

        DrawText("PLAY", (int)play_button.x + 5, (int)play_button.y + 10, 20, WHITE);
        DrawText("OPTIONS", (int)options_button.x + 5, (int)options_button.y + 10, 20, WHITE);
        DrawText("EXIT", (int)exit_button.x + 5, (int)exit_button.y + 10, 20, WHITE);
        DrawText("ABOUT", (int)about_button.x + 5, (int)about_button.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DisplayGamemodesScreen() {
    mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("GAMEMODES", 20, 20, 30, BLUE);

        DrawRectangleRec(real_time_1v1_button, BLACK);
        DrawRectangleRec(turn_based_1v1_button, BLACK);
        DrawRectangleRec(return_to_main_button, BLACK);

        if(CheckCollisionPointRec(mouse_point, real_time_1v1_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = GAME_REAL;
            DrawRectangleRec(real_time_1v1_button, RED);
        }
        if(CheckCollisionPointRec(mouse_point, turn_based_1v1_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = GAME_TURN;
            DrawRectangleRec(turn_based_1v1_button, RED);
        }
        if(CheckCollisionPointRec(mouse_point, return_to_main_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = MAIN;
            DrawRectangleRec(return_to_main_button, RED);
        }

        DrawText("REAL-TIME 1V1", (int)real_time_1v1_button.x + 5, (int)real_time_1v1_button.y + 10, 20, WHITE);
        DrawText("TURN-BASED 1V1", (int)turn_based_1v1_button.x + 5, (int)turn_based_1v1_button.y + 10, 20, WHITE);
        DrawText("RETURN TO MAIN MENU", (int)return_to_main_button.x + 5, (int)return_to_main_button.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, const Model water_model, Model sky_model)
{
    DisableCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1);
    CheckMovement(ship2);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball);
    UpdateCannonballState(&ship2->cannonball);
    
    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    //rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0}); 
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415f/2, 0}); //adjust for model being offset rotationally by 90deg
    //rotate cannon
    //!No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //!Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    //Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    //Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1-> yaw - 3.1415f/2 + ship1->cannon->rotation.y));
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0}); 
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415f/2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415f/2, 0}));
    const Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2-> yaw - 3.1415f/2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    ship1->boundary = GetMeshBoundingBox(ship1->model.meshes[0]);
    ship2->boundary = GetMeshBoundingBox(ship2->model.meshes[0]);

    sky_model.transform = MatrixMultiply(sky_model.transform, MatrixScale(1000.0f, 1000.0f, 1000.0f));
    BoundingBox sky_bounding_box = GetMeshBoundingBox(sky_model.meshes[0]);
    sky_bounding_box.min = Vector3Scale(sky_bounding_box.min, 1000.0f);
    sky_bounding_box.max = Vector3Scale(sky_bounding_box.max, 1000.0f);

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);

            //draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            //TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling(); 
            DrawModel(sky_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, 
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, 
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(
                ship2->position, 
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(
                ship2->position, 
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(ship2->boundary, LIME);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        //!DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug ,10,30,20, BLACK);
        DrawText("W/S/A/D to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling(); 
            DrawModel(sky_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1.0f, WHITE);
            rlEnableBackfaceCulling();


            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, 
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, 
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(
                ship2->position, 
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(
                ship2->position, 
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        DrawText("UP/DOWN/RIGHT/LEFT to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    if (startup_counter > 0) {
        char *text = malloc(sizeof(char)*2); //with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH/4, HEIGHT/2, 50, WHITE);
            DrawText(text, 3*WIDTH/4, HEIGHT/2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    } else if (startup_counter == 0) {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH/4-70, HEIGHT/2, 50, WHITE);
            DrawText("Begin!", 3*WIDTH/4-70, HEIGHT/2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; //game starts
        }
        EndDrawing();
        sleep(1);
    } else {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);
        }
        EndDrawing();
    }
}

void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, const Model water_model, const Model sky_model) {
    DisableCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1);
    CheckMovement(ship2);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball);
    UpdateCannonballState(&ship2->cannonball);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    //rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0});
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415f/2, 0}); //adjust for model being offset rotationally by 90deg
    //rotate cannon
    //!No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //!Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    //Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    //Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1-> yaw - 3.1415f/2 + ship1->cannon->rotation.y));
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0});
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415f/2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415f/2, 0}));
    Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2-> yaw - 3.1415f/2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);

            //draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            //TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position,
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position,
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(
                ship2->position,
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(
                ship2->position,
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        //!DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug ,10,30,20, BLACK);
        DrawText("W/S/A/D to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){ 0.0f, 0.0f, 0.0f }, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position,
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position,
                Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0,1,0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(
                ship2->position,
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(
                ship2->position,
                Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0,1,0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        DrawText("UP/DOWN/RIGHT/LEFT to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    if (startup_counter > 0) {
        char *text = malloc(sizeof(char)*2); //with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH/4, HEIGHT/2, 50, WHITE);
            DrawText(text, 3*WIDTH/4, HEIGHT/2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    } else if (startup_counter == 0) {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH/4-70, HEIGHT/2, 50, WHITE);
            DrawText("Begin!", 3*WIDTH/4-70, HEIGHT/2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; //game starts
        }
        EndDrawing();
        sleep(1);
    } else {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);
        }
        EndDrawing();
    }
}

void DisplayGameOverScreen()
{
    mouse_point = GetMousePosition();
}

void DisplayOptionsScreen()
{
    mouse_point = GetMousePosition();
    Color reticle_en; // reticle enabled
    if (settings.show_reticle)
        reticle_en = BLUE;
    else
        reticle_en = RED;
    Rectangle reticle_rec = {17, 17, WIDTH - 37, 23};

    Color first_person_en; // reticle enabled
    if (settings.first_or_third_person_cam)
        first_person_en = BLUE;
    else
        first_person_en = RED;
    Rectangle first_person_rec = {17, 57, WIDTH - 37, 23};

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("TOGGLE RETICLE:", 20, 20, 20, BLACK);
        DrawRectangle(WIDTH - 40, 20, 20, 20, reticle_en);
        DrawRectangleLinesEx(reticle_rec, 3, BLACK);

        DrawText("ENABLE FIRST PERSON CAMERA:", 20, 60, 20, BLACK);
        DrawRectangle(WIDTH - 40, 60, 20, 20, first_person_en);
        DrawRectangleLinesEx(first_person_rec, 3, BLACK);

        // if rectangle containing settings is clicked, toggle setting
        if (CheckCollisionPointRec(mouse_point, reticle_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (settings.show_reticle)
            {
                settings.show_reticle = false;
            }
            else
                settings.show_reticle = true;
        }
        if (CheckCollisionPointRec(mouse_point, first_person_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            if (settings.first_or_third_person_cam)
            {
                settings.first_or_third_person_cam = false;
            }
            else
                settings.first_or_third_person_cam = true;
        }

        DrawRectangleRec(return_to_main_button, BLACK);
        if(CheckCollisionPointRec(mouse_point, return_to_main_button)){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = MAIN;
            DrawRectangleRec(return_to_main_button, RED);
        }
        DrawText("RETURN TO MAIN MENU", (int)return_to_main_button.x + 5, (int)return_to_main_button.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DisplayAboutScreen() {
    mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("Gameplay", 10, 10, 100, GREEN);
        DrawText("Credits", WIDTH/2+10, 10, 100, GREEN);
        DrawRectangleRec((Rectangle){(float)WIDTH/2-5, 0, 5, HEIGHT}, BLACK);

        //Credits
        DrawText("This game was brought to you by:", WIDTH/2+10, 110, 35, BLACK);
        DrawText("Kakagiannis Dimitrios & Panagiotis Skoulis", WIDTH/2+5, 165, 30, BLUE);
        DrawRectangleRec(github_jim_button, BLACK);
        DrawRectangleRec(github_panos_button, BLACK);
        DrawRectangleRec(return_to_main_button, BLACK);

        if(CheckCollisionPointRec(mouse_point, github_jim_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ; // TODO: Throw the user to the github account page
            DrawRectangleRec(github_jim_button, RED);
        }
        if(CheckCollisionPointRec(mouse_point, github_panos_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ; // TODO: Throw the user to the github account page
            DrawRectangleRec(github_panos_button, RED);
        }
        if(CheckCollisionPointRec(mouse_point, return_to_main_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = MAIN;
            DrawRectangleRec(return_to_main_button, RED);
        }

        DrawText("GITHUB", (int)github_jim_button.x + 5, (int)github_jim_button.y + 10, 20, WHITE);
        DrawText("GITHUB", (int)github_panos_button.x + 5, (int)github_panos_button.y + 10, 20, WHITE);
        DrawText("RETURN TO MAIN MENU", (int)return_to_main_button.x + 5, (int)return_to_main_button.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DeinitMainWindow()
{
    UnloadRenderTexture(screenShip1);
    UnloadRenderTexture(screenShip2);
    DestroyShip(&ship1);
    DestroyShip(&ship2);
    //! destroy the window and cleanup the OpenGL context
    CloseWindow();
}