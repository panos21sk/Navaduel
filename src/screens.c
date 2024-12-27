#include "screens.h"
#include "ship.h"
#include <util.h>
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
Rectangle controls_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 140, 160, 40};
Rectangle exit_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 220, 160, 40};
Rectangle about_button = {(float)WIDTH - 165, (float)HEIGHT - 45, 160, 40};
Rectangle real_time_1v1_button = {(float)WIDTH / 2 - 180, (float)HEIGHT / 2 - 20, 170, 40};
Rectangle turn_based_1v1_button = {(float)WIDTH / 2 + 10, (float)HEIGHT / 2 - 20, 180, 40};
Rectangle github_jim_button = {(float)WIDTH / 2 + 90, 210, 160, 40};
Rectangle github_panos_button = {(float)WIDTH / 2 + 430, 210, 160, 40};
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
    //TODO: Read settings off a file instead of initalizing them, here
    settings.show_reticle = false;
    settings.first_or_third_person_cam = true; // false is third person
    settings.enable_bgm = true;
    settings.enable_sfx = true;
    settings.fullscreen = false;
}

void DisplayMainScreen(Sound click)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVALDUEL", 20, 20, 30, BLUE);

        AddScreenChangeBtn(play_button, "PLAY", GetMousePosition(), click, &current_screen, GAMEMODES, settings.enable_sfx);
        AddScreenChangeBtn(options_button, "OPTIONS", GetMousePosition(), click, &current_screen, OPTIONS, settings.enable_sfx);
        AddScreenChangeBtn(controls_button, "CONTROLS", GetMousePosition(), click, &current_screen, CONTROLS, settings.enable_sfx);
        AddScreenChangeBtn(about_button, "ABOUT", GetMousePosition(), click, &current_screen, ABOUT, settings.enable_sfx);

        DrawRectangleRec(exit_button, BLACK);

        if (CheckCollisionPointRec(GetMousePosition(), exit_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                CloseWindow(); // TODO: Causes SIGSEV: Address Boundary error
            }
            DrawRectangleRec(exit_button, RED);
        }
        DrawText("EXIT", (int)exit_button.x + 5, (int)exit_button.y + 10, 20, WHITE);

        EndDrawing();
    }
}
void DisplayGamemodesScreen(Sound click)
{
    mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("GAMEMODES", 20, 20, 30, BLUE);

        AddScreenChangeBtn(real_time_1v1_button, "REAL-TIME 1V1", GetMousePosition(), click, &current_screen, GAME_REAL, settings.enable_sfx);
        AddScreenChangeBtn(turn_based_1v1_button, "TURN-BASED 1V1", GetMousePosition(), click, &current_screen, GAME_TURN, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }
    EndDrawing();
}

void DisplayRealTimeGameScreen(Ship *ship1, Ship *ship2, const Model water_model, Model sky_model, Sound splash, Sound fire)
{
    DisableCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1, fire, settings.enable_sfx);
    CheckMovement(ship2, fire, settings.enable_sfx);

    // Update Camera manually
    // TODO: Find a way to get the camera behind the ship regardless of where its facing
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball, splash, settings.enable_sfx);
    UpdateCannonballState(&ship2->cannonball, splash, settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    // rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0});
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
    // rotate cannon
    //! No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //! Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1->yaw - 3.1415f / 2 + ship1->cannon->rotation.y));
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0});
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415f / 2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415f / 2, 0}));
    const Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2->yaw - 3.1415f / 2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    ship1->hitbox = GetMeshBoundingBox(ship1->model.meshes[0]);
    ship2->hitbox = GetMeshBoundingBox(ship2->model.meshes[0]);

    sky_model.transform = MatrixMultiply(sky_model.transform, MatrixScale(1000.0f, 1000.0f, 1000.0f));
    BoundingBox sky_bounding_box = GetMeshBoundingBox(sky_model.meshes[0]);
    sky_bounding_box.min = Vector3Scale(sky_bounding_box.min, 1000.0f);
    sky_bounding_box.max = Vector3Scale(sky_bounding_box.max, 1000.0f);

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            // draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            // TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(ship2->hitbox, LIME);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();

        //! DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug, 10, 30, 20, BLACK);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
            DrawBoundingBox(sky_bounding_box, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

    if (startup_counter > 0)
    {
        char *text = malloc(sizeof(char) * 2); // with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH / 4, HEIGHT / 2, 50, WHITE);
            DrawText(text, 3 * WIDTH / 4, HEIGHT / 2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    }
    else if (startup_counter == 0)
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            DrawText("Begin!", 3 * WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; // game starts
        }
        EndDrawing();
        sleep(1); //TODO: Find solution without using sleep
    }
    else
    {
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

void DisplayTurnBasedGameScreen(Ship *ship1, Ship *ship2, const Model water_model, const Model sky_model, Sound splash, Sound fire)
{
    DisableCursor();

    //! Input Handling:
    // Ship Movement
    CheckMovement(ship1, fire, settings.enable_sfx);
    CheckMovement(ship2, fire, settings.enable_sfx);

    // Update Camera manually
    UpdateShipCamera(ship1, settings.first_or_third_person_cam);
    UpdateShipCamera(ship2, settings.first_or_third_person_cam);

    UpdateCannonballState(&ship1->cannonball, splash, settings.enable_sfx);
    UpdateCannonballState(&ship2->cannonball, splash, settings.enable_sfx);

    const Rectangle splitScreenRect = {0.0f, 0.0f, (float)screenShip1.texture.width, (float)-screenShip1.texture.height};

    // rotate ships
    ship1->model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw, 0});
    ship1->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship1->yaw - 3.1415f / 2, 0}); // adjust for model being offset rotationally by 90deg
    // rotate cannon
    //! No idea why this fucking works???? Here is old approach. Cannon spun around unctrollably when combining its pitch with its ships yaw. Pls explen to mi
    //! Old approach for reference: ship1->cannon->rail_model.transform = MatrixRotateXYZ(Vector3RotateByAxisAngle(ship1->cannon->rotation, (Vector3){0,1,0}, ship1->yaw));
    // Rotating around Z instead of X to account for cannon 90deg rotation offset on display, which shuffles the x and z axes. Try setting pitch variable to rotation.z and try old approach again if time allows it
    // Combine transform or rotation around y axis first and then around the cannons new x axis, "I think"
    Matrix cannon_transform1 = MatrixMultiply(MatrixRotateZ(-ship1->cannon->rotation.x), MatrixRotateY(ship1->yaw - 3.1415f / 2 + ship1->cannon->rotation.y));
    ship1->cannon->rail_model.transform = cannon_transform1;
    ship2->model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw, 0});
    ship2->cannon->stand_model.transform = MatrixRotateXYZ((Vector3){0, ship2->yaw - 3.1415f / 2, 0});
    ship2->cannon->rail_model.transform = MatrixRotateXYZ(Vector3Add(ship2->cannon->rotation, (Vector3){0, ship2->yaw - 3.1415f / 2, 0}));
    Matrix cannon_transform2 = MatrixMultiply(MatrixRotateZ(-ship2->cannon->rotation.x), MatrixRotateY(ship2->yaw - 3.1415f / 2 + ship2->cannon->rotation.y));
    ship2->cannon->rail_model.transform = cannon_transform2;

    BeginTextureMode(screenShip1);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera1);
        {

            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            // draw skybox - need to temporarily disable backface culling because textures need to be shown from inside
            // TODO: Improve the skybox pls :(
            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();

        //! DEBUGGING
        char debug[100];
        snprintf(debug, sizeof(debug), "%f, %d", ship1->cannonball.position.y, ship1->can_fire);
        DrawText(debug, 10, 10, 20, BLACK);
    }
    EndTextureMode();

    BeginTextureMode(screenShip2);
    {
        ClearBackground(RAYWHITE);

        BeginMode3D(camera2);
        {
            DrawModel(water_model, (Vector3){-100, 0, -100}, 10.0f, WHITE);

            rlDisableBackfaceCulling();
            DrawModel(sky_model, (Vector3){0.0f, 0.0f, 0.0f}, 1000.0f, WHITE);
            rlEnableBackfaceCulling();

            DrawModel(ship1->model, ship1->position, 1.0f, WHITE);
            DrawModel(ship1->cannon->rail_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawModel(ship1->cannon->stand_model, Vector3Add(ship1->position, Vector3RotateByAxisAngle(ship1->cannon->relative_position, (Vector3){0, 1, 0}, ship1->yaw)), 5.0f, WHITE);
            DrawSphere(ship1->cannonball.position, 1, BLACK);

            DrawModel(ship2->model, ship2->position, 1.0f, WHITE);
            DrawModel(ship2->cannon->rail_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawModel(ship2->cannon->stand_model, Vector3Add(ship2->position, Vector3RotateByAxisAngle(ship2->cannon->relative_position, (Vector3){0, 1, 0}, ship2->yaw)), 5.0f, WHITE);
            DrawSphere(ship2->cannonball.position, 1, BLACK);
        }
        EndMode3D();
    }
    EndTextureMode();

    if (startup_counter > 0)
    {
        char *text = malloc(sizeof(char) * 2); // with null char
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            sprintf(text, "%d", startup_counter);
            DrawText(text, WIDTH / 4, HEIGHT / 2, 50, WHITE);
            DrawText(text, 3 * WIDTH / 4, HEIGHT / 2, 50, WHITE);
            --startup_counter;
        }
        EndDrawing();
        sleep(1);
        free(text);
    }
    else if (startup_counter == 0)
    {
        BeginDrawing();
        {
            ClearBackground(RAYWHITE);

            DrawTextureRec(screenShip1.texture, splitScreenRect, (Vector2){0, 0}, WHITE);
            DrawTextureRec(screenShip2.texture, splitScreenRect, (Vector2){WIDTH / 2.0f, 0}, WHITE);
            DrawLine(WIDTH / 2, 0, WIDTH / 2, HEIGHT, BLACK);

            DrawText("Begin!", WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            DrawText("Begin!", 3 * WIDTH / 4 - 70, HEIGHT / 2, 50, WHITE);
            ship1->can_move = true;
            ship2->can_move = true;
            --startup_counter; // game starts
        }
        EndDrawing();
        sleep(1);
    }
    else
    {
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

void DisplayControlsScreen(Sound click)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("REAL TIME: ", 5, 5, 20, BLACK);
        DrawText("Player 1 controls: ", 15, 35, 20, BLACK);
        DrawText("W/S - Move forward/backward", 25, 65, 20, BLACK);
        DrawText("A/D - Turn Ship left/right", 25, 95, 20, BLACK);
        DrawText("Q/E - Turn Cannon left/right", 25, 125, 20, BLACK);
        DrawText("Space - Fire Cannon", 25, 155, 20, BLACK);
        DrawText("Player 2 controls: ", 15, 185, 20, BLACK);
        DrawText("Up arrow/Down arrow - Move forward/backward", 25, 215, 20, BLACK);
        DrawText("Left arrow/Right arrow - Turn Ship left/right", 25, 245, 20, BLACK);
        DrawText("Semicolon/Apostrophe - Turn Cannon left/right", 25, 275, 20, BLACK);
        DrawText("Enter - Fire Cannon", 25, 305, 20, BLACK);
        DrawText("TURN BASED", 5, 335, 20, BLACK);
        DrawText("Each Player has the same controls as Player 1 in the Real time mode.", 15, 365, 20, BLACK);

        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }

    EndDrawing();
}

void DisplayOptionsScreen(Sound click, bool* bgm_en)
{
    Rectangle reticle_rec = {17, 17, WIDTH - 37, 23};
    Rectangle first_person_rec = {17, 57, WIDTH - 37, 23};
    Rectangle fullscreen_rec = {17, 97, WIDTH - 37, 23};
    Rectangle sfx_rec = {17, 137, WIDTH - 37, 23};
    Rectangle bgm_rec = {17, 177, WIDTH - 37, 23};

    bool tmp = settings.fullscreen; 

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        AddSetting(&settings.fullscreen, "FULLSCREEN:", fullscreen_rec, click, settings.enable_sfx);
        AddSetting(&settings.enable_bgm, "ENABLE BACKGROUND MUSIC:", bgm_rec, click, settings.enable_sfx);
        *bgm_en = (settings.enable_bgm) ? true : false;
        AddSetting(&settings.enable_sfx, "ENABLE SOUND EFFECTS:", sfx_rec, click, settings.enable_sfx);
        AddSetting(&settings.show_reticle, "SHOW TARGET RETICLE:", reticle_rec, click, settings.enable_sfx);
        AddSetting(&settings.first_or_third_person_cam, "FIRST PERSON", first_person_rec, click, settings.enable_sfx);
        
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }
    EndDrawing();

    //detect toggle in fullscreen boolean
    if(settings.fullscreen != tmp){
        //TODO: If time allows it, find a way to remove dependency from WIDTH, HEIGHT preprocessor definitions, to fullscreen into our monitors res instead
        //TODO: of what is already defined
        // int display = GetCurrentMonitor();
        // if (IsWindowFullscreen())
        //     {
        //         // if we are full screen, then go back to the windowed size
        //         SetWindowSize(WIDTH, HEIGHT);
        //     }
        //     else
        //     {
        //         // if we are not full screen, set the window size to match the monitor we are on
        //         SetWindowSize(GetMonitorWidth(display), GetMonitorHeight(display));
        //     }
        ToggleFullscreen();
    }
}

void DisplayAboutScreen(Sound click)
{
    mouse_point = GetMousePosition();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("Gameplay", 10, 10, 100, GREEN);
        //TODO: Add details about gameplay
        DrawText("Credits", WIDTH / 2 + 10, 10, 100, GREEN);
        DrawRectangleRec((Rectangle){(float)WIDTH / 2 - 5, 0, 5, HEIGHT}, BLACK);

        // Credits
        DrawText("This game was brought to you by:", WIDTH / 2 + 10, 110, 35, BLACK);
        DrawText("Kakagiannis Dimitrios & Panagiotis Skoulis", WIDTH / 2 + 5, 165, 30, BLUE);
        DrawRectangleRec(github_jim_button, BLACK);
        DrawRectangleRec(github_panos_button, BLACK);

        if (CheckCollisionPointRec(mouse_point, github_jim_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                OpenURL("https://github.com/syseditor/");
            }
            DrawRectangleRec(github_jim_button, RED);
        }
        if (CheckCollisionPointRec(mouse_point, github_panos_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                OpenURL("https://github.com/panos21sk/");
            }
            DrawRectangleRec(github_panos_button, RED);
        }
        if (CheckCollisionPointRec(mouse_point, return_to_main_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                current_screen = MAIN;
            }
            DrawRectangleRec(return_to_main_button, RED);
        }

        DrawText("GITHUB", (int)github_jim_button.x + 5, (int)github_jim_button.y + 10, 20, WHITE);
        DrawText("GITHUB", (int)github_panos_button.x + 5, (int)github_panos_button.y + 10, 20, WHITE);

        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
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