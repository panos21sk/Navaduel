#include "raylib.h"
#include "screens.h"

void AddScreenChangeBtn(Rectangle rec, char* text, Vector2 mouse_point, Sound click, screen* current_screen, screen scr, bool sfx_en){
    DrawRectangleRec(rec, BLACK);
    if (CheckCollisionPointRec(mouse_point, rec))
        {
            if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
            {
                if(sfx_en) PlaySound(click);
                *current_screen = scr;
            }
            DrawRectangleRec(rec, RED);
        }
    DrawText(text, (int)rec.x + 5, (int)rec.y + 10, 20, WHITE);
}

void AddSetting(bool* setting, char* setting_name, Rectangle rec, Sound click, bool sfx_en){
    if (CheckCollisionPointRec(GetMousePosition(), rec) && IsMouseButtonPressed(MOUSE_BUTTON_LEFT)) {
        if(sfx_en) PlaySound(click);
        *setting = !(*setting);
    }
    DrawRectangleLinesEx(rec, 3, BLACK);
    DrawText(setting_name, rec.x + 3, rec.y + 3, 20, BLACK);
    DrawRectangle(WIDTH - 40, rec.y + 3, 17, 17, *setting ? BLUE : RED);
}