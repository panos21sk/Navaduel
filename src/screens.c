#include "screens.h"
#include "ship.h"
#include "raylib.h"

screen current_screen = MAIN;
Rectangle play_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};

void displayMainScreen() {
    const Vector2 mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVALDUEL", 20, 20, 30, BLUE);
        DrawRectangleRec(play_button, BLACK);

        //TODO: Check if button was clicked. Make button collision detection into a function
        if (CheckCollisionPointRec(mouse_point, play_button) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
        {
            current_screen = GAME;
        }
    }
    EndDrawing();
}

void displayGameScreen(Ship *ship1, Ship *ship2, Model water_model, RenderTexture screenShip1, RenderTexture screenShip2) {
    DisableCursor();
    //! Input Handling:
    // Ship Movement
    checkMovement(ship1);
    checkMovement(ship2);

    //Update Camera manually
    //TODO: Find a way to get the camera behind the ship regardless of where its facing
    updateCamera(ship1, CAMERA_DISTANCE_VECTOR);
    updateCamera(ship2, CAMERA_DISTANCE_VECTOR);

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

        DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
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

        DrawRectangle(0, 0, GetScreenWidth()/2, 40, Fade(RAYWHITE, 0.8f));
        DrawText("UP/DOWN/RIGHT/LEFT to move", 10, 10, 20, DARKBLUE);
    }
    EndTextureMode();

    //! Rendering:
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){ 0, 0 }, WHITE);
        DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){ WIDTH/2.0f, 0 }, WHITE);
        DrawLine(WIDTH/2, 0, WIDTH/2, HEIGHT, BLACK);
    }
    EndDrawing();
}

void displayGameOverScreen() {

}