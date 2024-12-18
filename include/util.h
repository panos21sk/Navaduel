#include <raylib.h>

#ifndef UTIL_H
#define UTIL_H

bool rectangle_pressed(Rectangle rec){
    if(IsMouseButtonPressed(MOUSE_BUTTON_LEFT)){
        Vector2 mousepos = GetMousePosition();
        //check horizontal alignment
        if(mousepos.x - rec.x > 0 && mousepos.x - rec.x - rec.width < 0){
            if(mousepos.y - rec.y > 0 && mousepos.y - rec.y - rec.height < 0){
                return true;
            } else return false;
        } else return false;
    }
}

#endif