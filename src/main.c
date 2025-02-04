//LICENSE
/*
Copyright (C) 2024 Panagiotis Skoulis, Dimitrios Kakagiannis

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

/* Import the required game headers (first party libraries) */
#include "ship.h"
#include "screens.h"
#include "util.h"
#include "game.h"
#include "anim.h"

/* Import the required game headers (third party libraries) */
#include "raylib.h"
#include "raymath.h"

/* Import the required tool headers (third party libraries) */
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

/* Defines the SkyBox's Bounding Box scaling factor */
#define BOUNDS_SCALAR 650.0f


//Main contains initialization of many variables passed down to other functions, the basic game loop, 
//and program de-initialization before process termination (freeing up resources).
int main() {
	/* Disable logs (except for fatal errors - code 6) for lag reduction */
    SetTraceLogLevel(6);

	/* Main window initialization */
	InitMainWindow();
	Image window_img = LoadImage("resources/N.png");
	SetWindowIcon(window_img);

	/* Load game settings */
	bool bgm_en;
	bgm_en = true;
	if(access("config.ini", F_OK) == 0){
		LoadSettings(&bgm_en);
	}
	if(settings.fullscreen) ToggleFullscreen();

	/* Initialize audio and start the background game music (bgm) */
	InitAudioDevice();
    //Music from #Uppbeat (free for Creators!):
    //https://uppbeat.io/t/studiokolomna/corsairs
    Music bgm = LoadMusicStream("resources/sound/music/corsairs-studiokolomna-main-version-23542-02-33.mp3");
	PlayMusicStream(bgm);

	/* Initialize models for rendering */
	Texture2D water_tex[6] = {LoadTexture("resources/sprites/water_anim/image_grid_24x24_01.png"), 
	LoadTexture("resources/sprites/water_anim/image_grid_24x24_02.png"),
	LoadTexture("resources/sprites/water_anim/image_grid_24x24_03.png"),
	LoadTexture("resources/sprites/water_anim/image_grid_24x24_04.png"),
	LoadTexture("resources/sprites/water_anim/image_grid_24x24_05.png"),
	LoadTexture("resources/sprites/water_anim/image_grid_24x24_06.png")};
	Mesh water_cube = GenMeshCube(300, 1, 300);
	Model water_model = LoadModelFromMesh(water_cube);
	water_model.materials[0].maps[MATERIAL_MAP_DIFFUSE].texture = water_tex[0];
	//skybox model
	Texture2D skybox_texture = LoadTexture("resources/sprites/sky.png");
	Material skybox_material = LoadMaterialDefault();
	skybox_material.maps[MATERIAL_MAP_DIFFUSE].texture = skybox_texture;
	Mesh skybox_cube = GenMeshCube(1, 1, 1);
	Model skybox_model = LoadModelFromMesh(skybox_cube);
	skybox_model.materials[0] = skybox_material;
	//ships and cannons
	Model ship1 = LoadModel("resources/models/ship1edited.glb");
	Model ship2 = LoadModel("resources/models/ship2edited.glb");
	Model cannon_stand = LoadModel("resources/models/cannon_stand.glb");
	Model cannon_rail = LoadModel("resources/models/cannon_rail.glb");
	Model ship_models[4] = {ship1, ship2, cannon_stand, cannon_rail};

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
	//https://www.cgtrader.com/free-3d-models/exterior/other/low-poly-palm-trees-162aed6c-6afd-4675-82d6-b36857c6b255
	Model palm_tree = LoadModel("resources/models/palm_tree.glb");
	Texture2D rock_tex = LoadTexture("resources/sprites/rock.png");

	/* Initialize runtime game settings */
	bool gen_obs = false;
	Obstacles obstacles;
	int player_count = 2;
	int* player_count_addr = &player_count; // pass onto gamemode screen
	int type_list[8] = {0};
	int team_list[8] = {0};
	bool gen_ships = false;
	char real_or_turn;
	char* real_or_turn_addr = &real_or_turn;

	/* Sounds */
	Sound game_sounds[3] = {fire, splash, explosion};
	Texture2D game_textures[2] = {heart_empty, heart_full};
	Model game_models[2] = {water_model, skybox_model};

	/* Animations */
	Animation explosion_anim = CreateAnim("resources/sprites/explosion_sheet.png", 8, 12, (Vector2){64, 64});
	Animation splash_anim = CreateAnim("resources/sprites/splash_sheet.png", 15, 8, (Vector2){64, 64});
	Animation anim_list[2] = {splash_anim, explosion_anim};

	/* Recalculate SkyBox's Bounding Box bounds (game bounds) */
	{
		game_bounds = GetMeshBoundingBox(skybox_model.meshes[0]);
		game_bounds.min = Vector3Scale(game_bounds.min, BOUNDS_SCALAR);
		game_bounds.max = Vector3Scale(game_bounds.max, BOUNDS_SCALAR);
	}

	/* ! Game loop ! */
	while (!exit_window)
	{
		if(WindowShouldClose()) exit_window = true; // Exit
		if(bgm_en) UpdateMusicStream(bgm);
		CheckFullscreenToggle();
		/* Begin screen rendering */
		switch (current_screen) {
			case MAIN: // Main screen
			{
				int default_teams[8] = {0};
				memcpy(team_list, default_teams, sizeof(team_list));
				for(int i = 0; i < ship_data.player_count; i++){
					ship_data.ship_list[i].team = team_list[i];
				}
				
				DisplayMainScreen(click, &obstacles, sand_tex, palm_tree, rock_tex); //Displays the game's MAIN screen
				break;
			}
			case GAMEMODES: // Gamemode selection screen
			{
				DisplayGamemodesScreen(click, player_count_addr, real_or_turn_addr);
				game_ended = false;
				break;
			}
			case SHIP_SELECT: // Ship selection screen
			{
				gen_obs = true;
				gen_ships = true;
				DisplayShipSelectScreen(click, &type_list[0], player_count, real_or_turn);
				break;
			}
			case TEAM_SELECT: // Team selection screen
			{
				DisplayTeamSelectScreen(click, &team_list[0], player_count, real_or_turn);
				break;
			}
			case GAME_REAL: // Real-time gameplay screen
			{
				if(gen_obs){
					obstacles = init_obs(sand_tex, rock_tex, palm_tree);
					gen_obs = false;
				}
				if(gen_ships){
					ship_data = CreateShipData(player_count, &type_list[0], &team_list[0], obstacles, ship_models);
					gen_ships = false;
				}
				gamemode = GAME_REAL;
				DisplayRealTimeGameScreen(ship_data, obstacles, game_models, game_sounds, game_textures, anim_list, water_tex); //Starts the real-time game
				break;
			}
			case GAME_TURN: // Turn-based gameplay screen
			{
				if(gen_obs){
					obstacles = init_obs(sand_tex, rock_tex, palm_tree);
					gen_obs = false;
				}
				if(gen_ships){
					ship_data = CreateShipData(player_count, &type_list[0], &team_list[0], obstacles, ship_models);
					gen_ships = false;
				}
				gamemode = GAME_TURN;
				DisplayTurnBasedGameScreen(ship_data, obstacles, game_models, game_sounds, game_textures, anim_list, water_tex); //Starts the turn-based game
				break;
			}
			case GAME_MENU: // Game menu screen (toggled with ESC)
			{
				DisplayGameMenuScreen(click, obstacles);
				break;
			}
			case GAME_OVER: // Game over screen
			{	
				int default_teams[8] = {0};
				memcpy(team_list, default_teams, sizeof(team_list));
				for(int i = 0; i < ship_data.player_count; i++){
					ship_data.ship_list[i].team = team_list[i];
				}
				
				player_count = 2;
				DisplayGameOverScreen(wintext, click); //Ends the game (game over)
				break;
			}
			case OPTIONS: // Options (settings) screen
			{	
				DisplayOptionsScreen(click, &bgm_en);
				break;
			}
			case CONTROLS: // Control settings screen
			{
				DisplayControlsScreen(click);
				break;
			}
			case ABOUT: // About screen
			{
				DisplayAboutScreen(click); //Credits and gameplay
				break;
			}
			default:
				break;
		}
	}
	/* Unload models and prepare to kill the program */
	UnloadModel(water_model);
	UnloadModel(ship1);
	UnloadModel(ship2);
	UnloadModel(cannon_stand);
	UnloadModel(cannon_rail);
	for(int i = 0; i < 6; i++){
		UnloadTexture(water_tex[i]);
	}
	UnloadTexture(skybox_texture);
	UnloadMaterial(skybox_material);
	UnloadMesh(skybox_cube);
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
	if (wintext != NULL) {
    	free(wintext);
    	wintext = NULL; // Set to NULL to avoid dangling pointer
	}
	CloseAudioDevice();
	DeinitMainWindow(); //Main window de-initialization
	UnloadImage(window_img);
	return 0;
}