#include "raylib.h"

#ifndef STATE_H
#define STATE_H

typedef struct{
    //for player 1
    Camera camera1; 
    Vector3 ship1pos;
    Vector3 ship1rot;
} state;

state init_state()
{
    // Initialize camera 1
    Camera camera1 = {0};
    camera1.position = (Vector3){0.0f, 25.0f, 50.0f}; // Camera position
    camera1.target = (Vector3){0.0f, 10.0f, 0.0f};    // Camera looking at point
    camera1.up = (Vector3){0.0f, 1.6f, 0.0f};         // Camera up vector (rotation towards target)
    camera1.fovy = 45.0f;                             // Camera field-of-view Y
    camera1.projection = CAMERA_PERSPECTIVE;
    Vector3 Ship1Pos = {0.0f, 0.0f, 0.0f}; // initialization
    Vector3 Ship1Rot = {0.0f, 0.0f, 0.0f};
}

#endif