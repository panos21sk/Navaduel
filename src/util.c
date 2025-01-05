#include "ini.h"
#include "raylib.h"
#include "screens.h"
#include "util.h"
#include "ship.h"
#include "game.h"
#include "cJSON.h"
#include "raymath.h"
#include <setjmp.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

int control_index = 0;
bool mouse_control;
jmp_buf reset_point;
setting settings;


bool strtobool(const char *input) {
    if(strcmp(input, "true") == 0) return true;
    return false;
}

char *booltostr(const bool input) {
    if(input) return "true";
    return "false";
}

//Reference: https://github.com/benhoyt/inih
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
        cJSON *n_turn = cJSON_CreateNumber(next_turn->id);
        cJSON *has_fired = cJSON_CreateBool(has_fired_once);

        cJSON_AddItemToObject(jsonfinal, "move_time", move_t);
        cJSON_AddItemToObject(jsonfinal, "fire_time", fire_t);
        cJSON_AddItemToObject(jsonfinal, "current_turn", c_turn);
        cJSON_AddItemToObject(jsonfinal, "next_turn", n_turn);
        cJSON_AddItemToObject(jsonfinal, "has_fired", has_fired);
    }

    const char *jsonstring = cJSON_Print(jsonfinal);
    FILE *stateFile = fopen("game.json", "w");
    fprintf(stateFile, "%s", jsonstring);

    success_save = !fclose(stateFile);
    cJSON_Delete(jsonfinal);
}

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
        cJSON *n_turn = cJSON_GetObjectItemCaseSensitive(jsonstate, "next_turn");
        cJSON *has_fired = cJSON_GetObjectItemCaseSensitive(jsonstate, "has_fired");

        move_time = move_t->valueint;
        fire_time = fire_t->valueint;
        current_turn = &ship_data->ship_list[c_turn->valueint];
        next_turn = &ship_data->ship_list[n_turn->valueint];
        has_fired_once = has_fired->type ? true : false;
    }

    Island *island_list = malloc(sizeof(Island)*island_count->valueint);
    Rock *rock_list = malloc(sizeof(Island)*island_count->valueint);

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

char btn_control[2] = {'\0', '\0'};
void AddButtonSetting(int *key, const Rectangle rec, const char *setting_name) {
    btn_control[0] = (char)*key;
    if(mouse_control) {
        if(CheckCollisionPointRec(GetMousePosition(), rec)) {
            SetMouseCursor(MOUSE_CURSOR_IBEAM);
            int new_key = GetKeyPressed();
            while(isalnum(new_key) || ispunct(new_key)) {
                *key = toupper(new_key);
                btn_control[0] = (char)toupper(new_key);
                new_key = GetKeyPressed();
            }
            DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, GREEN);
        } else {
            mouse_control = false;
            SetMouseCursor(MOUSE_CURSOR_ARROW);
            UpdateSettingsConfig(settings);
        }
    } else {
        mouse_control = CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT);
        DrawRectangleLines((int)rec.x, (int)rec.y, (int)rec.width, (int)rec.height, GRAY);
    }
    DrawText(btn_control, (int)rec.x + 10, (int)rec.y + 8, 25, BLACK);
    DrawText(setting_name, (int)rec.x - 400, (int)rec.y + 10, 20, BLACK);
}

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

    cJSON *cannon_rel_pos = cJSON_CreateArray();
    if(cannon_rel_pos == NULL) goto fail;
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.x));
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.y));
    cJSON_AddItemToArray(cannon_rel_pos, cJSON_CreateNumber(ship.cannon->relative_position.z));
    cJSON_AddItemToArray(array, cannon_rel_pos);

    cJSON *health = cJSON_CreateNumber(ship.current_health);
    if(health == NULL) goto fail;
    cJSON_AddItemToArray(array, health);

    return array;

    fail:
    {
        cJSON_Delete(array);
        return NULL;
    }
}

Color ReturnColorFromTeamInt(int col_int){
    switch(col_int){
        case 0: return WHITE;
        case 1: return RED;
        case 2: return BLUE;
        case 3: return GREEN;
        case 4: return YELLOW;
        default: return BLACK;
    }
}