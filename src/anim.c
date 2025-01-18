/* Import the required game headers (first party libraries) */
#include "anim.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"

/**
 * @brief Creates an animation to play on a rendered screen.
 * @param sprite_sheet_path The path directed to the sprite's texture (in resources folder)
 * @param animFrames The animation frames
 * @param frameDelay The animation frame-change delay
 * @param size The animation's size
 * @return An Animation object with the essential information to emulate an animation
 */
Animation CreateAnim(const char* sprite_sheet_path, const int animFrames, const int frameDelay, const Vector2 size)
{
    Animation anim_inst; // Animation instance (returned object)
    anim_inst.tex = LoadTexture(sprite_sheet_path);

    anim_inst.animFrames = animFrames;
    anim_inst.frameDelay = frameDelay;
    anim_inst.currentAnimFrame = 0;
    anim_inst.frameCounter = 0;
    anim_inst.frameRec = (Rectangle){0, 0, anim_inst.tex.width/anim_inst.animFrames, anim_inst.tex.height};

    //Filling so data isn't junk
    anim_inst.play = false;
    anim_inst.pos = (Vector3){0,0,0};
    anim_inst.size = size;
    
    return anim_inst;
}

/**
 * @brief Updates an animation frame to the next scheduled frame.
 * @param anim A pointer to the to-be-updated Animation object
 */
void UpdateAnim(Animation* anim)
{
    if (anim->play)
    {
        anim->frameCounter++;
        if (anim->frameCounter >= anim->frameDelay)
        {
            // Move to next frame
            anim->currentAnimFrame++;
            // Loop if total anim frames exceeded
            if (anim->currentAnimFrame >= anim->animFrames){
                anim->currentAnimFrame = 0;
                anim->play = false;
            }
            anim->frameRec.x = (float)anim->currentAnimFrame*(float)anim->tex.width/anim->animFrames;
            // Reset frame counter
            anim->frameCounter = 0;
        }
    }
}

/**
 * @brief Spawns an animation.
 * @param anim A pointer to the to-be-spawned Animation object
 * @param pos The spawn position (3D Vector)
 */
void StartAnim(Animation* anim, Vector3 pos){
    anim->play = true; 
    anim->pos = pos;
}

/**
 * @brief Plays an animation, if it's scheduled to be played.
 * @param anim The to-be-played Animation object
 * @param cam The Camera object in which the animation will be played
 */
void DrawAnim(Animation anim, Camera cam){
    if(anim.play){
        DrawBillboardRec(cam, anim.tex, anim.frameRec, anim.pos, anim.size, WHITE);
    }
}