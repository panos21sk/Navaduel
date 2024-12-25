#include "screens.h"
#include "ship.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"
#include "stdio.h" //for snprintf for debugging

screen current_screen = MAIN;
Rectangle play_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle options_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 60, 160, 40}; // diff: 40px height
Rectangle exit_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 140, 160, 40};
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

    screenShip1 = LoadRenderTexture(WIDTH / 2, HEIGHT);
    screenShip2 = LoadRenderTexture(WIDTH / 2, HEIGHT);

    // inital settings
    settings.show_reticle = false;
    settings.first_or_third_person_cam = true; // false is third person
}

void DisplayMainScreen()
{
    const Vector2 mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVADUEL", 20, 20, 30, BLUE);

        DrawRectangleRec(play_button, BLACK);
        DrawText("PLAY", play_button.x + 5, play_button.y + 10, 20, WHITE);

        DrawRectangleRec(options_button, BLACK);
        DrawText("OPTIONS", options_button.x + 5, options_button.y + 10, 20, WHITE);

        DrawRectangleRec(exit_button, BLACK);
        DrawText("EXIT", exit_button.x + 5, exit_button.y + 10, 20, WHITE);

        if (CheckCollisionPointRec(mouse_point, play_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            current_screen = GAME;
        }
        if (CheckCollisionPointRec(mouse_point, options_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            current_screen = OPTIONS;
        }
        if (CheckCollisionPointRec(mouse_point, exit_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            DeinitMainWindow(); // TODO: Causes SIGSEV: Address Boundary error
        }
    }
    EndDrawing();
}

void DisplayGameScreen(Ship *ship1, Ship *ship2, const Model water_model, const Model sky_model)
{
    DisableCursor();
    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1);
    CheckMovement(ship2);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2,  settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball);
    UpdateCannonballState(&ship2->cannonball);
    
    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    //rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0}); 
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415/2, 0}); //adjust for model being offset rotationally by 90deg
    //rotate cannon
    //!No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //!Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    //Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    //Combine transform or rotation around y axis first and then around the cannons new x axis, "i think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1-> yaw - 3.1415/2 + ship1->cannon->rotation.y)); 
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0}); 
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415/2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415/2, 0}));
    Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2-> yaw - 3.1415/2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

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

    //! Rendering:
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
        DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
        DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);
    }
    EndDrawing();
}

void DisplayGameOverScreen()
{
}

void DisplayOptionsScreen()
{
    const Vector2 mouse_point = GetMousePosition();

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

    Rectangle return_to_main_rec = {20, HEIGHT - 60, 260, 40};

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

        DrawRectangleRec(return_to_main_rec, BLACK);
        DrawText("RETURN TO MAIN MENU", return_to_main_rec.x + 5, return_to_main_rec.y + 10, 20, WHITE);
        if(CheckCollisionPointRec(mouse_point, return_to_main_rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
            current_screen = MAIN;
        }
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