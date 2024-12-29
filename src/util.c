#include "ini.h"
#include "raylib.h"
#include "screens.h"
#include "util.h"
#include "ship.h"
#include "game.h"
#include "cJSON.h"
#include <setjmp.h>
#include <stdlib.h>
#include <time.h> //for srand
#include <string.h>

int control_index = 0;
jmp_buf jump_point;
setting settings;

bool strtobool(const char *input) {
    if(strcmp(input, "true") == 0) return true;
    else return false;
}

static int parseHandler(void* user, const char* section, const char* name, const char* value) {
    setting* settings = user;

    if (MATCH("settings", "show_reticle")) {
        settings->show_reticle = strtobool(value);
    } else if (MATCH("settings", "first_or_third_person_cam")) {
        settings->first_or_third_person_cam = strtobool(value);
    } else if (MATCH("settings", "enable_sfx")) {
        settings->enable_sfx = strtobool(value);
    }else if (MATCH("settings", "enable_bgm")) {
        settings->enable_bgm = strtobool(value);
    } else if (MATCH("settings", "fullscreen")) {
        settings->fullscreen = strtobool(value);

    } else {
        return 0;  /* unknown section/name, error */
    }
    return 1;
}

void AddScreenChangeBtn(const Rectangle rec, const char* text, const Vector2 mouse_point, const Sound click, screen* current_screen, const screen scr, bool sfx_en){
    DrawRectangleRec(rec, BLACK);
    if (CheckCollisionPointRec(mouse_point, rec))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if(*current_screen == GAMEMODES && scr != MAIN) gamemode = scr; //saves selected gamemode
                if(*current_screen == GAME_MENU) {
                    success_save = 0;
                    if(scr == MAIN) {
                        while(control_index < 1) {
                            ++control_index;
                            longjmp(jump_point, 0);
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
                        longjmp(jump_point, 0);
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
    }
    DrawRectangleLinesEx(rec, 3, BLACK);
    DrawText(setting_name, (int)rec.x + 3, (int)rec.y + 3, 20, BLACK);
    DrawRectangle(WIDTH - 40, (int)rec.y + 3, 17, 17, *setting ? BLUE : RED);
}

void LoadSettings() {
    settings.enable_bgm = NULL;
    settings.enable_sfx = NULL;
    settings.first_or_third_person_cam = NULL;
    settings.fullscreen = NULL;
    settings.show_reticle = NULL;

    if(ini_parse("config.ini", parseHandler, &settings) < 0) printf("\n\nSettings were not loaded\n\n");
    else printf("\n\nSettings were loaded\n\n");
}

cJSON *create_ship_json(const Ship ship) {
    cJSON *array = cJSON_CreateArray();
    if(array == NULL) goto fail;

    cJSON *id = cJSON_CreateNumber(ship.id);
    if(id == NULL) goto fail;
    cJSON_AddItemToArray(array, id);

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