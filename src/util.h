#ifndef UTIL_H
#define UTIL_H

#include "raylib.h"

void AddScreenChangeBtn(Rectangle rec, char* text, Vector2 mouse_point, Sound click, screen* current_screen, screen scrm, bool sfx_en);
void AddSetting(bool* setting, char* setting_name, Rectangle rec, Sound click, bool sfx_en);

#endif //UTIL_H