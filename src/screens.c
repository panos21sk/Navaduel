#include "screens.h"
#include "ship.h"
#include "raylib.h"

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
    settings.first_or_third_person_cam = false; // false is third person
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

void DisplayGameScreen(Ship *ship1, Ship *ship2, const Model water_model)
{
    DisableCursor();
    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1);
    CheckMovement(ship2);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, CAMERA_DISTANCE_VECTOR);
    UpdateShipCamera(ship2, CAMERA_DISTANCE_VECTOR);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {
            DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
            DrawModel(ship1->model, ship1->camera->position, 1, RED);
            DrawModel(ship2->model, ship2->camera->position, 1, BLUE);
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
            DrawModel(water_model, (Vector3){-100, -1, -100}, 1.0f, BLUE);
            DrawModel(ship1->model, ship1->camera->position, 1.0f, RED);
            DrawModel(ship2->model, ship2->camera->position, 1.0f, BLUE);
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
    //! destroy the window and cleanup the OpenGL context
    CloseWindow();
}