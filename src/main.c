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
	//skybox model
	const Texture2D skybox_texture = LoadTexture("resources/sprites/sky.png");
	const Material skybox_material = LoadMaterialDefault();
	skybox_material.maps[MATERIAL_MAP_DIFFUSE].texture = skybox_texture;
	Mesh skybox_cube = GenMeshCube(1, 1, 1);
	Model skybox_model = LoadModelFromMesh(skybox_cube);
	skybox_model.materials[0] = skybox_material;

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
				DisplayGameScreen(&ship1, &ship2, water_model, skybox_model); //Starts the game
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
	UnloadModel(water_model);
	UnloadModel(skybox_model);
	UnloadTexture(water_tex);
	UnloadTexture(skybox_texture);
	return 0;
}