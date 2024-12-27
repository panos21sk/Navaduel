#include "raylib.h"
#include "screens.h"
#include <setjmp.h>

int control_index = 0;
jmp_buf jump_point;

void AddScreenChangeBtn(const Rectangle rec, const char* text, const Vector2 mouse_point, const Sound click, screen* current_screen, const screen scr, const bool sfx_en){
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
    DrawText(setting_name, rec.x + 3, rec.y + 3, 20, BLACK);
    DrawRectangle(WIDTH - 40, rec.y + 3, 17, 17, *setting ? BLUE : RED);
}