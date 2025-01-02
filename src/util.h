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
} setting;

extern int control_index;
extern jmp_buf reset_point;
extern setting settings;

Ship *getShipFromId(int id);
bool isEvenNumber(int number);
bool strtobool(const char *input);
char *booltostr(bool input);
void AddScreenChangeBtn(const Rectangle rec, const char* text, const Vector2 mouse_point, const Sound click, screen* current_screen, const screen scr, const bool sfx_en);
void AddSetting(bool* setting, const char* setting_name, Rectangle rec, Sound click, bool sfx_en);
void LoadSettings(bool* bgm_en);
void UpdateSettingsConfig(setting settings);
static int parseHandler(void* user, const char* section, const char* name, const char* value);
cJSON *create_ship_json(Ship ship);
Color ReturnColorFromTeamInt(int col_int);

#endif //UTIL_H