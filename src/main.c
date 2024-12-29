#include <setjmp.h>
#include <stdlib.h>

#include "raylib.h"
#include "ship.h"
#include "screens.h"
#include "util.h"
#include "game.h"

int main() {
	//! Main window initialization
	InitMainWindow();

	//! Load game settings
	LoadSettings();

	//! Initalize Audio and start bgm
	InitAudioDevice();
	//start bgm
    //Music from #Uppbeat (free for Creators!):
    //https://uppbeat.io/t/studiokolomna/corsairs
    Music bgm = LoadMusicStream("resources/sound/music/corsairs-studiokolomna-main-version-23542-02-33.mp3");
    PlayMusicStream(bgm);
    //pause with StopMusicStream(bgm), resume with ResumeMusicStream(bgm);
	bool bgm_en;
	bgm_en = true;

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

	//Sound Effect by 
	//<a href="https://pixabay.com/users/soundreality-31074404/?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=233951">Jurij</a> from 
	//<a href="https://pixabay.com/sound-effects//?utm_source=link-attribution&utm_medium=referral&utm_campaign=music&utm_content=233951">Pixabay</a>
	Sound click = LoadSound("resources/sound/sfx/mouse-click-sound-233951.mp3");
	Sound fire = LoadSound("resources/sound/sfx/cannon-fired-gamemaster-audio-2-2-00-01.mp3");
	Sound splash = LoadSound("resources/sound/sfx/water-splash-cannonball-joshua-chivers-1-00-02.mp3");
	Sound explosion = LoadSound("resources/sound/sfx/mixkit-sea-mine-explosion-1184.mp3");

	Texture2D heart_full = LoadTexture("resources/sprites/heart_full.png");
	Texture2D heart_empty = LoadTexture("resources/sprites/heart_empty.png");

	Texture2D sand_tex = LoadTexture("resources/sprites/8_BIT_Sand.png");
	Model palm_tree = LoadModel("resources/models/low_poly_palm_tree.glb");

	// Set-up ships-players
	{
		setjmp(jump_point);
		SetupShips();
	}

	const int island_count = GetRandomValue(MIN_ISLANDS, MAX_ISLANDS);
	Island* island_list = CreateAllIslands(sand_tex, palm_tree, (Vector2){-500, -500}, (Vector2){500, 500}, island_count); //hardcoded bounds initially

	//! Game loop
	while (!exit_window) // run the loop until the user presses ESCAPE or presses the Close button on the window
	{
		if(WindowShouldClose()) exit_window = true;
		if(bgm_en) UpdateMusicStream(bgm);
		//rendering begin
		switch (current_screen) {
			case MAIN:
			{
				DisplayMainScreen(click); //Displays the game's MAIN screen
				break;
			}
			case GAMEMODES:
			{
				DisplayGamemodesScreen(click);
				break;
			}
			case GAME_REAL:
			{	
				DisplayRealTimeGameScreen(&ship1, &ship2, island_list, island_count, water_model, skybox_model, splash, fire, explosion, heart_full, heart_empty); //Starts the real-time game
				break;
			}
			case GAME_TURN:
			{
				DisplayTurnBasedGameScreen(&ship1, &ship2, island_list, island_count, water_model, skybox_model, splash, fire, explosion, heart_full, heart_empty); //Starts the turn-based game
				break;
			}
			case GAME_MENU:
			{
				DisplayGameMenuScreen(click);
				break;
			}
			case GAME_OVER:
			{
				DisplayGameOverScreen(winner, click); //Ends the game (game over)
				break;
			}
			case OPTIONS:
			{	
				DisplayOptionsScreen(click, &bgm_en); //Settings
				break;
			}
			case CONTROLS:
			{
				DisplayControlsScreen(click);
				break;
			}
			case ABOUT:
			{
				DisplayAboutScreen(click); //Credits and gameplay
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
	UnloadSound(click);
	UnloadSound(fire);
	UnloadSound(splash);
	UnloadTexture(heart_full);
	UnloadTexture(heart_empty);
	UnloadMusicStream(bgm);
	CloseAudioDevice();
	// TODO: add everything to 1 function
	return 0;
}
