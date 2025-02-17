/* Import the required game headers (first party libraries) */
#include "screens.h"
#include "obstacles.h"
#include "ship.h"
#include "util.h"
#include "game.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"
#include "rlgl.h"

/* Variable initialization */
bool exit_window = false;

int success_save = 0;
int success_load = 1;

screen current_screen = MAIN;
screen gamemode;

/* Screen buttons */
Rectangle play_new_button = {WIDTH / 2 - 80, HEIGHT / 2 - 100, 160, 40};
Rectangle play_load_button = {WIDTH / 2 - 80, HEIGHT / 2 - 20, 160, 40};
Rectangle options_button = {WIDTH / 2 - 80, HEIGHT / 2 + 60, 160, 40}; // diff: 40px height
Rectangle controls_button = {WIDTH / 2 - 80, HEIGHT / 2 + 140, 160, 40};
Rectangle exit_button = {WIDTH / 2 - 80, HEIGHT / 2 + 220, 160, 40};
Rectangle about_button = {WIDTH - 165, HEIGHT - 45, 160, 40};
Rectangle real_time_1v1_button = {WIDTH / 2 - 180, HEIGHT / 2 - 20, 170, 40};
Rectangle turn_based_1v1_button = {WIDTH / 2 + 10, HEIGHT / 2 - 20, 180, 40};
Rectangle game_button = {WIDTH - 90, HEIGHT - 60, 70, 40};
Rectangle player_count_button = {WIDTH / 2 - 12, HEIGHT / 2 + 32, 24, 24}; // textbox
Rectangle github_jim_button = {WIDTH / 2 + 90, 210, 160, 40};
Rectangle github_panos_button = {WIDTH / 2 + 430, 210, 160, 40};
Rectangle play_again_button = {WIDTH / 2 - 80, HEIGHT / 2 - 20, 160, 40};
Rectangle save_button = {WIDTH / 2 - 100, HEIGHT / 2 + 40, 180, 40};
Rectangle continue_game_button = {WIDTH / 2 - 100, HEIGHT / 2 - 20, 180, 40};
Rectangle exit_no_save_button = {WIDTH / 2 - 100, HEIGHT / 2 + 100, 180, 40};
Rectangle return_to_main_button = {20, HEIGHT - 60, 260, 40};

/* Button settings (name style: movementType_player) */
Rectangle forward_btn_1 = {WIDTH/6 - 30, 120, 60, 60};
Rectangle backwards_btn_1 = {WIDTH/6 - 30, 200, 60, 60};
Rectangle right_btn_1 = {WIDTH/6 + 50, 200, 60, 60};
Rectangle left_btn_1 = {WIDTH/6 - 110, 200, 60, 60};
Rectangle cannon_right_btn_1 = {WIDTH/6 + 50, 280, 60, 60};
Rectangle cannon_left_btn_1 = {WIDTH/6 - 110, 280, 60, 60};
Rectangle fire_btn_1 = {WIDTH/6 - 30, 280, 60, 60};

Rectangle forward_btn_2 = {WIDTH/2 - 30, 120, 60, 60};
Rectangle backwards_btn_2 = {WIDTH/2 - 30, 200, 60, 60};
Rectangle right_btn_2 = {WIDTH/2 + 50, 200, 60, 60};
Rectangle left_btn_2 = {WIDTH/2 - 110, 200, 60, 60};
Rectangle cannon_right_btn_2 = {WIDTH/2 + 50, 280, 60, 60};
Rectangle cannon_left_btn_2 = {WIDTH/2 - 110, 280, 60, 60};
Rectangle fire_btn_2 = {WIDTH/2 - 30, 280, 60, 60};

Rectangle forward_btn_3 = {5.*WIDTH/6 - 30, 120, 60, 60};
Rectangle backwards_btn_3 = {5.*WIDTH/6 - 30, 200, 60, 60};
Rectangle right_btn_3 = {5.*WIDTH/6 + 50, 200, 60, 60};
Rectangle left_btn_3 = {5.*WIDTH/6 - 110, 200, 60, 60};
Rectangle cannon_right_btn_3 = {5.*WIDTH/6 + 50, 280, 60, 60};
Rectangle cannon_left_btn_3 = {5.*WIDTH/6 - 110, 280, 60, 60};
Rectangle fire_btn_3 = {5.*WIDTH/6 - 30, 280, 60, 60};

/* Screens */
RenderTexture screenShip1; //Real-time Player 1
RenderTexture screenShip2; //Real-time Player 2
RenderTexture screenCurrentShip; //Turn-based current Player

/**
 * @brief Initializes the game's window.
 */
void InitMainWindow()
{
    // Tell the window to use vsync, work on high DPI displays and add anti aliasing
    SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI | FLAG_MSAA_4X_HINT);
    // Create the window and OpenGL context
    InitWindow(WIDTH, HEIGHT, "NavalDuel"); //0,0 for native screen resolution
    // Set Framerate
    SetTargetFPS(60);
    // So we can use ESC for the game menu
    SetExitKey(0);

    screenShip1 = LoadRenderTexture((int)WIDTH / 2, (int)HEIGHT);
    screenShip2 = LoadRenderTexture((int)WIDTH / 2, (int)HEIGHT);
    screenCurrentShip = LoadRenderTexture((int)WIDTH, (int)HEIGHT);
}

/**
 * @brief Displays the game's Main screen.
 * @param click The button click sound
 * @param obstacles A pointer to the generated Obstacles instance
 * @param sand_tex The islands' sand texture
 * @param palm_tree The islands' palm tree texture
 * @param rock_tex The rocks' texture
 */
void DisplayMainScreen(const Sound click, Obstacles *obstacles, const Texture2D sand_tex, const Model palm_tree, const Texture2D rock_tex)
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

                    // Loads the most recently saved game state
                    if(!LoadGameState(obstacles, &ship_data, sand_tex, palm_tree, rock_tex)) goto escape;
                }
                DrawRectangleRec(play_load_button, RED);
            }
            DrawText("LOAD GAME", (int)play_load_button.x + 5, (int)play_load_button.y + 10, 20, WHITE);

            escape:
            {
                if (!success_load)
                {
                    DrawText("No saved game state", (int)WIDTH / 2 - 107, (int)HEIGHT - 30, 20, RED);
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

/**
 * @brief Displays the gamemode selection screen.
 * @param click The button click sound
 * @param player_count_addr A pointer to the player count integer
 * @param real_or_turn_addr A pointer to the variable declaring if the gamemode is real-time or turn-based
 */
void DisplayGamemodesScreen(const Sound click, int *player_count_addr, char* real_or_turn_addr) {
    static char player_count[2] = {'2', '\0'}; //Displayed in button
    control_index = 0;
    static int letter_count;
    player_count[0] = (char)(*player_count_addr + 48);
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("GAMEMODES", 20, 20, 30, BLUE);
        DrawRectangleRec(player_count_button, LIGHTGRAY);
        if (CheckCollisionPointRec(GetMousePosition(), player_count_button))
        {
            int key = 50;
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            DrawRectangleLines((int)player_count_button.x, (int)player_count_button.y, (int)player_count_button.width, (int)player_count_button.height, RED);
            key = GetCharPressed();
            while (key > 0)
            {
                if ((key >= 50) && (key <= 56) && (letter_count < 2)) // keycode -> ascii,  lettercount -> 1 num + terminator, accept numbers 2-8
                {
                    player_count[0] = (char)key;
                    player_count[1] = '\0'; // Add null terminator at the end of the string.
                    *player_count_addr = key % 48;
                }
                key = GetCharPressed();
            }
        }
        else
        {
            SetMouseCursor(MOUSE_CURSOR_DEFAULT);
        }
        DrawText(player_count, (int)player_count_button.x + 2, (int)player_count_button.y + 2, 20, BLACK);
        DrawText("Player Count:", (int)player_count_button.x - 150, (int)player_count_button.y + 2, 20, BLACK);
        DrawText("Hover mouse over the gray box and enter a number\nranging from 2-8 to change amount of players\nWorks only for Turn-Based",
                 (int)player_count_button.x + 26, (int)player_count_button.y + 2, 20, BLACK);

        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) if(CheckCollisionPointRec(GetMousePosition(), real_time_1v1_button)) *real_or_turn_addr = 'r';
        if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) if(CheckCollisionPointRec(GetMousePosition(), turn_based_1v1_button)) *real_or_turn_addr = 't';
        if(*player_count_addr == 2){
            AddScreenChangeBtn(real_time_1v1_button, "REAL-TIME 1V1", GetMousePosition(), click, &current_screen, SHIP_SELECT, settings.enable_sfx);
            AddScreenChangeBtn(turn_based_1v1_button, "TURN-BASED", GetMousePosition(), click, &current_screen, SHIP_SELECT, settings.enable_sfx);
        } else {
            AddScreenChangeBtn(turn_based_1v1_button, "TURN-BASED", GetMousePosition(), click, &current_screen, SHIP_SELECT, settings.enable_sfx);
        }
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }
    EndDrawing();
}

/**
 * @brief Displays the ship selection screen.
 * @param click The button click sound
 * @param type_list An array declaring each player's ship type
 * @param player_count The amount of players participating in the game
 * @param real_or_turn The variable which declares if the game is real-time or turn-based
 */
void DisplayShipSelectScreen(const Sound click, int* type_list, const int player_count, const char real_or_turn)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("SHIP 1: Bigger, More health, Slower movement & firing, More powerful shots.", 5, 5, 20, BLUE);
        DrawText("SHIP 2: Smaller, Less health, Faster movement & firing, Less powerful shots.", 5, 30, 20, RED); // next up is y 55
        Rectangle Rec0 = (Rectangle){5, 55, WIDTH - 10, 22};
        Rectangle btn0;
        Rectangle btn1;
        btn0.x = Rec0.x + Rec0.width - 80 * 2;
        btn0.y = Rec0.y;
        btn0.height = Rec0.height;
        btn0.width = 80;
        btn1 = btn0;
        btn1.x += 80;
        // size of typelist is 8
        for (int i = 0; i < player_count; i++) //dont use ship_data_addr->player_count
        {
            // outer rec
            Rec0.y = (float)(55 + 25 * i);
            DrawRectangleLines((int)Rec0.x - 1, (int)Rec0.y - 1, (int)Rec0.width + 2, (int)Rec0.height + 2, BLACK);
            DrawText(TextFormat("Player %d:", i), (int)Rec0.x + 1, (int)Rec0.y + 1, 20, BLACK);
            // prep for buttons
            btn0.y = Rec0.y;
            btn1.y = btn0.y;
            if (CheckCollisionPointRec(GetMousePosition(), btn0))
            {
                DrawRectangleLines((int)btn0.x, (int)btn0.y, (int)btn0.width, (int)btn0.height, RED);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {type_list[i] = 0; if(settings.enable_sfx) PlaySound(click);}
            }
            else DrawRectangleLines((int)btn0.x, (int)btn0.y, (int)btn0.width, (int)btn0.height, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), btn1))
            {
                DrawRectangleLines((int)btn1.x, (int)btn1.y, (int)btn1.width, (int)btn1.height, RED);
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                    {type_list[i] = 1; if(settings.enable_sfx) PlaySound(click);}
            }
            else DrawRectangleLines((int)btn1.x, (int)btn1.y, (int)btn1.width, (int)btn1.height, BLACK);
            DrawRectangleRec(btn0, ((bool)type_list[i]) ? LIGHTGRAY : GRAY);
            DrawText("SHIP 1", (int)btn0.x + 1, (int)btn0.y + 1, 20, BLACK);
            DrawRectangleRec(btn1, ((bool)type_list[i]) ? GRAY : LIGHTGRAY);
            DrawText("SHIP 2", (int)btn1.x + 1, (int)btn1.y + 1, 20, BLACK);
        }
        bool tmp = false;
        if(real_or_turn == 'r') tmp = true;
        if(player_count == 2){
            AddScreenChangeBtn(game_button, "GAME!", GetMousePosition(), click, &current_screen, (tmp) ? GAME_REAL : GAME_TURN, settings.enable_sfx);
        } else if(player_count > 2){
            AddScreenChangeBtn(game_button, "GAME!", GetMousePosition(), click, &current_screen, TEAM_SELECT, settings.enable_sfx);
        }

        AddScreenChangeBtn(return_to_main_button, "RETURN TO GAMEMODE", GetMousePosition(), click, &current_screen, GAMEMODES, settings.enable_sfx);
    }
    EndDrawing();
}

/**
 * @brief Displays the team selection screen.
 * @param click The button click sound
 * @param team_list A pointer to the array declaring each player's team
 * @param player_count The amount of players participating in the game
 * @param real_or_turn The variable which declares if the game is real-time or turn-based
 */
void DisplayTeamSelectScreen(const Sound click, int* team_list, const int player_count, const char real_or_turn){
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);
        DrawText("TEAM SELECT:", 5, 5, 20, BLUE);
        Rectangle Rec0 = (Rectangle){5, 35, WIDTH - 8, 22};
        Rectangle Btn0 = (Rectangle){WIDTH - 85, 35, 80, 20};
        for(int i = 0; i < player_count; i++){
            Rec0.y = 35 + 25*i;
            DrawRectangleLinesEx(Rec0, 2, BLACK);
            DrawText(TextFormat("Player %d:", i + 1), 5, Rec0.y, 20, BLACK);
            for(int j = 0; j < 5; j++){
                Btn0.y = Rec0.y;
                Btn0.x = WIDTH - 85 - 80*j;
                static char* team;
                if(j == team_list[i]) DrawRectangleRec(Btn0, GRAY);
                else DrawRectangleRec(Btn0, LIGHTGRAY);
                if(CheckCollisionPointRec(GetMousePosition(), Btn0)) {
                    DrawRectangleLinesEx(Btn0, 2, RED) ;
                    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
                        team_list[i] = j;
                        if(settings.enable_sfx) PlaySound(click);
                    }
                } else DrawRectangleLinesEx(Btn0, 2, BLACK) ;
                switch(j){
                    case 0:
                        team = "NONE";
                        break;
                    case 1:
                        team = "RED";
                        break;
                    case 2:
                        team = "BLUE";
                        break;
                    case 3:
                        team = "GREEN";
                        break;
                    case 4:
                        team = "YELLOW";
                        break;
                }
                DrawText(team, Btn0.x + 2, Btn0.y + 2, 20, (j == 0)? BLACK : ReturnColorFromTeamInt(j));
            }
        }

        bool tmp = false;
        if(real_or_turn == 'r') tmp = true;
        AddScreenChangeBtn(game_button, "GAME!", GetMousePosition(), click, &current_screen, (tmp) ? GAME_REAL : GAME_TURN, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "SHIP SELECT SCREEN", GetMousePosition(), click, &current_screen, SHIP_SELECT, settings.enable_sfx);
    }
    EndDrawing();
}

/**
 * @brief Displays the game over screen, when the game ends.
 * @param wintext The text which declares the winner of the game
 * @param click The button click sound
 */
void DisplayGameOverScreen(char* wintext, const Sound click)
{

    ShowCursor();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("THE GAME IS OVER", (int)WIDTH / 2 - 175, 20, 35, BLUE);
        AddScreenChangeBtn(play_again_button, "PLAY AGAIN", GetMousePosition(), click, &current_screen, GAMEMODES, settings.enable_sfx);
        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);

        DrawText(TextFormat("The winner is %s!", wintext), (int)WIDTH / 2 - 170, 70, 30, LIME);
    }
    EndDrawing();
}

/**
 * @brief Displays the game menu screen, when pressing the ESCAPE button in-game.
 * @param click The button click sound
 * @param obstacles The Obstacles instance
 */
void DisplayGameMenuScreen(const Sound click, const Obstacles obstacles) {
    if(IsKeyPressed(KEY_ESCAPE)) {
        current_screen = gamemode;
        success_save = 0;
    }

    ShowCursor();
    BeginDrawing();
    {
        ClearBackground(RAYWHITE); // prior to change

        AddScreenChangeBtn(continue_game_button, "CONTINUE GAME", GetMousePosition(), click, &current_screen, gamemode, settings.enable_sfx);
        AddScreenChangeBtn(exit_no_save_button, "EXIT", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);

        // Manually build the save button
        {
            DrawRectangleRec(save_button, BLACK);
            if (CheckCollisionPointRec(GetMousePosition(), save_button))
            {
                if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
                {
                    if(settings.enable_sfx) PlaySound(click);

                    SaveGameState(obstacles);
                }
                DrawRectangleRec(save_button, RED);
            }
            DrawText("SAVE GAME", (int)save_button.x + 5, (int)save_button.y + 10, 20, WHITE);
            if (success_save)
                DrawText("Game state saved successfully", (int)WIDTH / 2 - 170, (int)HEIGHT - 30, 20, GREEN);
        }
    }
    EndDrawing();
}

/**
 * @brief Displays the control settings screen.
 * @param click The button click sound
 */
void DisplayControlsScreen(const Sound click)
{
    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawLine((int)WIDTH/3, 0, (int)WIDTH/3, (int)HEIGHT, BLACK);
        DrawLine(2*(int)WIDTH/3, 0, 2*(int)WIDTH/3, (int)HEIGHT, BLACK);

        DrawText("PLAYER 1", (int)WIDTH/6 - 70, 20, 30, BROWN);
        DrawText("PLAYER 2", (int)WIDTH/2 - 70, 20, 30, BROWN);
        DrawText(">2 PLAYERS", 5*(int)WIDTH/6 - 90, 20, 30, BROWN);

        AddButtonSetting(&settings.player_one_buttons.forward, forward_btn_1, "Forward", 0);
        AddButtonSetting(&settings.player_one_buttons.backwards, backwards_btn_1, "Backwards", 1);
        AddButtonSetting(&settings.player_one_buttons.right, right_btn_1, "Right", 2);
        AddButtonSetting(&settings.player_one_buttons.left, left_btn_1, "Left", 3);
        AddButtonSetting(&settings.player_one_buttons.turn_cannon_left, cannon_left_btn_1, "Cannon Left", 4);
        AddButtonSetting(&settings.player_one_buttons.turn_cannon_right, cannon_right_btn_1, "Cannon Right", 5);
        AddButtonSetting(&settings.player_one_buttons.fire, fire_btn_1, "Fire", 6);

        AddButtonSetting(&settings.player_two_buttons.forward, forward_btn_2, "Forward", 7);
        AddButtonSetting(&settings.player_two_buttons.backwards, backwards_btn_2, "Backwards", 8);
        AddButtonSetting(&settings.player_two_buttons.right, right_btn_2, "Right", 9);
        AddButtonSetting(&settings.player_two_buttons.left, left_btn_2, "Left", 10);
        AddButtonSetting(&settings.player_two_buttons.turn_cannon_left, cannon_left_btn_2, "Cannon Left", 11);
        AddButtonSetting(&settings.player_two_buttons.turn_cannon_right, cannon_right_btn_2, "Cannon Right", 12);
        AddButtonSetting(&settings.player_two_buttons.fire, fire_btn_2, "Fire", 13);

        AddButtonSetting(&settings.player_indep_buttons.forward, forward_btn_3, "Forward", 14);
        AddButtonSetting(&settings.player_indep_buttons.backwards, backwards_btn_3, "Backwards", 15);
        AddButtonSetting(&settings.player_indep_buttons.right, right_btn_3, "Right", 16);
        AddButtonSetting(&settings.player_indep_buttons.left, left_btn_3, "Left", 17);
        AddButtonSetting(&settings.player_indep_buttons.turn_cannon_left, cannon_left_btn_3, "Cannon Left", 18);
        AddButtonSetting(&settings.player_indep_buttons.turn_cannon_right, cannon_right_btn_3, "Cannon Right", 19);
        AddButtonSetting(&settings.player_indep_buttons.fire, fire_btn_3, "Fire", 20);

        AddScreenChangeBtn(return_to_main_button, "RETURN TO MAIN MENU", GetMousePosition(), click, &current_screen, MAIN, settings.enable_sfx);
    }
    EndDrawing();
}

/**
 * @brief Displays the options (settings) screen.
 * @param click The button click sound
 * @param bgm_en Declares if background game music is enabled or not
 */
void DisplayOptionsScreen(const Sound click, bool *bgm_en)
{
    const Rectangle reticle_rec = {17, 17, WIDTH - 37, 23};
    const Rectangle first_person_rec = {17, 57, WIDTH - 37, 23};
    const Rectangle fullscreen_rec = {17, 97, WIDTH - 37, 23};
    const Rectangle sfx_rec = {17, 137, WIDTH - 37, 23};
    const Rectangle bgm_rec = {17, 177, WIDTH - 37, 23};
    const Rectangle fps_rec = {17, 217, WIDTH - 37, 23};

    const bool tmp = settings.fullscreen;

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

    // detect toggle in fullscreen boolean
    if (settings.fullscreen != tmp)
    {
        ToggleFullscreen();
    }
}

/**
 * @brief Displays the about screen, with credits and gameplay instructions.
 * @param click The button click sound
 */
void DisplayAboutScreen(const Sound click)
{

    BeginDrawing();
    {
        ClearBackground(RAYWHITE);

        DrawText("Gameplay", 10, 10, 100, GREEN);
        DrawText("Navaduel is a game where you and up to 7 friends can all duke it out\nwith pirate ships trying to sink each other in the open sea full of\nrandomly generated obstacles by precisely aiming your cannons\nat each others current positions until only one team is left.\nThe game also features a real-time 1v1 game mode where you can face off\nagainst one friend in real-time combat instead of taking turns\nand analysing each other players movement", 10, 120, 20, BLACK);
        DrawText("Credits", (int)WIDTH / 2 + 10, 10, 100, GREEN);
        DrawRectangleRec((Rectangle){WIDTH / 2 - 5, 0, 5, HEIGHT}, BLACK);

        // Credits
        DrawText("This game was brought to you by:", (int)WIDTH / 2 + 10, 110, 35, BLACK);
        DrawText("Kakagiannis Dimitrios & Panagiotis Skoulis", (int)WIDTH / 2 + 5, 165, 30, BLUE);
        DrawRectangleRec(github_jim_button, BLACK);
        DrawRectangleRec(github_panos_button, BLACK);

        if (CheckCollisionPointRec(GetMousePosition(), github_jim_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                OpenURL("https://github.com/syseditor/");
            }
            DrawRectangleRec(github_jim_button, RED);
        }
        if (CheckCollisionPointRec(GetMousePosition(), github_panos_button))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                PlaySound(click);
                OpenURL("https://github.com/panos21sk/");
            }
            DrawRectangleRec(github_panos_button, RED);
        }
        if (CheckCollisionPointRec(GetMousePosition(), return_to_main_button))
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

/**
 * @brief De-initializes the Main window on exit.
 */
void DeinitMainWindow()
{
    UnloadRenderTexture(screenShip1);
    UnloadRenderTexture(screenShip2);
    UnloadRenderTexture(screenCurrentShip);
    //! destroy the window and cleanup the OpenGL context
    CloseWindow();
}