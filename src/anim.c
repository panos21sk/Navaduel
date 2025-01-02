#include "raylib.h"
#include "ship.h"
#include "anim.h"
#include <stdio.h> // For sprintf
#include <stdlib.h>

Animation CreateAnim(char* sprite_sheet_path, int animFrames, int frameDelay, Vector2 size)
{
    Animation anim_inst;
    anim_inst.tex = LoadTexture(sprite_sheet_path);

    anim_inst.animFrames = animFrames;
    anim_inst.frameDelay = frameDelay;
    anim_inst.currentAnimFrame = 0;
    anim_inst.frameCounter = 0;
    anim_inst.frameRec = (Rectangle){0, 0, anim_inst.tex.width/anim_inst.animFrames, anim_inst.tex.height};

    //filling so data isnt junk
    anim_inst.play = false;
    anim_inst.pos = (Vector3){0,0,0};
    anim_inst.size = size;
    
    return anim_inst;
}

void UpdateAnim(Animation* anim)
{
    if (anim->play)
    {
        anim->frameCounter++;
        if (anim->frameCounter >= anim->frameDelay)
        {
            // Move to next frame
            anim->currentAnimFrame++;
            // loop if total anim frames exceeded
            if (anim->currentAnimFrame >= anim->animFrames){
                anim->currentAnimFrame = 0;
                anim->play = false;
            }
            anim->frameRec.x = (float)anim->currentAnimFrame*(float)anim->tex.width/anim->animFrames;
            // reset frame counter
            anim->frameCounter = 0;
        }
    }
}

void StartAnim(Animation* anim, Vector3 pos){
    anim->play = true; 
    anim->pos = pos;
}

void DrawAnim(Animation anim, Camera cam){
    if(anim.play){
        DrawBillboardRec(cam, anim.tex, anim.frameRec, anim.pos, anim.size, WHITE);
    }
}