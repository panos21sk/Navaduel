#include "screens.h"
#include "ship.h"
#include "raylib.h"
#include "raymath.h"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int startup_counter = GAME_STARTUP_COUNTER;

screen current_screen = MAIN;

Rectangle play_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle options_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 60, 160, 40}; // diff: 40px height
Rectangle exit_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 140, 160, 40};
Rectangle about_button = {(float)WIDTH - 165, (float)HEIGHT - 45, 160, 40};
Rectangle github_jim_button = {(float)WIDTH/2+90, 210, 160, 40};
Rectangle github_panos_button = {(float)WIDTH/2+430, 210, 160, 40};

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

    // inital settings
    settings.show_reticle = false;
    settings.first_or_third_person_cam = false; // false is third person
}

void DisplayMainScreen()
{
    const Vector2 mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVALDUEL", 20, 20, 30, BLUE);

        DrawRectangleRec(play_button, BLACK);
        DrawRectangleRec(options_button, BLACK);
        DrawRectangleRec(exit_button, BLACK);
        DrawRectangleRec(about_button, BLACK);

        if (CheckCollisionPointRec(mouse_point, play_button)) {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = GAME;
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

void DisplayGameScreen(Ship *ship1, Ship *ship2, const Model water_model) {
    DisableCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1);
    CheckMovement(ship2);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, CAMERA_DISTANCE_VECTOR_TP, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, CAMERA_DISTANCE_VECTOR_TP, settings.first_or_third_person_cam);
    

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    //rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0}); //rotate ship by 90deg to match alignment 
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0}); 

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);
            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        DrawText("W/S/A/D to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, -10, -100}, 10.0f, WHITE);
            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
        }
        EndMode3D();

        DrawRectangle(0, 0, GetScreenWidth() / 2, 40, Fade(RAYWHITE, 0.8f));
        DrawText("UP/DOWN/RIGHT/LEFT to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    if (startup_counter > 0) {
        char *text = malloc(sizeof(char));
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
        if(CheckCollisionPointRec(mouse_point, return_to_main_rec)){
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) current_screen = MAIN;
            DrawRectangleRec(return_to_main_rec, RED);
        }
        DrawText("RETURN TO MAIN MENU", (int)return_to_main_rec.x + 5, (int)return_to_main_rec.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DisplayAboutScreen() {
    const Vector2 mouse_point = GetMousePosition();
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

        if(CheckCollisionPointRec(mouse_point, github_jim_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ; // TODO: Throw the user to the github account page
            DrawRectangleRec(github_jim_button, RED);
        }
        if(CheckCollisionPointRec(mouse_point, github_panos_button)) {
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) ; // TODO: Throw the user to the github account page
            DrawRectangleRec(github_panos_button, RED);
        }
        DrawText("GITHUB", (int)github_jim_button.x + 5, (int)github_jim_button.y + 10, 20, WHITE);
        DrawText("GITHUB", (int)github_panos_button.x + 5, (int)github_panos_button.y + 10, 20, WHITE);
    }
    EndDrawing();
}

void DeinitMainWindow()
{
    UnloadRenderTexture(screenShip1);
    UnloadRenderTexture(screenShip2);
    //! destroy the window and cleanup the OpenGL context
    CloseWindow();
}