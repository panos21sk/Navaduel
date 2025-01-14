#ifndef UTIL_H
#define UTIL_H

#include "raylib.h"
#include <setjmp.h>
#include "cJSON.h"
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

#include "screens.h"
#include "ship.h"

typedef struct {
    bool show_reticle;
    bool first_or_third_person_cam;
    bool fullscreen;
    bool enable_sfx;
    bool enable_bgm;
    bool show_fps;
    movement_buttons player_one_buttons;
    movement_buttons player_two_buttons;
    movement_buttons player_indep_buttons; //independent
} setting;

extern int control_index;
extern jmp_buf reset_point;
extern setting settings;

bool strtobool(const char *input);
char *booltostr(bool input);
void AddScreenChangeBtn(Rectangle rec, const char* text, Vector2 mouse_point, Sound click, screen* current_screen, screen scr, bool sfx_en);
void AddSetting(bool* setting, const char* setting_name, Rectangle rec, Sound click, bool sfx_en);
void AddButtonSetting(int *key, Rectangle rec, char *label_name, int btn_id);
void LoadSettings(bool* bgm_en);
void UpdateSettingsConfig(setting settings);
static int parseHandler(void* user, const char* section, const char* name, const char* value);
cJSON *create_ship_json(Ship ship, int type);
Color ReturnColorFromTeamInt(int col_int);
void SaveGameState(Obstacles obstacles);
int LoadGameState(Obstacles *obstacles, Ship_data *ship_data, Texture2D sand_tex, Model palm_tree, Texture2D rock_tex);

#endif //UTIL_H