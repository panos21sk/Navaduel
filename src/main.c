#include "raylib.h"
#include "ship.h"
#include "screens.h"

int main() {
	//! Main window initialization
	InitMainWindow();

	//! Setup ships-players
	SetupShips();

	//! Iniatializing Models for rendering
	const Texture2D water_tex = LoadTexture("resources/sprites/water.png");
	const Mesh water_cube = GenMeshCube(300, 1, 300);
	Model water_model = LoadModelFromMesh(water_cube);
	water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex;

	//! Game loop
	while (!WindowShouldClose()) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		//rendering begin
		switch (current_screen) {
			case MAIN:
			{
				DisplayMainScreen(); //Displays the game's MAIN screen
				break;
			}
			case GAME:
			{
				DisplayGameScreen(&ship1, &ship2, water_model); //Starts the game
				break;
			}
			case GAME_OVER:
			{
				DisplayGameOverScreen(); //Ends the game (game over)
				break;
			}
			case OPTIONS:
			{	
				DisplayOptionsScreen(); //Settings
				break;
			}
			case ABOUT:
			{
				DisplayAboutScreen(); //Credits and gameplay
				break;
			}
			default:
				break;
		}
	}
	DeinitMainWindow(); //Main window de-initialization
	return 0;
}