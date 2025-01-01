#ifndef ANIM_H
#define ANIM_H

#include "raylib.h" // Include raylib for Vector3 and Texture2D

// Animation structure
typedef struct {
    Texture2D* frames;  // Array of frames
    int frameCount;     // Total number of frames
    int currentFrame;   // Current frame index
    bool playing;       // Is the animation playing?
    float frameTime;    // Time per frame in seconds
    float timer;        // Internal timer
    Vector3 position;   // 3D position of the animation
    float size;         // Size of the animation
} Animation;

// Function prototypes
void InitAnimation(Animation *anim, const char* gifPath, int maxFrames, float frameTime, Vector3 position, float size);
void UpdateAnimation(Animation *anim, float deltaTime);
void DrawAnimation(Animation *anim, Camera cam);
void UnloadAnimation(Animation *anim);

#endif // ANIM_H