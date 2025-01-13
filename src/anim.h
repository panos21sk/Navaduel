/* Helping source: https://www.raylib.com/examples/textures/loader.html?name=textures_gif_player */

#ifndef ANIM_H
#define ANIM_H

/* Import the required game headers (first party libraries) */
#include "raylib.h"

/* Animation structure */
typedef struct {
    Texture2D tex; //texture to be updated
    Rectangle frameRec; //position of curr frame in spritesheet
    int animFrames; //total no. of frames in animation
    int currentAnimFrame; //current frame to display
    int frameDelay; //delay between frames of animation
    int frameCounter; //counter of total frames elapsed
    
    bool play; //control is animation should play
    Vector3 pos; //control where to play animation in 3d space
    Vector2 size; //the animation's size
} Animation;

/* Function declarations */
Animation CreateAnim(const char* sprite_sheet_path, int animFrames, int frameDelay, Vector2 size);
void UpdateAnim(Animation* anim);
void StartAnim(Animation* anim, Vector3 pos);
void DrawAnim(Animation anim, Camera cam);

#endif // ANIM_H