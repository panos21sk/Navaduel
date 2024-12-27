#ifndef UTIL_H
#define UTIL_H

#include "raylib.h"
#include <setjmp.h>
#define MATCH(s, n) strcmp(section, s) == 0 && strcmp(name, n) == 0

typedef struct {
    bool show_reticle;
    bool first_or_third_person_cam;
    bool fullscreen;
    bool enable_sfx;
    bool enable_bgm;
} setting;

extern int control_index;
extern jmp_buf jump_point;
extern setting settings;

void AddScreenChangeBtn(const Rectangle rec, const char* text, const Vector2 mouse_point, const Sound click, screen* current_screen, const screen scr, const bool sfx_en);
void AddSetting(bool* setting, const char* setting_name, Rectangle rec, Sound click, bool sfx_en);
void LoadSettings();
static int parseHandler(void* user, const char* section, const char* name, const char* value);

#endif //UTIL_H