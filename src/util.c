/* Import the required game headers (first party libraries) */
#include "screens.h"
#include "util.h"
#include "ship.h"
#include "game.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"
#include "raymath.h"

/* Import the required tool headers (third party libraries) */
#include "cJSON.h" //JSON parser
#include "ini.h" //INI parser
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

/* Variable initialization */
int control_index = 0;
setting settings;

/**
 * @brief Converts a string to bool (default to false)
 * @param input The string to be converted
 * @return The appropriate Boolean value
 */
bool strtobool(const char *input) {
    if(strcmp(input, "true") == 0) return true;
    return false;
}

/**
 * @brief Converts a bool to string (default to "false")
 * @param input The bool to be converted
 * @return The appropriate string value
 */
char *booltostr(const bool input) {
    if(input) return "true";
    return "false";
}

//Reference: https://github.com/benhoyt/inih
/**
 * @brief The main INI parser, parsing config.ini and setting up the settings object (follows prototype structure, check reference).
 * @param user The object to be set up
 * @param section The current INI section
 * @param name The current INI item's name
 * @param value The item's value
 * @return Returns 1 in success, 0 in failure
 */
static int parseHandler(void* user, const char* section, const char* name, const char* value) {
    setting* settings = user;

    //game settings
    if (MATCH("Game", "show_reticle")) {
        settings->show_reticle = strtobool(value);
    } else if (MATCH("Game", "first_or_third_person_cam")) {
        settings->first_or_third_person_cam = strtobool(value);
    } else if (MATCH("Game", "enable_sfx")) {
        settings->enable_sfx = strtobool(value);
    }else if (MATCH("Game", "enable_bgm")) {
        settings->enable_bgm = strtobool(value);
    } else if (MATCH("Game", "fullscreen")) {
        settings->fullscreen = strtobool(value);
    } else if (MATCH("Game", "show_fps")) {
        settings->show_fps = strtobool(value);
    }
    //player one btns
    else if (MATCH("PlayerOneButtons", "move_key_up")) {
        settings->player_one_buttons.forward = atoi(value);
    } else if (MATCH("PlayerOneButtons", "move_key_down")) {
        settings->player_one_buttons.backwards = atoi(value);
    } else if (MATCH("PlayerOneButtons", "move_key_left")) {
        settings->player_one_buttons.left = atoi(value);
    } else if (MATCH("PlayerOneButtons", "move_key_right")) {
        settings->player_one_buttons.right = atoi(value);
    } else if (MATCH("PlayerOneButtons", "move_key_cannon_left")) {
        settings->player_one_buttons.turn_cannon_left = atoi(value);
    } else if (MATCH("PlayerOneButtons", "move_key_cannon_right")) {
        settings->player_one_buttons.turn_cannon_right = atoi(value);
    } else if (MATCH("PlayerOneButtons", "fire_key")) {
        settings->player_one_buttons.fire = atoi(value);
    }
    //player two btns
    else if (MATCH("PlayerTwoButtons", "move_key_up")) {
        settings->player_two_buttons.forward = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "move_key_down")) {
        settings->player_two_buttons.backwards = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "move_key_left")) {
        settings->player_two_buttons.left = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "move_key_right")) {
        settings->player_two_buttons.right = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "move_key_cannon_left")) {
        settings->player_two_buttons.turn_cannon_left = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "move_key_cannon_right")) {
        settings->player_two_buttons.turn_cannon_right = atoi(value);
    } else if (MATCH("PlayerTwoButtons", "fire_key")) {
        settings->player_two_buttons.fire = atoi(value);
    }
    //player indep btns
    else if (MATCH("PlayerIndependentButtons", "move_key_up")) {
        settings->player_indep_buttons.forward = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "move_key_down")) {
        settings->player_indep_buttons.backwards = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "move_key_left")) {
        settings->player_indep_buttons.left = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "move_key_right")) {
        settings->player_indep_buttons.right = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "move_key_cannon_left")) {
        settings->player_indep_buttons.turn_cannon_left = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "move_key_cannon_right")) {
        settings->player_indep_buttons.turn_cannon_right = atoi(value);
    } else if (MATCH("PlayerIndependentButtons", "fire_key")) {
        settings->player_indep_buttons.fire = atoi(value);
    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

/**
 * @brief Saves the current game state, parsing the data to a JSON file.
 * @param obstacles The Obstacles instance, holding the essential information about each and every spawned obstacle in the game
 */
void SaveGameState(const Obstacles obstacles) {
    // Saving data
    cJSON *jsonfinal = cJSON_CreateObject();

    cJSON *playercount = cJSON_CreateNumber(ship_data.player_count);
    cJSON_AddItemToObject(jsonfinal, "player_count", playercount);

    for (int i = 0; i<ship_data.player_count; i++) {
        cJSON *jsonship = create_ship_json(ship_data.ship_list[i], ship_data.type_list[i]);
        cJSON_AddItemToObject(jsonfinal, TextFormat("ship_%d", i), jsonship);
    }
    cJSON *gamemodeSt = cJSON_CreateString(gamemode == GAME_REAL ? "GAME_REAL" : "GAME_TURN");
    cJSON *rock_count = cJSON_CreateNumber(obstacles.rock_count);
    cJSON *island_count = cJSON_CreateNumber(obstacles.island_count);

    cJSON_AddItemToObject(jsonfinal, "gamemode", gamemodeSt);
    cJSON_AddItemToObject(jsonfinal, "rock_count", rock_count);
    cJSON_AddItemToObject(jsonfinal, "island_count", island_count);

    // Registering rocks to game.json
    for(int i = 0; i < obstacles.rock_count; i++) {
        cJSON *rock_info = cJSON_CreateArray();
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].center_pos.x));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].center_pos.y));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].center_pos.z));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].rotation_vec.x));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].rotation_vec.y));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].rotation_vec.z));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].height));
        cJSON_AddItemToArray(rock_info, cJSON_CreateNumber(obstacles.rock_list[i].geometry_id));
        cJSON_AddItemToObject(jsonfinal, TextFormat("rock_%d", i), rock_info);
    }

    // Registering islands to game.json
    for(int i = 0; i < obstacles.island_count; i++) {
        cJSON *island_info = cJSON_CreateArray();
        cJSON_AddItemToArray(island_info, cJSON_CreateNumber(obstacles.island_list[i].center_pos.x));
        cJSON_AddItemToArray(island_info, cJSON_CreateNumber(obstacles.island_list[i].center_pos.y));
        cJSON_AddItemToArray(island_info, cJSON_CreateNumber(obstacles.island_list[i].center_pos.z));
        cJSON_AddItemToArray(island_info, cJSON_CreateNumber(obstacles.island_list[i].radius));
        cJSON_AddItemToObject(jsonfinal, TextFormat("island_%d", i), island_info);
    }

    if (gamemode == GAME_TURN)
    {
        cJSON *move_t = cJSON_CreateNumber(move_time);
        cJSON *fire_t = cJSON_CreateNumber(fire_time);
        cJSON *c_turn = cJSON_CreateNumber(current_turn->id);
        cJSON *has_fired = cJSON_CreateBool(has_fired_once);

        cJSON_AddItemToObject(jsonfinal, "move_time", move_t);
        cJSON_AddItemToObject(jsonfinal, "fire_time", fire_t);
        cJSON_AddItemToObject(jsonfinal, "current_turn", c_turn);
        cJSON_AddItemToObject(jsonfinal, "has_fired", has_fired);
    }

    const char *jsonstring = cJSON_Print(jsonfinal);
    FILE *stateFile = fopen("game.json", "w");
    fprintf(stateFile, "%s", jsonstring);

    success_save = !fclose(stateFile);
    cJSON_Delete(jsonfinal);
}

/**
 * @brief Loads a game state from a JSON file, parsing its data and initializing the appropriate variables for smooth gameplay.
 * @param obstacles A pointer to the Obstacles object
 * @param ship_data A pointer to the array holding the ships' data
 * @param sand_tex The islands' sand texture
 * @param palm_tree The islands' palm tree model
 * @param rock_tex The rocks' texture
 * @return Returns 1 in success, 0 in failure
 */
int LoadGameState(Obstacles *obstacles, Ship_data *ship_data, Texture2D sand_tex, Model palm_tree, Texture2D rock_tex) {
    //init
    FILE *stateFile = fopen("game.json", "r");
    if (stateFile == NULL)
    {
        printf("An error occurred");
        success_load = 0;
        return 1;
    }

    //File's exact length
    fseek(stateFile, 0, SEEK_END);
    long len = ftell(stateFile);
    fseek(stateFile, 0, SEEK_SET);

    char *buffer = malloc(len+1);
    fread(buffer, sizeof(char), len, stateFile);
    buffer[len] = '\0';
    cJSON *jsonstate = cJSON_Parse(buffer);
    if (jsonstate == NULL)
    {
        puts("No saved game state");
        success_load = 0;
        fclose(stateFile);
        free(buffer);
        return 1;
    }

    cJSON *playercount = cJSON_GetObjectItemCaseSensitive(jsonstate, "player_count");
    Ship *ship_list = malloc(sizeof(Ship)*playercount->valueint);
    int *type_list = malloc(sizeof(int)*playercount->valueint);
    int *team_list = malloc(sizeof(int)*playercount->valueint);
    for(int i = 0; i<playercount->valueint; i++) {
        cJSON *shipstate = cJSON_GetObjectItemCaseSensitive(jsonstate, TextFormat("ship_%d", i));
        cJSON *shiptype = cJSON_GetArrayItem(shipstate, 1);
        cJSON *shipteam = cJSON_GetArrayItem(shipstate, 2);

        type_list[i] = shiptype->valueint;
        team_list[i] = shipteam->valueint;
        ship_list[i] = LoadShip(type_list[i], shipstate, playercount->valueint);
    }
    ship_data->player_count = playercount->valueint;
    ship_data->ship_list = ship_list;
    ship_data->team_list = team_list;
    ship_data->type_list = type_list;

    cJSON *gamemodeSt = cJSON_GetObjectItemCaseSensitive(jsonstate, "gamemode");
    cJSON *rock_count = cJSON_GetObjectItemCaseSensitive(jsonstate, "rock_count");
    cJSON *island_count = cJSON_GetObjectItemCaseSensitive(jsonstate, "island_count");

    gamemode = strcmp(gamemodeSt->valuestring, "GAME_REAL") == 0 ? GAME_REAL : GAME_TURN;

    if(gamemode == GAME_TURN) {
        cJSON *move_t = cJSON_GetObjectItemCaseSensitive(jsonstate, "move_time");
        cJSON *fire_t = cJSON_GetObjectItemCaseSensitive(jsonstate, "fire_time");
        cJSON *c_turn = cJSON_GetObjectItemCaseSensitive(jsonstate, "current_turn");
        cJSON *has_fired = cJSON_GetObjectItemCaseSensitive(jsonstate, "has_fired");

        move_time = move_t->valueint;
        fire_time = fire_t->valueint;
        current_turn = &ship_data->ship_list[c_turn->valueint];
        has_fired_once = has_fired->type ? true : false;
    }

    Island *island_list = malloc(sizeof(Island)*island_count->valueint);
    Rock *rock_list = malloc(sizeof(Island)*rock_count->valueint);

    for(int i = 0; i < island_count->valueint; i++) {
        Island island;
        cJSON *island_info = cJSON_GetObjectItemCaseSensitive(jsonstate, TextFormat("island_%d", i));

        island.center_pos = (Vector3){
            (float)cJSON_GetArrayItem(island_info, 0)->valuedouble,
            (float)cJSON_GetArrayItem(island_info, 1)->valuedouble,
            (float)cJSON_GetArrayItem(island_info, 2)->valuedouble
        };
        island.radius = cJSON_GetArrayItem(island_info, 3)->valueint;
        island.palm_tree = palm_tree;
        Mesh sphere_mesh = GenMeshSphere((float)island.radius, 128, 128);
        island.island_sphere = LoadModelFromMesh(sphere_mesh);
        island.island_sphere.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = sand_tex;
        island.sand_tex = sand_tex;

        island_list[i] = island;
    }

    for(int i = 0; i < rock_count->valueint; i++) {
        Rock rock;
        cJSON *rock_info = cJSON_GetObjectItemCaseSensitive(jsonstate, TextFormat("rock_%d", i));

        rock.center_pos = (Vector3){
            (float)cJSON_GetArrayItem(rock_info, 0)->valuedouble,
            (float)cJSON_GetArrayItem(rock_info, 1)->valuedouble,
            (float)cJSON_GetArrayItem(rock_info, 2)->valuedouble,
        };
        rock.rotation_vec = (Vector3){
            (float)cJSON_GetArrayItem(rock_info, 3)->valuedouble,
            (float)cJSON_GetArrayItem(rock_info, 4)->valuedouble,
            (float)cJSON_GetArrayItem(rock_info, 5)->valuedouble
        };
        rock.height = cJSON_GetArrayItem(rock_info, 6)->valueint;
        rock.geometry_id = cJSON_GetArrayItem(rock_info, 7)->valueint;
        rock.rock_tex = rock_tex;
        if(rock.geometry_id == 1) {
            rock.model = LoadModelFromMesh(GenMeshCube(
                (float)rock.height / 3 + (float)rock.model_coefficient * (float)rock.height / 8,
                (float)rock.height,
                (float)rock.height / 3 + (float)rock.model_coefficient * (float)rock.height / 8));
        } else {
            rock.model = LoadModelFromMesh(GenMeshSphere((float)rock.height, 64, 64));
        }
        rock.model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = rock_tex;
        rock.model.transform = MatrixRotateXYZ(rock.rotation_vec);

        rock_list[i] = rock;
    }
  
    obstacles->island_count = island_count->valueint;
    obstacles->rock_count = rock_count->valueint;
    obstacles->island_list = island_list;
    obstacles->rock_list = rock_list;

    success_load = !fclose(stateFile);
    free(buffer);
    cJSON_Delete(jsonstate);

    if (success_load)
    {
        is_loaded = true;
        reset_state = 0;
        current_screen = gamemode;
        return 1;
    }
    return 0;
}

/**
 * @brief Adds a working button.
 * @param rec The button's rectangle
 * @param text The button's text
 * @param mouse_point The current mouse location on-screen
 * @param click The button click sound
 * @param current_screen A pointer to the current screen variable
 * @param scr The screen to switch to when the button is pressed
 * @param sfx_en Declares if the sound effects are enabled or not (derived from settings)
 */
void AddScreenChangeBtn(const Rectangle rec, const char* text, const Vector2 mouse_point, const Sound click, screen* current_screen, const screen scr, bool sfx_en){
    DrawRectangleRec(rec, BLACK);
    if (CheckCollisionPointRec(mouse_point, rec))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if(*current_screen == GAMEMODES && scr != MAIN) {
                    dice_state = 1;
                    reset_state = 1;
                }
                if(*current_screen != MAIN) success_load = 1;
                if(*current_screen == GAME_MENU) {
                    success_save = 0;
                    if(scr == MAIN) {
                        while(control_index < 1) {
                            ++control_index;
                        }
                    }
                }
                if(*current_screen == GAME_OVER) {
                    while(control_index < 1) {
                        if(is_loaded) {
                            fclose(fopen("game.json", "w"));
                            is_loaded = false;
                        }
                        ++control_index;
                        startup_counter = GAME_STARTUP_COUNTER;
                    }
                }
                if(sfx_en) PlaySound(click);
                *current_screen = scr;
            }
            DrawRectangleRec(rec, RED);
        }
    DrawText(text, (int)rec.x + 5, (int)rec.y + 10, 20, WHITE);
}

/**
 * @brief Adds a setting in the Options screen.
 * @param setting A pointer to the setting
 * @param setting_name The setting's name
 * @param rec The setting's rectangle
 * @param click The button click sound
 * @param sfx_en Declares if the sound effects are enabled or not
 */
void AddSetting(bool* setting, const char* setting_name, const Rectangle rec, const Sound click, const bool sfx_en){
    if (CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if(sfx_en) PlaySound(click);
        *setting = !(*setting);
        UpdateSettingsConfig(settings);
    }
    DrawRectangleLinesEx(rec, 3, BLACK);
    DrawText(setting_name, (int)rec.x + 3, (int)rec.y + 3, 20, BLACK);
    DrawRectangle(WIDTH - 40, (int)rec.y + 3, 17, 17, *setting ? BLUE : RED);
}

/**
 * @brief Adds a button setting in the Controls screen.
 * @param key The key to control
 * @param rec The button's rectangle
 * @param label_name The key's functionality label
 * @param btn_id The button's id number
 */
void AddButtonSetting(int *key, const Rectangle rec, const char *label_name, const int btn_id) {
    static bool mouse_control[21] = {0};
    static char button_control[21][2] = {0};
    static int error[21] = {0};

    switch(*key) {
        case 257:
            button_control[btn_id][0] = (char)0;
        break;
        case 262:
            button_control[btn_id][0] = (char)1;
        break;
        case 263:
            button_control[btn_id][0] = (char)2;
        break;
        case 264:
            button_control[btn_id][0] = (char)3;
        break;
        case 265:
            button_control[btn_id][0] = (char)4;
        break;
        default: button_control[btn_id][0] = (char)*key;
    }
    button_control[btn_id][1] = '\0';
    if(mouse_control[btn_id]) {
        if(CheckCollisionPointRec(GetMousePosition(), rec)) {
            char temp;
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            int new_key = GetKeyPressed();
            if(new_key) error[btn_id] = 0;
            while(isalnum(new_key) || ispunct(new_key) || isspace(new_key) || (new_key >= 257 && new_key <= 265)) {
                if (new_key == 257) temp = (char)0;
                else if (new_key == 262) temp = (char)1;
                else if (new_key == 263) temp = (char)2;
                else if (new_key == 264) temp = (char)3;
                else if (new_key == 265) temp = (char)4;
                else temp = (char)toupper(new_key);

                int c=14, j=0;
                if(btn_id > 13) {
                    c=7;
                    j=14;
                }
                for(int i=0; i<c; i++) {
                    if(temp == button_control[i+j][0]) error[btn_id] += 1;
                }
                if(error[btn_id]) break;
                button_control[btn_id][0] = temp;
                *key = toupper(new_key);
                new_key = GetKeyPressed();
            }
            if(error[btn_id]) {
                DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, RED);
                DrawText("KEY ALREADY IN USE", WIDTH/2 - 163, HEIGHT - 50, 30, RED);
                if(btn_id < 14) DrawText("Note: Player 1 and Player 2 MUST have different controls", 5*WIDTH/6-250, HEIGHT-30, 18, DARKBLUE);
            }
            else DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, GREEN);
            DrawText(label_name, 30, HEIGHT - 100, 30, GREEN);
        } else {
            mouse_control[btn_id] = false;
            error[btn_id] = 0;
            SetMouseCursor(MOUSE_CURSOR_ARROW);
            UpdateSettingsConfig(settings);
        }
    } else {
        mouse_control[btn_id] = CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, GRAY);
    }
    switch((int)button_control[btn_id][0]) {
        case 32:
            DrawText("SP", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        case 0:
            DrawText("EN", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        case 1:
            DrawText("RA", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        case 2:
            DrawText("LA", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        case 3:
            DrawText("DA", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        case 4:
            DrawText("UA", (int)rec.x + 4, (int)rec.y + 12, 40, BLACK);
        break;
        default: DrawText(button_control[btn_id], (int)rec.x + 18, (int)rec.y + 12, 40, BLACK);
    }
}

/**
 * @brief Loads settings, parsed from config.ini.
 * @param bgm_en Declares if the background game music is enabled or not
 */
void LoadSettings(bool* bgm_en) {
    movement_buttons init_btns = {0};

    settings.enable_bgm = NULL;
    settings.enable_sfx = NULL;
    settings.first_or_third_person_cam = NULL;
    settings.fullscreen = NULL;
    settings.show_reticle = NULL;
    settings.show_fps = NULL;
    settings.player_indep_buttons = init_btns;
    settings.player_one_buttons = init_btns;
    settings.player_two_buttons = init_btns;

    if(ini_parse("config.ini", parseHandler, &settings) < 0) printf("\n\nSettings were not loaded\n\n");
    else printf("\n\nSettings were loaded\n\n");

    *bgm_en = settings.enable_bgm;
}

/**
 * @brief Dynamically parses any change in settings during runtime to config.ini.
 * @param settings The settings object
 */
void UpdateSettingsConfig(const setting settings) {
    const char *game = TextFormat("[Game] ; Game settings\nshow_reticle = %s\nfirst_or_third_person_cam = %s ; true = first person, false = third person\nfullscreen = %s\nenable_sfx = %s\nenable_bgm = %s\nshow_fps = %s\n\n",
        booltostr(settings.show_reticle), booltostr(settings.first_or_third_person_cam),
        booltostr(settings.fullscreen), booltostr(settings.enable_sfx), booltostr(settings.enable_bgm),
        booltostr(settings.show_fps));

    const char *p1 = TextFormat("[PlayerOneButtons]\nmove_key_up = %d\nmove_key_down = %d\nmove_key_right = %d\nmove_key_left = %d\nmove_key_cannon_right = %d\nmove_key_cannon_left = %d\nfire_key = %d\n\n",
        settings.player_one_buttons.forward, settings.player_one_buttons.backwards,
        settings.player_one_buttons.right, settings.player_one_buttons.left,
        settings.player_one_buttons.turn_cannon_right, settings.player_one_buttons.turn_cannon_left,
        settings.player_one_buttons.fire);

    const char *p2 = TextFormat("[PlayerTwoButtons]\nmove_key_up = %d\nmove_key_down = %d\nmove_key_right = %d\nmove_key_left = %d\nmove_key_cannon_right = %d\nmove_key_cannon_left = %d\nfire_key = %d\n\n",
        settings.player_two_buttons.forward, settings.player_two_buttons.backwards,
        settings.player_two_buttons.right, settings.player_two_buttons.left,
        settings.player_two_buttons.turn_cannon_right, settings.player_two_buttons.turn_cannon_left,
        settings.player_two_buttons.fire);

    const char *pi = TextFormat("[PlayerIndependentButtons]\nmove_key_up = %d\nmove_key_down = %d\nmove_key_right = %d\nmove_key_left = %d\nmove_key_cannon_right = %d\nmove_key_cannon_left = %d\nfire_key = %d\n\n",
        settings.player_indep_buttons.forward, settings.player_indep_buttons.backwards,
        settings.player_indep_buttons.right, settings.player_indep_buttons.left,
        settings.player_indep_buttons.turn_cannon_right, settings.player_indep_buttons.turn_cannon_left,
        settings.player_indep_buttons.fire);

    fclose(fopen("config.ini", "w")); //clears the file

    FILE *config = fopen("config.ini", "a");
    fputs(game, config);
    fputs(p1, config);
    fputs(p2, config);
    fputs(pi, config);
    fclose(config);
}

/**
 * @brief Creates a JSON array, storing essential information about a ship's state.
 * @param ship The ship to be saved
 * @param type The ship's type
 * @return A pointer to a cJSON array object
 */
cJSON *create_ship_json(const Ship ship, const int type) {
    cJSON *array = cJSON_CreateArray();
    if(array == NULL) goto fail;

    cJSON *id = cJSON_CreateNumber(ship.id);
    if(id == NULL) goto fail;
    cJSON_AddItemToArray(array, id);

    cJSON *shiptype = cJSON_CreateNumber(type);
    if(shiptype == NULL) goto fail;
    cJSON_AddItemToArray(array, shiptype);

    cJSON *teamnum = cJSON_CreateNumber(ship.team);
    if(teamnum == NULL) cJSON_AddItemToArray(array, cJSON_CreateNull());
    cJSON_AddItemToArray(array, teamnum);

    cJSON *yaw = cJSON_CreateNumber(ship.yaw);
    if(yaw == NULL) goto fail;
    cJSON_AddItemToArray(array, yaw);

    cJSON *position = cJSON_CreateArray();
    if(position == NULL) goto fail;
    cJSON_AddItemToArray(position, cJSON_CreateNumber(ship.position.x));
    cJSON_AddItemToArray(position, cJSON_CreateNumber(ship.position.y));
    cJSON_AddItemToArray(position, cJSON_CreateNumber(ship.position.z));
    cJSON_AddItemToArray(array, position);

    cJSON *prev_pos = cJSON_CreateArray();
    if(prev_pos == NULL) goto fail;
    cJSON_AddItemToArray(prev_pos, cJSON_CreateNumber(ship.prev_position.x));
    cJSON_AddItemToArray(prev_pos, cJSON_CreateNumber(ship.prev_position.y));
    cJSON_AddItemToArray(prev_pos, cJSON_CreateNumber(ship.prev_position.z));
    cJSON_AddItemToArray(array, prev_pos);

    cJSON *cannon_rel_pos = cJSON_CreateArray();
    if(cannon_rel_pos == NULL) goto fail;
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.x));
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.y));
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.z));
    cJSON_AddItemToArray(array, cannon_rel_pos);

    cJSON *health = cJSON_CreateNumber(ship.current_health);
    if(health == NULL) goto fail;
    cJSON_AddItemToArray(array, health);

    cJSON *prev_shot_r = cJSON_CreateNumber(ship.prev_shot_release);
    if(prev_shot_r == NULL) goto fail;
    cJSON_AddItemToArray(array, prev_shot_r);

    cJSON *reload_time = cJSON_CreateNumber(ship.time_to_reload_since_last_shot);
    if(reload_time == NULL) goto fail;
    cJSON_AddItemToArray(array, reload_time);

    cJSON *is_destr = cJSON_CreateBool(ship.is_destroyed);
    if(is_destr == NULL) goto fail;
    cJSON_AddItemToArray(array, is_destr);

    return array;

    fail:
    {
        cJSON_Delete(array);
        return NULL;
    }
}

/**
 * @brief Returns the color of each team, depending on its id.
 * @param col_int The team's id
 * @return The appropriate Color code
 */
Color ReturnColorFromTeamInt(const int col_int){
    switch(col_int){
        case 0: return WHITE;
        case 1: return RED;
        case 2: return BLUE;
        case 3: return GREEN;
        case 4: return YELLOW;
        default: return BLACK;
    }
}

/**
 * @brief Toggles fullscreen and updates settings dynamically, when F11 is pressed anytime during runtime.
 */
void CheckFullscreenToggle() {
    if(IsKeyPressed(KEY_F11)) {
        settings.fullscreen = !settings.fullscreen;
        UpdateSettingsConfig(settings);
        ToggleFullscreen();
    }
}