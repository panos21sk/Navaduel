#include "raylib.h"
#include "ship.h"
#include "anim.h"
#include <stdio.h> // For sprintf
#include <stdlib.h>

// Initialize the animation
void InitAnimation(Animation *anim, const char *gifPath, int maxFrames, float frameTime, Vector3 position, float size) {
    anim->frames = (Texture2D *)malloc(sizeof(Texture2D) * maxFrames);
    anim->frameCount = 0;
    anim->currentFrame = 0;
    anim->playing = false;
    anim->frameTime = frameTime;
    anim->timer = 0.0f;
    anim->position = position;
    anim->size = size;

    // Load frames from the specified path
    char framePath[128];
    for (int i = 0; i < maxFrames; i++) {
        sprintf(framePath, "%s_%02d.png", gifPath, i);
        if (FileExists(framePath)) {
            anim->frames[i] = LoadTexture(framePath);
            anim->frameCount++;
        } else {
            break;
        }
    }
}

// Update the animation's state
void UpdateAnimation(Animation *anim, float deltaTime) {
    if (!anim->playing || anim->frameCount <= 0) return;

    anim->timer += deltaTime;
    if (anim->timer >= anim->frameTime) {
        anim->timer -= anim->frameTime;
        anim->currentFrame = (anim->currentFrame + 1) % anim->frameCount;
    }
}

// Draw the animation in 3D space
void DrawAnimation(Animation *anim, Camera cam) {
    if (anim->playing && anim->frameCount > 0) {
        DrawBillboard(cam, anim->frames[anim->currentFrame], anim->position, anim->size, WHITE);
    }
}

// Unload the animation's resources
void UnloadAnimation(Animation *anim) {
    for (int i = 0; i < anim->frameCount; i++) {
        UnloadTexture(anim->frames[i]);
    }
    free(anim->frames);
    anim->frames = NULL;
}