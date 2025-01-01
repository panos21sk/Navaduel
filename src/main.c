#include <setjmp.h>
#include <stdlib.h>
#include <unistd.h>

#include "raylib.h"
#include "ship.h"
#include "screens.h"
#include "util.h"
#include "game.h"
#include "raymath.h"

int main() {
    //SetTraceLogLevel(7);
	//! Main window initialization
	InitMainWindow();

	//! Load game settings
	bool bgm_en;
	bgm_en = true;
	if(access("config.ini", F_OK) == 0){
		LoadSettings(&bgm_en);
	}
	//! Initalize Audio and start bgm
	InitAudioDevice();
	//start bgm
    //Music from #Uppbeat (free for Creators!):
    //https://uppbeat.io/t/studiokolomna/corsairs
    Music bgm = LoadMusicStream("resources/sound/music/corsairs-studiokolomna-main-version-23542-02-33.mp3");
	PlayMusicStream(bgm);
    //pause with StopMusicStream(bgm), resume with ResumeMusicStream(bgm);
	
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
	Texture2D rock_tex = LoadTexture("resources/sprites/rock.png");

	bool gen_obs = true;
	Obstacles obstacles;
	Ship* ship_list;
	int player_count = 2;
	int* player_count_addr = &player_count; // pass onto gamemode screen
	int type_list[8] = {0};
	int team_list[8] = {0};
	bool gen_ships;
	Ship_data ship_data;
	char real_or_turn;
	char* real_or_turn_addr = &real_or_turn;

	Sound game_sounds[3] = {fire, splash, explosion};
	Texture2D game_textures[2] = {heart_empty, heart_full};
	Model game_models[2] = {water_model, skybox_model};
	

	//Recalculate SkyBox bounds
	{
		game_bounds = GetMeshBoundingBox(skybox_model.meshes[0]);
		game_bounds.min = Vector3Scale(game_bounds.min, 900.0f);
		game_bounds.max = Vector3Scale(game_bounds.max, 900.0f);
	}

	// Reset ships-players
	// {
	// 	if(setjmp(reset_point)) ResetShipsState(&ship_data);
	// }
	//! Game loop
	while (!exit_window)
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
				DisplayGamemodesScreen(click, player_count_addr, real_or_turn_addr);
				break;
			}
			case SHIP_SELECT:
			{
				gen_obs = true;
				gen_ships = true;
				DisplayShipSelectScreen(click, &type_list[0], player_count, real_or_turn);
				break;
			}
			case TEAM_SELECT:
				DisplayTeamSelectScreen(click, &team_list[0], player_count, real_or_turn);
				break;
			case GAME_REAL:
			{
				if(gen_obs){
					obstacles = init_obs(sand_tex, rock_tex, palm_tree);
					gen_obs = false;
				}
				if(gen_ships){
					ship_data = CreateShipData(player_count, &type_list[0], &team_list[0], obstacles);
					gen_ships = false;
				}
				DisplayRealTimeGameScreen(ship_data, obstacles, game_models, game_sounds, game_textures); //Starts the real-time game
				break;
			}
			case GAME_TURN:
			{	
				if(gen_obs){
					obstacles = init_obs(sand_tex, rock_tex, palm_tree);
					gen_obs = false;
				}
				if(gen_ships){
					ship_data = CreateShipData(player_count, &type_list[0], &team_list[0], obstacles);
					gen_ships = false;
				}
				DisplayTurnBasedGameScreen(ship_data, obstacles, game_models, game_sounds, game_textures); //Starts the turn-based game
				break;
			}
			case GAME_MENU:
			{
				DisplayGameMenuScreen(click);
				break;
			}
			case GAME_OVER:
			{
				player_count = 2;
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
	UnloadMesh(water_cube);
	UnloadModel(water_model);
	UnloadModel(skybox_model);
	UnloadTexture(water_tex);
	UnloadTexture(skybox_texture);
	UnloadSound(click);
	UnloadSound(fire);
	UnloadSound(splash);
	UnloadSound(explosion);
	UnloadTexture(heart_full);
	UnloadTexture(heart_empty);
	UnloadTexture(sand_tex);
	UnloadTexture(rock_tex);
	UnloadModel(palm_tree);
	UnloadMusicStream(bgm);
	CloseAudioDevice();
	// TODO: add everything to 1 function
	// TODO: properly unload island and rock models (throws an error if not)
	return 0;
}
