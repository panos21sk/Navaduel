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
// Include needed 3rd party libs
#include "raylib.h"
// Include first party headers

// #Dont know if this is a worthy inclusion, keep in mind for structuring
//  typedef struct{
////for player 1
//     Camera camera1;
//     Vector3 ship1pos;
//     Vector3 ship1rot;
// } state;

typedef enum
{
	MAIN,
	OPTIONS,
	GAME,
	GAME_OVER
} screen;

int main()
{
	//! Initialize window
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(1280, 800, "Navaduel");
	// Set Framerate
	SetTargetFPS(60);

	//! Initialize Game State
	Camera camera1 = {0};
	camera1.position = (Vector3){0.0f, 25.0f, 50.0f}; // Camera position
	camera1.target = (Vector3){0.0f, 10.0f, 0.0f};	  // Camera looking at point
	camera1.up = (Vector3){0.0f, 1.6f, 0.0f};		  // Camera up vector (rotation towards target)
	camera1.fovy = 45.0f;							  // Camera field-of-view Y
	camera1.projection = CAMERA_PERSPECTIVE;
	Vector3 ship1pos = {0.0f, 0.0f, 0.0f}; // initialization
	Vector3 ship1rot = {0.0f, 0.0f, 0.0f};

	//! Iniatializing Models for rendering
	Model ship = LoadModel("../../resources/models/ship2.glb");
	Texture2D water_tex = LoadTexture("../../resources/sprites/water.png");
	Mesh water_cube = GenMeshCube(300, 1, 300);
	Model water_model = LoadModelFromMesh(water_cube);
	water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex;

	//! Starting condition
	screen current_screen = MAIN;

	//! Game loop
	while (!WindowShouldClose()) // run the loop untill the user presses ESCAPE or presses the Close button on the window
	{

		//rendering begin
		switch (current_screen)
		{
		case MAIN:
		{
			BeginDrawing();
			ClearBackground(RAYWHITE);
			DrawText("NAVADUEL", 20, 20, 30, BLUE);
			int scrh = GetScreenHeight();
			int scrw = GetScreenWidth();
			Rectangle play_button = {scrw / 2 - 80, scrh / 2 - 20, 160, 40};
			DrawRectangleRec(play_button, BLACK);

			//TODO: Check if button was clicked. Make button collision detection into a function
			if (IsMouseButtonPressed(MOUSE_BUTTON_LEFT))
			{
				Vector2 mousepos = GetMousePosition();
				// check horizontal alignment
				if (mousepos.x - play_button.x > 0 && mousepos.x - play_button.x - play_button.width < 0)
				{
					if (mousepos.y - play_button.y > 0 && mousepos.y - play_button.y - play_button.height < 0)
					{
						DrawRectangle(50, 50, 200, 200, RED);
						current_screen = GAME;
					}
					else
						DrawRectangle(50, 50, 200, 200, BLUE);
				}
				else
					DrawRectangle(50, 50, 200, 200, BLUE);
			}
			EndDrawing();
		} break;
		case GAME:
		{
			//! Input Handling:
			UpdateCameraPro(&camera1, (Vector3){0, 0, 0}, (Vector3){0, 0, 0}, 0);

			//! Rendering:
			BeginDrawing();
			ClearBackground(RAYWHITE);
			BeginMode3D(camera1);
			// Draw the ship
			DrawModel(ship, ship1pos, 1.0f, WHITE);
			// Draw water surface as a thin cube mesh
			DrawModel(water_model, (Vector3){-100, -1, -100}, 1, BLUE);
			EndMode3D();
			EndDrawing();
		} break;
		default: break;
		}
	}

	//! destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}
