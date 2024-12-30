#include "screens.h"
#include "ship.h"
#include "util.h"
#include "raylib.h"
#include "game.h"
#include "rlgl.h"
#include "cJSON.h"
#include <stdio.h> //for snprintf for debugging
#include <stdlib.h>
#include <string.h>

bool exit_window = false;

int success_save = 0;
int success_load = 1;

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
Rectangle player_count_button = {(float)WIDTH / 2 - 12, (float)HEIGHT / 2 + 32, 24, 24}; //textbox
Rectangle github_jim_button = {(float)WIDTH / 2 + 90, 210, 160, 40};
Rectangle github_panos_button = {(float)WIDTH / 2 + 430, 210, 160, 40};
Rectangle play_again_button = {(float)WIDTH / 2 - 80, (float)HEIGHT / 2 - 20, 160, 40};
Rectangle save_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 + 40, 180, 40};
Rectangle continue_game_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 - 20, 180, 40};
Rectangle exit_no_save_button = {(float)WIDTH / 2 - 100, (float)HEIGHT / 2 + 100, 180, 40};
Rectangle return_to_main_button = {20, HEIGHT - 60, 260, 40};

RenderTexture screenShip1;
RenderTexture screenShip2;
RenderTexture screenCurrentShip;

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
    screenCurrentShip = LoadRenderTexture(WIDTH, HEIGHT);
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

        // LOAD GAME BUTTON
        {
            DrawRectangleRec(play_load_button, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), play_load_button))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    PlaySound(click);

                    //TODO: (In the future) Let the user choose their desired game state
                    if(success_load == 1) {
                        //init
                        FILE *stateFile = fopen("game.json", "r");
                        if(stateFile == NULL) {
                            printf("An error occurred");
                            success_load = 0;
                            return;
                        }

                        char buffer[1024];
                        fread(buffer, 1, sizeof(buffer), stateFile);
                        cJSON *jsonstate = cJSON_Parse(buffer);
                        if(jsonstate == NULL) {
                            puts("No saved game state");
                            success_load = 0;
                            return;
                        }

                        cJSON *ship1State = cJSON_GetObjectItemCaseSensitive(jsonstate, "ship1");
                        cJSON *ship2State = cJSON_GetObjectItemCaseSensitive(jsonstate, "ship2");
                        cJSON *gamemodeSt = cJSON_GetObjectItemCaseSensitive(jsonstate, "gamemode");
                        gamemode = strcmp(gamemodeSt->valuestring, "GAME_REAL") == 0 ? GAME_REAL : GAME_TURN;
                        LoadShip(&ship1, ship1State);
                        LoadShip(&ship2, ship2State);
                        success_load = !fclose(stateFile);
                        if(success_load) {
                            is_loaded = true;
                            current_screen = gamemode;
                        }
                    }
                    else goto escape;
                }
                DrawRectangleRec(play_load_button, RED);
            }
            DrawText("LOAD GAME", (int)play_load_button.x + 5, (int)play_load_button.y + 10, 20, WHITE);
            {
                escape:
                    if (!success_load) {
                        DrawText("No saved game state", WIDTH / 2 - 107, HEIGHT-30, 20, RED);
                    }
            }
        }

        // EXIT BUTTON
        {
            DrawRectangleRec(exit_button, BLACK);

            if (CheckCollisionPointRec(GetMousePosition(), exit_button))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    PlaySound(click);
                    exit_window = true;
                    return;
                }
                DrawRectangleRec(exit_button, RED);
            }
            DrawText("EXIT", (int)exit_button.x + 5, (int)exit_button.y + 10, 20, WHITE);
        }
    }
    EndDrawing();
}


char player_count[2] = {'2', '\0'};
void DisplayGamemodesScreen(const Sound click, int* player_count_addr)
{
    mouse_point = GetMousePosition();
    control_index = 0;
    static int letter_count;
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("GAMEMODES", 20, 20, 30, BLUE);
        DrawRectangleRec(player_count_button, LIGHTGRAY);
        if(CheckCollisionPointRec(mouse_point, player_count_button)){
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            DrawRectangleLines(player_count_button.x, player_count_button.y, player_count_button.width, player_count_button.height, RED);
            int key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 50) && (key <= 56) && (letter_count < 2)) //keycode -> ascii,  lettercount -> 1 num + terminator, accept numbers 2-8
                {
                    player_count[0] = (char)key;
                    player_count[1] = '\0'; // Add null terminator at the end of the string.
                    *player_count_addr = key % 48;
                }
                key = GetCharPressed();
            }
        } else {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        DrawText(player_count, player_count_button.x + 2, player_count_button.y + 2, 20, BLACK);
        DrawText("Player Count:", player_count_button.x - 150, player_count_button.y + 2, 20, BLACK);
        DrawText("Hover mouse over the gray box and enter a number\nranging from 2-8 to change amount of players\nWorks only for Turn-Based", 
                player_count_button.x + 26, player_count_button.y + 2, 20, BLACK);

        AddScreenChangeBtn(real_time_1v1_button, "REAL-TIME 1V1", GetMousePosition(), click, &current_screen, GAME_REAL, settings.enable_sfx);
        AddScreenChangeBtn(turn_based_1v1_button, "TURN-BASED 1V1", GetMousePosition(), click, &current_screen, GAME_TURN, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }
    EndDrawing();
}

void DisplayShipSelectScreen(Sound click, Ship_data* ship_data_addr){
    DrawText("SHIP 1: Bigger, More health, Slower movement & firing.", 5, 5, 20, BLUE);
    DrawText("SHIP 2: Smaller, Less health, Faster movement & firing.", 5, 30, 20, RED); // next up is y 55
    Rectangle Rec0 = (Rectangle){5, 55, WIDTH - 10, 22};
    int type_list[8];
    for(int i = 0; i < ship_data_addr->player_count; i++){
        //outer rec
        Rec0.y = 55 + 25*i;
        DrawRectangleLines(Rec0.x - 1, Rec0.y - 1, Rec0.width + 2, Rec0.height + 2, BLACK);
        DrawText(TextFormat("Player %d:", ship_data_addr->ship_list[i].id), Rec0.x + 1, Rec0.y + 1, 20, BLACK);
        //prep for buttons
        Rectangle btn0;
        Rectangle btn1;
        bool is_ship_1_selected = true;
        btn0.x = Rec0.x + Rec0.width - 80 * 2; btn0.y = Rec0.y; btn0.height = Rec0.height; btn0.width = 80;
        btn1 = btn0; btn1.x += 80;
        if(CheckCollisionPointRec(GetMousePosition(), btn0)){
            DrawRectangleLines(btn0.x, btn0.y, btn0.width, btn0.height, RED);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) is_ship_1_selected = true;
        } else
        if(CheckCollisionPointRec(GetMousePosition(), btn1)){
            DrawRectangleLines(btn1.x, btn1.y, btn1.width, btn1.height, RED);
            if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) is_ship_1_selected = false;
        }
        DrawRectangleRec(btn0, is_ship_1_selected ? GRAY : LIGHTGRAY);
        DrawRectangleRec(btn1, is_ship_1_selected ? LIGHTGRAY : GRAY);
        type_list[i] = (int)!is_ship_1_selected; //ship1 is 0, ship2 is 1, hence the !
    }
}

void DisplayGameOverScreen(const int winnerId, const Sound click)
{
    mouse_point = GetMousePosition();
    ShowCursor();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("THE GAME IS OVER", WIDTH/2-175, 20, 35, BLUE);
        AddScreenChangeBtn(play_again_button, "PLAY AGAIN", mouse_point, click, &current_screen, GAMEMODES, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", mouse_point, click, &current_screen, MAIN, settings.enable_sfx);

        char *winnerstr = malloc(sizeof(char)*strlen("The winner is Player N!")+1);
        strcpy(winnerstr, "The winner is ");
        if(winnerId == 1) strcat(winnerstr, "Player 1!");
        else if(winnerId == 2) strcat(winnerstr, "Player 2!");
        else strcat(winnerstr, "no one!");
        DrawText(winnerstr, WIDTH/2-170, 70, 30, LIME);
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

                    // Saving ships' state
                    cJSON *jsonship1 = create_ship_json(ship1);
                    cJSON *jsonship2 = create_ship_json(ship2);
                    cJSON *jsonfinal = cJSON_CreateObject();
                    cJSON *gamemodeSt = cJSON_CreateString(gamemode == GAME_REAL ? "GAME_REAL" : "GAME_TURN");

                    cJSON_AddItemToObject(jsonfinal, "ship1", jsonship1);
                    cJSON_AddItemToObject(jsonfinal, "ship2", jsonship2);
                    cJSON_AddItemToObject(jsonfinal, "gamemode", gamemodeSt);

                    if(gamemode == GAME_TURN) {
                        cJSON *move_t = cJSON_CreateNumber(move_time);
                        cJSON *fire_t = cJSON_CreateNumber(fire_time);
                        cJSON *c_turn = cJSON_CreateNumber(current_turn->id);
                        cJSON *n_turn = cJSON_CreateNumber(next_turn->id);

                        cJSON_AddItemToObject(jsonfinal, "move_time", move_t);
                        cJSON_AddItemToObject(jsonfinal, "fire_time", fire_t);
                        cJSON_AddItemToObject(jsonfinal, "current_turn", c_turn);
                        cJSON_AddItemToObject(jsonfinal, "next_turn", n_turn);
                    }

                    const char *jsonstring = cJSON_Print(jsonfinal);
                    FILE *stateFile = fopen("game.json", "w");
                    fprintf(stateFile, "%s", jsonstring);

                    success_save = !fclose(stateFile);
                    cJSON_Delete(jsonfinal);
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
    Rectangle fps_rec = {17, 217, WIDTH - 37, 23};

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
        AddSetting(&settings.show_fps, "SHOW FPS:", fps_rec, click, settings.enable_sfx);
        
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