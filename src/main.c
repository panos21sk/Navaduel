/*
Raylib example file.
This is an example main file for a simple raylib project.
Use this as a starting point or replace it with your code.

For a C++ project simply rename the file to .cpp and re-run the build script 

-- Copyright (c) 2020-2024 Jeffery Myers
--
--This software is provided "as-is", without any express or implied warranty. In no event 
--will the authors be held liable for any damages arising from the use of this software.

--Permission is granted to anyone to use this software for any purpose, including commercial 
--applications, and to alter it and redistribute it freely, subject to the following restrictions:

--  1. The origin of this software must not be misrepresented; you must not claim that you 
--  wrote the original software. If you use this software in a product, an acknowledgment 
--  in the product documentation would be appreciated but is not required.
--
--  2. Altered source versions must be plainly marked as such, and must not be misrepresented
--  as being the original software.
--
--  3. This notice may not be removed or altered from any source distribution.

*/

#include "raylib.h"

int main ()
{
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);

	// Create the window and OpenGL context
	InitWindow(1280, 800, "Navaduel");

	//Initialize camera 1
	Camera camera1 = { 0 };
	camera1.position = (Vector3){ 20.0f, 20.0f, 20.0f }; // Camera position
	camera1.target = (Vector3){ 0.0f, 8.0f, 0.0f };      // Camera looking at point
	camera1.up = (Vector3){ 0.0f, 1.6f, 0.0f };          // Camera up vector (rotation towards target)
	camera1.fovy = 45.0f;                                // Camera field-of-view Y
    camera1.projection = CAMERA_PERSPECTIVE;

	SetTargetFPS(60);

	Model ship = LoadModel("../../resources/models/ship2.glb");

	Vector3 shipPos = { 0.0f, 0.0f, 0.0f };

	// game loop
	while (!WindowShouldClose())		// run the loop untill the user presses ESCAPE or presses the Close button on the window
	{
		Vector3 oldCamPos = camera1.position;    // Store old camera1 position
		Vector3 oldShip1Pos = shipPos;

        UpdateCamera(&camera1, CAMERA_FIRST_PERSON);

		BeginDrawing();
			ClearBackground(RAYWHITE);
			BeginMode3D(camera1);

                // Draw the ship
                DrawModel(ship, shipPos, 1.0f, WHITE);
                DrawGrid(10, 5.0f);

            EndMode3D();
		EndDrawing();

            
	}

	// destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
