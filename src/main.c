// Include needed 3rd party libs
#include "raylib.h"
// Include first party headers
#include "ship.h"
#include "screens.h"

int main() {
	//! Initialize window
	// Tell the window to use vsync and work on high DPI displays
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_HIGHDPI);
	// Create the window and OpenGL context
	InitWindow(WIDTH, HEIGHT, "NavalDuel");
	// Set Framerate
	SetTargetFPS(60);

	setupShips();

	//! Iniatializing Models for rendering
	const Texture2D water_tex = LoadTexture("resources/sprites/water.png");
	const Mesh water_cube = GenMeshCube(300, 1, 300);
	Model water_model = LoadModelFromMesh(water_cube);
	water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex;

	const RenderTexture screenShip1 = LoadRenderTexture(WIDTH/2, HEIGHT);
	const RenderTexture screenShip2 = LoadRenderTexture(WIDTH/2, HEIGHT);

	//! Game loop
	while (!WindowShouldClose()) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		//rendering begin
		switch (current_screen) {
			case MAIN:
			{
				displayMainScreen(); //Displays the game's MAIN screen
				break;
			}
			case GAME:
			{
				displayGameScreen(&ship1, &ship2, water_model, screenShip1, screenShip2); //Starts the game
				break;
			}
			case GAME_OVER:
			{
				displayGameOverScreen();
				break;
			}
			default:
				break;
		}
	}
	UnloadRenderTexture(screenShip1);
	UnloadRenderTexture(screenShip2);
	//! destroy the window and cleanup the OpenGL context
	CloseWindow();
	return 0;
}