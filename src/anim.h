/* Helping source: https://www.raylib.com/examples/textures/loader.html?name=textures_sprite_anim*/

#ifndef ANIM_H
#define ANIM_H

/* Import the required game headers (first party libraries) */
#include "raylib.h"

/* Animation structure */
typedef struct {
    Texture2D tex; //Texture to be updated
    Rectangle frameRec; //Position of current frame in sprite sheet
    int animFrames; //Total number of frames in animation
    int currentAnimFrame; //Current frame to display
    int frameDelay; //Delay between frames of animation
    int frameCounter; //Counter of total frames elapsed
    
    bool play; //Control if animation should play
    Vector3 pos; //Control where to play animation in 3d space
    Vector2 size; //The animation's size
} Animation;

/* Function declarations */
Animation CreateAnim(const char* sprite_sheet_path, int animFrames, int frameDelay, Vector2 size);
void UpdateAnim(Animation* anim);
void StartAnim(Animation* anim, Vector3 pos);
void DrawAnim(Animation anim, Camera cam);

#endif // ANIM_H