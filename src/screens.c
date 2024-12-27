#include "screens.h"
#include "ship.h"
#include "util.h"
#include "raylib.h"
#include "game.h"
#include "rlgl.h"
#include <stdio.h> //for snprintf for debugging
#include <unistd.h>
#include <stdlib.h>

int success_save = 0;
int success_load = 0;

Vector2 mouse_point;

screen current_screen = MAIN;
screen gamemode;

Rectangle play_new_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 100, 160, 40};
Rectangle play_load_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle options_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 60, 160, 40}; // diff: 40px height
Rectangle controls_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 140, 160, 40};
Rectangle exit_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 + 220, 160, 40};
Rectangle about_button = {(float)WIDTH - 165, (float)HEIGHT - 45, 160, 40};
Rectangle real_time_1v1_button = {(float)WIDTH / 2 - 180, (float)HEIGHT / 2 - 20, 170, 40};
Rectangle turn_based_1v1_button = {(float)WIDTH / 2 + 10, (float)HEIGHT / 2 - 20, 180, 40};
Rectangle github_jim_button = {(float)WIDTH / 2 + 90, 210, 160, 40};
Rectangle github_panos_button = {(float)WIDTH / 2 + 430, 210, 160, 40};
Rectangle play_again_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle save_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 + 40, 180, 40};
Rectangle continue_game_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 - 20, 180, 40};
Rectangle exit_no_save_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 + 100, 180, 40};
Rectangle return_to_main_button = {20, HEIGHT - 60, 260, 40};
//TO BE REMOVED
Rectangle debug_game_over_menu = {(float)WIDTH-170, (float)HEIGHT/2, 160, 40};

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

void DisplayMainScreen(const Sound click)
{
    startup_counter = GAME_STARTUP_COUNTER;
    control_index = 0;
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("NAVALDUEL", 20, 20, 30, BLUE);

        AddScreenChangeBtn(play_new_button, "NEW GAME", GetMousePosition(), click, &current_screen, GAMEMODES, settings.enable_sfx);
        AddScreenChangeBtn(options_button, "OPTIONS", GetMousePosition(), click, &current_screen, OPTIONS, settings.enable_sfx);
        AddScreenChangeBtn(controls_button, "CONTROLS", GetMousePosition(), click, &current_screen, CONTROLS, settings.enable_sfx);
        AddScreenChangeBtn(about_button, "ABOUT", GetMousePosition(), click, &current_screen, ABOUT, settings.enable_sfx);
        AddScreenChangeBtn(debug_game_over_menu, "DEBUG GAME-OVER", GetMousePosition(), click, &current_screen, GAME_OVER, settings.enable_sfx);

        // LOAD GAME BUTTON
        {
            DrawRectangleRec(play_load_button, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), play_load_button))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    PlaySound(click);

                    // TESTING
                    const int size = sizeof(char)*strlen("Hello world!")+1;
                    char *info = malloc(size);
                    FILE *stateFile = fopen("game.gmst", "r");
                    fgets(info, size, stateFile);
                    success_load = !fclose(stateFile);
                    printf("%s", info);
                    free(info);
                }
                DrawRectangleRec(play_load_button, RED);
            }
            DrawText("LOAD GAME", (int)play_load_button.x + 5, (int)play_load_button.y + 10, 20, WHITE);
        }

        // EXIT BUTTON
        {
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
        }
    }
    EndDrawing();
}

void DisplayGamemodesScreen(const Sound click)
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

void DisplayGameOverScreen(const int winnerId, const Sound click)
{
    mouse_point = GetMousePosition();
    //SetupShips(); //Resets ships' state //IN-DEBUG MODE->COMMENTED
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("THE GAME IS OVER", WIDTH/2, 20, 30, BLUE);
        AddScreenChangeBtn(play_again_button, "PLAY AGAIN", mouse_point, click, &current_screen, GAMEMODES, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", mouse_point, click, &current_screen, MAIN, settings.enable_sfx);

        char *winnerstr = malloc(sizeof(char)*strlen("The winner is Player N!")+1);
        strcpy(winnerstr, "The winner is ");
        if(winnerId == 1) strcat(winnerstr, "Player 1!");
        else strcat(winnerstr, "Player 2!");
        DrawText(winnerstr, WIDTH/2, 40, 25, LIME);
        free(winnerstr);
    }
    EndDrawing();
}

void DisplayGameMenuScreen(const Sound click) {
    if(IsKeyPressed(KEY_ESCAPE)) {
        current_screen = gamemode;
        success_save = 0;
    }

    mouse_point = GetMousePosition();
    ShowCursor();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE); //prior to change

        AddScreenChangeBtn(continue_game_button, "CONTINUE GAME", mouse_point, click, &current_screen, gamemode, settings.enable_sfx);
        AddScreenChangeBtn(exit_no_save_button, "EXIT", mouse_point, click, &current_screen, MAIN, settings.enable_sfx);

        //Manually build the save button
        {
            DrawRectangleRec(save_button, BLACK);
            if (CheckCollisionPointRec(mouse_point, save_button))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    PlaySound(click);

                    // Saving ships' state (might change to JSON format)
                    FILE *stateFile = fopen("game.gmst", "w"); // gmst stands for game state, doesn't exist as a file extension
                    fprintf(stateFile, "Hello world!");
                    success_save = !fclose(stateFile);
                    //TODO: INSERT USEFUL INFO TO THE FILE T-T
                }
                DrawRectangleRec(save_button, RED);
            }
            DrawText("SAVE GAME", (int)save_button.x + 5, (int)save_button.y + 10, 20, WHITE);
            if(success_save) DrawText("Game state saved successfully", WIDTH / 2 - 170, HEIGHT-30, 20, GREEN);
        }
    }
    EndDrawing();
}

void DisplayControlsScreen(const Sound click)
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

void DisplayOptionsScreen(const Sound click, bool* bgm_en)
{
    Rectangle reticle_rec = {17, 17, WIDTH - 37, 23};
    Rectangle first_person_rec = {17, 57, WIDTH - 37, 23};
    Rectangle fullscreen_rec = {17, 97, WIDTH - 37, 23};
    Rectangle sfx_rec = {17, 137, WIDTH - 37, 23};
    Rectangle bgm_rec = {17, 177, WIDTH - 37, 23};

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

        if(settings.fullscreen){
            SetWindowSize(GetScreenWidth(), GetScreenHeight());
        } else {
            SetWindowSize(WIDTH, HEIGHT);
        }
    }
    EndDrawing();
}

void DisplayAboutScreen(const Sound click)
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